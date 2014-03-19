/*
 * GLWindow.cpp
 *
 *  Created on: Sep 16, 2010
 *      Author: tbabb
 */

#include "GLWindow.h"
#include "Timing.h"
#include <cstdio>
#include <iostream>
#include <cstring>
#include <algorithm>

//TODO: should store DB of all currently pressed keys for querying?
//TODO: should abstract cumbersome KeyEvent obj?
//TODO: make disable keyRepeat modal somehow?
//TODO: Manipulator (give a GLWindow to it, it should register itself as a listener and reference the camera)

//CONSIDER: an ortho cam implementation and its manipulator are closely related.

//TODO: a State abstraction (like a camera or a shader or a transform or color or displaylist(?)), which can be added to a
//      Drawable object that wraps other Drawables.
//      maybe a little abstract-y, but perhaps someday once this project becomes more complex.

GLWindow::GLWindow():winname("OpenGL_App"),
		w(800),h(600),id(-1),cam(w/(double)h){
	int argc = 1;
	char *nm = new char[winname.length()+1];
	std::strncpy(nm, winname.c_str(), winname.length());
	nm[winname.length()] = '\0';
	init(&argc, &nm);
	delete [] nm;
}

GLWindow::GLWindow(std::string name, int w, int h):winname(name),
		w(w),h(h),id(-1),cam(w/(double)h){
	int argc = 1;
    char *nm = new char[winname.length()+1];
    std::strncpy(nm, winname.c_str(), winname.length());
    nm[winname.length()] = '\0';
    init(&argc, &nm);
    delete [] nm;
}

GLWindow::GLWindow(int* argc, char** argv, std::string name, int w, int h):
		winname(name),w(w),h(h),id(-1){
	init(argc, argv);
}

GLWindow::~GLWindow() {
	window_objs.erase(id);
}

/****************************************
 * Member Functions                     *
 ****************************************/

void GLWindow::init(int* argc, char** argv){
    mtx_valid = false;
	depthTest = true;
	clearBuffers = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

	glutInit(argc, argv); //should this happen more than once? maybe should be in showAll()?
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // | GLUT_MULTISAMPLE);
    glutInitDisplayString("double rgb=8 depth>=16 hidpi");
	glutInitWindowSize(w, h);
	glutIgnoreKeyRepeat(true); //this should probably not be here.
	id = glutCreateWindow(winname.c_str());
	
	glEnable(GL_MULTISAMPLE);

	//callbacks!
	glutDisplayFunc(draw_callback);
	glutReshapeFunc(reshape_callback);
	glutMouseFunc(mouse_callback);
	glutMotionFunc(mouseMoveDown_callback);
	glutPassiveMotionFunc(mouseMoveUp_callback);
	glutSpecialFunc(specialKeyDown_callback);
	glutSpecialUpFunc(specialKeyUp_callback);
	glutKeyboardFunc(keyDown_callback);
	glutKeyboardUpFunc(keyUp_callback);

	window_objs[id] = this;
}

void GLWindow::setClearColor(Vec4d clr){
    this->clear_clr = clr;
}

void GLWindow::draw(){
    //render pre-passes
    for (unsigned int i = 0; i < prePasses.size(); i++){
        prePasses[i]->render();
    }
    
    processPicks();
    
    //render the main pass
	if (depthTest){
		glEnable(GL_DEPTH_TEST);
	}
	glClearColor(clear_clr.x, clear_clr.y, clear_clr.z, clear_clr.w);
	glClear(clearBuffers);
	cam.use();
    
    glGetDoublev(GL_MODELVIEW_MATRIX,  last_mdlmtx);
    glGetDoublev(GL_PROJECTION_MATRIX, last_prjmtx);
    mtx_valid = true;
    
	for (unsigned int i = 0; i < scene.size(); i++){
		Drawable* d = scene[i];
		d->draw();
	}
	cam.disable();
	glutSwapBuffers();
}

void GLWindow::redraw(){
	int old_id = glutGetWindow();
	if (old_id != id && old_id != 0){
		glutSetWindow(id); //switch to the window associated with this GLWindow object
		glutPostRedisplay();
		glutSetWindow(old_id);
	} else {
		glutPostRedisplay();
	}
}

void GLWindow::reshape(int ww, int hh){
	w = ww;
	h = hh;
	glViewport(0,0,w,h); // if x,y are ever nonzero, unprojectClick will also
                         // need to be updated.
    cam.setAspect(w/(double)h);
}

bool GLWindow::removeObject(Drawable *obj){
    std::vector<Drawable*>::iterator i = std::remove(scene.begin(), scene.end(), obj);
    if (i != scene.end()){
        scene.erase(i, scene.end());
        return true;
    } else {
        return false;
    }
}

void GLWindow::processPicks() {
    if (pickListeners.empty() or not mtx_valid) {
        // nobody to notify; or state was bogus when picks were generated
        while (not picks.empty()) picks.pop();
        return;
    }
    
    pickMutex.lock();
    while (not picks.empty()) {
        PickEvt evt = picks.front();
        picks.pop();
        DepthCondition cond;
        Vec3d pt = unprojectClickInValidContext(evt.pt, last_mdlmtx, last_prjmtx, &cond);
        typedef std::vector<PickListener*>::iterator iter;
        for (iter i = pickListeners.begin(); i != pickListeners.end(); i++) {
            if ((*i)->pointPicked(this, evt.button, evt.mods, evt.time, evt.pt, pt, cond)) break;
        }
    }
    pickMutex.unlock();
}

Vec3d GLWindow::unprojectClickInValidContext(const Vec2d &p, 
                                             double mdlmtx[16],
                                             double prjmtx[16],
                                             DepthCondition *cond) {
    Vec3d outpt;
    float z;
    int x = p.x + 0.5;
    int y = this->h - (p.y + 0.5);
    int vp[4] = {0,0,w,h};
    glReadBuffer(GL_FRONT);
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
    gluUnProject(x, y, z, mdlmtx, prjmtx, vp, &outpt.x, &outpt.y, &outpt.z);
    if (z == 1) *cond      = DEPTH_FARPLANE;
    else if (z == 0) *cond = DEPTH_NEARPLANE;
    else *cond             = DEPTH_NONE;
    return outpt;
}

/****************************************
 * Window state callbacks               *
 ****************************************/

std::tr1::unordered_map<int, GLWindow*> GLWindow::window_objs;
int  GLWindow::mouse_state   = 0;

GLWindow* GLWindow::get_window_obj(){
	int id = glutGetWindow();
	std::tr1::unordered_map<int,GLWindow*>::iterator i = window_objs.find(id);
	if (i == window_objs.end() || id < 1){
		return NULL; //current window does not belong to any GLWindows.
	} else {
		return (*i).second;
	}
}

void GLWindow::draw_callback(){
	GLWindow* w = get_window_obj();
	if (w){
		w->draw();
	}
}

void GLWindow::reshape_callback(int w, int h){
	GLWindow* win = get_window_obj();
	if (win){
		win->reshape(w,h);
	}  
}

/****************************************
 * UI/mouse callbacks                   *
 ****************************************/

void GLWindow::mouse_callback(int button, int state, int x, int y){
	GLWindow* win = get_window_obj();
	//update mouse state
	int mask = 0;
	switch (button){
		case GLUT_LEFT_BUTTON:
			mask = LEFT_MOUSE_MASK;
			break;
		case GLUT_MIDDLE_BUTTON:
			mask = MIDDLE_MOUSE_MASK;
			break;
		case GLUT_RIGHT_BUTTON:
			mask = RIGHT_MOUSE_MASK;
			break;
	}
	mouse_state = (mouse_state & ~mask) | ((state == GLUT_DOWN) * mask);

	//notify listeners
	int mods = glutGetModifiers();
	if (win){
		for (unsigned int i = 0; i < win->guiListeners.size(); i++){
			win->guiListeners[i]->mouseEvent(win, button, state == GLUT_DOWN, mods, x, y);
		}
        if (state == GLUT_DOWN and win->pickListeners.size() > 0) {
            // queue a pick event. we'll need to process this at the next frame
            // draw, when our gl context is valid.
            win->pickMutex.lock();
            double t = now();
            PickEvt e = {Vec2d(x, y), t, button, mods};
            win->picks.push(e);
            win->pickMutex.unlock();
        }
	}
}

void GLWindow::keyDown_callback(unsigned char key, int x, int y){
    GLWindow* win = get_window_obj();
    if (win){
        for (unsigned int i = 0; i < win->guiListeners.size(); i++){
            win->guiListeners[i]->keyEvent(win, key, 0, true, false, x, y);
        }
    }
}

void GLWindow::keyUp_callback(unsigned char key, int x, int y){
    GLWindow* win = get_window_obj();
    if (win){
        for (unsigned int i = 0; i < win->guiListeners.size(); i++){
            win->guiListeners[i]->keyEvent(win, key, 0, false, false, x, y);
        }
    }
}

void GLWindow::specialKeyDown_callback(int keycode, int x, int y){
	GLWindow* win = get_window_obj();
	if (win){
		for (unsigned int i = 0; i < win->guiListeners.size(); i++){
			win->guiListeners[i]->keyEvent(win, '\0', keycode, true, true, x, y);
		}
	}
}

void GLWindow::specialKeyUp_callback(int keycode, int x, int y){
	GLWindow* win = get_window_obj();
	if (win){
		for (unsigned int i = 0; i < win->guiListeners.size(); i++){
			win->guiListeners[i]->keyEvent(win, '\0', keycode, false, true, x, y);
		}
	}
}

void GLWindow::mouseMoveDown_callback(int x, int y){
	GLWindow* win = get_window_obj();
	if (win){
		for (unsigned int i = 0; i < win->guiListeners.size(); i++){
			win->guiListeners[i]->mouseMotion(win, x, y, mouse_state);
		}
	}
}

void GLWindow::mouseMoveUp_callback(int x, int y){
	GLWindow* win = get_window_obj();
	if (win){
		for (unsigned int i = 0; i < win->guiListeners.size(); i++){
			win->guiListeners[i]->mouseMotion(win, x, y, mouse_state);
		}
	}
}

void GLWindow::showAll(){
	glutMainLoop();
}
