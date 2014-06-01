/*
 * GLWindow.h
 *
 * A <GLWindow> is an object-oriented abstration of the rather ugly
 * GLUT windowing code. Each window holds a list of <Drawable> objects, which exist in
 * world space (i.e. are drawn after the Camera's transform). This list is kept in <scene>,
 * which may be modified directly.
 *
 * <GLWindow>s also accept <GUIListeners>, which will be notified when input events happen
 * to the parent <GLWindow>. Just add new listeners directly to <guiListeners>.
 *
 * Multiple <GLWindow>s may exist at a time. Note that showAll() never returns control; it
 * calls glutMainLoop().
 *
 *  Created on: Sep 16, 2010
 *      Author: tbabb
 */

//TODO: per IQ: render context should not be here; it should be in RenderPass;
//      we may want to render to multiple windows. or maybe we want to render
//      to texture. all we need is a function called setViewport() which can
//      be called from the window for live renderings.
// TODO: should not cbak to PickListeners from draw thread :(
// TODO: can we read the depth for picking at an earlier/better time?

#ifndef GLWINDOW_H_
#define GLWINDOW_H_

#include "glHeaders.h"
#include <GLUT/glut.h>
#include <vector>
#include <queue>
#include <string>
#include <boost/thread.hpp>
#include <tr1/unordered_map>

#include <geomc/linalg/Vec.h>
#include "Camera.h"
#include "Drawable.h"
#include "GUIListener.h"
#include "RenderPass.h"

using namespace geom;

class GLWindow {
private:
    struct PickEvt {
        Vec2d pt;
        double time;
        int button;
        int mods;
    };
    
    std::queue<PickEvt> picks;
    boost::mutex pickMutex;
    double last_mdlmtx[16];
    double last_prjmtx[16];
    bool   mtx_valid;
    
public:
	bool depthTest;
	int clearBuffers;

	std::string winname;
	int w;
	int h;
	int id;

    Camera cam;
    Vec4d clear_clr;
    std::vector<Drawable*> scene;
    std::vector<RenderPass*> prePasses;
    std::vector<GUIListener*> guiListeners;
    std::vector<PickListener*> pickListeners;

	//structors
	GLWindow();
	GLWindow(std::string name, int w, int h);
	GLWindow(int* argc, char** argv, std::string name, int w, int h);
	virtual ~GLWindow();

	//member functions
	virtual void init(int* argc, char **argv);
	virtual void draw();  //todo: make this protected; should only happen from redraw callback; this window might not have the draw state right now.
	void redraw();        //this is the one you want to call from elsewhere in the program, calls postRedisplay()
	void reshape(int ww, int hh);
	void setClearColor(Vec4d color);
	inline void setClearColor(Vec3d color) { setClearColor(Vec4d(color,1)); }
	bool removeObject(Drawable *obj);

	//public static funcs
	static void showAll();

protected:
    
    Vec3d unprojectClickInValidContext(const Vec2d &p, 
                                       double mdlmtx[16], 
                                       double prjmtx[16], 
                                       DepthCondition *cond);
    void  processPicks();
    
	//static variables
	static std::tr1::unordered_map<int, GLWindow*> window_objs;
	static int  mouse_state;

	//callbacks
	static void draw_callback();
	static void reshape_callback(int w, int h);
	static void mouse_callback(int button, int state, int x, int y);
	static void keyUp_callback(unsigned char key, int x, int y);
	static void keyDown_callback(unsigned char key, int x, int y);
	static void specialKeyDown_callback(int key, int x, int y);
	static void specialKeyUp_callback(int key, int x, int y);
	static void mouseMoveDown_callback(int x, int y);
	static void mouseMoveUp_callback(int x, int y);

	//protected static funcs
	static GLWindow* get_window_obj();

};

#endif /* GLWINDOW_H_ */
