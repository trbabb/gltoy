/*
 * GLWindow.cpp
 *
 *  Created on: Sep 16, 2010
 *      Author: tbabb
 */

#include "GLWindow.h"
#include "Timing.h"
#include <map>
#include <list>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <algorithm>


//CONSIDER: an ortho cam implementation and its manipulator are closely related.

//TODO: a State abstraction (like a camera or a shader or a transform or color or displaylist(?)), which can be added to a
//      Drawable object that wraps other Drawables.
//      maybe a little abstract-y, but perhaps someday once this project becomes more complex.


/****************************************
 * GLFW Callbacks                       *
 ****************************************/


void glfw_error(int error, const char* message) {
    // be dumb for now: just print the error
    std::cerr << "OpenGL Error: " << message << std::endl;
}


void glfw_window_resized(GLFWwindow* window, int width, int height) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h); // if x,y are ever nonzero, unprojectClick will also
                            // need to be updated.
    GLWindow* win = (GLWindow*) glfwGetWindowUserPointer(window);
    if (win) {
        win->reshaped(w,h);
        win->redraw();
    }
}

void glfw_window_needs_refresh(GLFWwindow* window) {
    GLWindow* win = (GLWindow*) glfwGetWindowUserPointer(window);
    if (win) {
        win->draw();
    }
}

void glfw_key_pressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
    GLWindow* win = (GLWindow*) glfwGetWindowUserPointer(window);
    if (win) {
        if (false and action == GLFW_REPEAT) {
            // action = GLFW_PRESS;
            return; // discard these events.
        }
        for (unsigned int i = 0; i < win->guiListeners.size(); i++){
            bool consume = win->guiListeners[i]->keyEvent(win, key, action == GLFW_PRESS or action == GLFW_REPEAT, mods);
            if (consume) break;
        }
    }
}

void glfw_char_generated(GLFWwindow* window, unsigned int unicode) {
    GLWindow* win = (GLWindow*) glfwGetWindowUserPointer(window);
    if (win) {
        for (unsigned int i = 0; i < win->guiListeners.size(); i++){
            bool consume = win->guiListeners[i]->charEvent(win, unicode);
            if (consume) break;
        }
    }
}

void glfw_mouse_moved(GLFWwindow* window, double x, double y) {
    GLWindow* win = (GLWindow*) glfwGetWindowUserPointer(window);
    if (win) {
        bool mb_left  = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)   == GLFW_PRESS;
        bool mb_mid   = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
        bool mb_right = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)  == GLFW_PRESS;
        int button_masks = (mb_left  * LEFT_MOUSE_MASK)   | 
                           (mb_mid   * MIDDLE_MOUSE_MASK) | 
                           (mb_right * RIGHT_MOUSE_MASK);
        for (unsigned int i = 0; i < win->guiListeners.size(); i++) {
            if (win->guiListeners[i]->mouseMotion(win, x, y, button_masks)) break;
        }
    }
}

void glfw_mouse_button(GLFWwindow* window, int button, int action, int mods) {
    GLWindow* win = (GLWindow*) glfwGetWindowUserPointer(window);
    if (win) {
        // query mouse position
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        
        // notify listeners
        for (unsigned int i = 0; i < win->guiListeners.size(); i++) {
            if (win->guiListeners[i]->mouseEvent(win, x, y, button, action == GLFW_PRESS, mods)) break;
        }
        // handle picking
        if (action == GLFW_PRESS and win->pickListeners.size() > 0) {
            // queue a pick event. we'll need to process this at the next frame
            // draw, when our gl context is valid.
            win->pickMutex.lock();
            double t = now();
            GLWindow::PickEvt e = { Vec2d(x, y), t, button, mods };
            win->picks.push(e);
            win->pickMutex.unlock();
            win->processPicks();
        }
    }
}


/****************************************
 * Global state / timers                *
 ****************************************/


typedef void (*timer_callback_fn_t)(void*);

struct timer_callback_t {
    timer_callback_fn_t fn; 
    void* payload;
};


// global variables:
std::multimap<double, timer_callback_t> timer_callbacks;
std::mutex callback_mutex;
std::list<GLWindow*> all_windows;
bool global_init = false;


void redraw_callback(void* payload) {
    GLWindow* window = (GLWindow*) payload;
    window->draw();
}


// redraw the given window after the given delay.
void redrawAfter(GLWindow* window, double delay_seconds) {
    postTimerCallback(delay_seconds, redraw_callback, window);
}


void postTimerCallback(double delay_seconds, void (*callback)(void*), void* payload) {
    timer_callback_t cbak = { callback, payload };
    callback_mutex.lock();
        timer_callbacks.insert(std::pair<double, timer_callback_t>(now() + delay_seconds, cbak));
    callback_mutex.unlock();
    
    // timeout might be set to wait for longer than the trigger time.
    // go process your events now (if any) and reset the timeout.
    // (where "now" means "after the end of the current event cycle")
    glfwPostEmptyEvent();
}


void GLWindow::showAll() {
    while (all_windows.size() > 0) {
        
        // check all windows for close events; redraw requests
        for (auto i = all_windows.begin(); i != all_windows.end(); /* post-increment within loop */) {
            GLWindow* glwin = *i;
            // close the window if necessary
            if (glfwWindowShouldClose(glwin->window)) {
                // close(), if successful, removes the window from the global list of windows
                // so we need to pre-increment our iterator, since it could be invalidated:
                auto i_next = std::next(i);
                if (glwin->close()) {
                    i = i_next;
                    continue;
                }
            }
            // window was not closed
            // handle picks before redrawing, as pick-handling migh affect what we draw.
            glwin->processPicks();
            
            // redraw if necessary
            if (glwin->needs_redraw) {
                glwin->draw();
            }
            // increment to next window
            ++i;
        }
        
        // handle any timers that are due
        while (timer_callbacks.size() > 0 and timer_callbacks.begin()->first <= now()) {
            callback_mutex.lock();
                auto i = timer_callbacks.begin();
                timer_callback_t cbak = i->second;
                timer_callbacks.erase(i);
            callback_mutex.unlock();
            
            cbak.fn(cbak.payload);
        }
        
        // schedule the next handling cycle
        callback_mutex.lock();
        double delay = 1; // run the loop at least once per sec
        if (timer_callbacks.size() > 0) {
            delay = timer_callbacks.begin()->first - now();
            callback_mutex.unlock();
        } else {
            callback_mutex.unlock();
        }
        // how long do we have to wait?
        if (delay > 0) {
            glfwWaitEventsTimeout(delay);
        }
    }
}


/****************************************
 * Structors                            *
 ****************************************/


GLWindow::GLWindow():
        winname("OpenGL_App"),
        window_shape(Vec2d(), Vec2d(800, 600)),
        cam(800 / (double)600){
    init();
}

GLWindow::GLWindow(std::string name, int w, int h):
        winname(name),
        window_shape(Vec2d(), Vec2d(w, h)),
        cam(w / (double)h) {
    init();
}

GLWindow::~GLWindow() {
    close();
}


/****************************************
 * Member Functions                     *
 ****************************************/


bool GLWindow::init() {
    mtx_valid    = false;
    depthTest    = true;
    clearBuffers = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    
    if (not global_init) {
        if (not glfwInit()) { return false; }
        glfwSetErrorCallback(glfw_error);
        global_init = true;
    }
    
    // enable antialiasing:
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    // create window
    Vec2i dims = (Vec2i)(this->window_shape.getDimensions());
    this->window = glfwCreateWindow(dims.x, dims.y, winname.c_str(), nullptr, nullptr);
    if (not this->window) return false;
    // so we can access this instance from GLFW callbacks:
    glfwSetWindowUserPointer(this->window, this);
    
    // register callbacks
    
    glfwSetFramebufferSizeCallback(this->window, glfw_window_resized);
    glfwSetWindowRefreshCallback(  this->window, glfw_window_needs_refresh);
    glfwSetKeyCallback(            this->window, glfw_key_pressed);
    glfwSetCharCallback(           this->window, glfw_char_generated);
    glfwSetCursorPosCallback(      this->window, glfw_mouse_moved);
    glfwSetMouseButtonCallback(    this->window, glfw_mouse_button);
    
    // enable this context
    glfwMakeContextCurrent(this->window);
    
    // prevent screen tearing; wait for current frame to finish blitting:
    glfwSwapInterval(1);
    
    // init extensions
    glewExperimental = true;
    if (glewInit() != GLEW_OK) return false;
    
    // necessary even after giving above multisample hint. 
    // the hint gives us a multisample-capable buffer;
    // this call actually enables multisampling.
    glEnable(GL_MULTISAMPLE);
    
    // set up the viewport
    // we query the size because the OS may have squished it to fit onscreen.
    int w, h;
    glfwGetFramebufferSize(this->window, &w, &h);
    this->reshaped(w, h);
    
    // now we're ready to accept refresh callbacks
    this->needs_redraw = true;
    all_windows.push_back(this);
    
    return true;
}


bool GLWindow::close() {
    if (this->window) {
        bool close_ok = true;
        for (auto i = this->guiListeners.begin(); i != this->guiListeners.end(); ++i) {
            close_ok = (*i)->windowClosing(this);
            if (not close_ok) break;
        }
        if (close_ok) {
            for (auto i = all_windows.begin(); i != all_windows.end(); ++i) {
                if (*i == this) {
                    all_windows.erase(i);
                    break;
                }
            }
            glfwDestroyWindow(this->window);
            this->window = nullptr;
            return true;
        }
    }
    return false;
}


void GLWindow::setClearColor(Vec4d clr){
    this->clear_clr = clr;
}


void GLWindow::draw(){
    // reset last draw request
    this->needs_redraw = false;
    
    // no draw if bogus context kthx
    if (not this->window) return;
    
    makeCurrent();
    
    //render pre-passes
    for (unsigned int i = 0; i < prePasses.size(); i++){
        prePasses[i]->render();
    }
    
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
    glfwSwapBuffers(this->window);
}


void GLWindow::redraw(){
    this->needs_redraw = true;
    glfwPostEmptyEvent();
}


// w and h are the framebuffer sizes
void GLWindow::reshaped(int w, int h){
    // these are screen coordinates (abstract; not pixels):
    int win_w, win_h;
    glfwGetWindowSize(this->window, &win_w, &win_h);
    this->window_shape = Rect<double, 2>(Vec2d(), Vec2d(win_w, win_h));
    
    cam.setAspect(w/(double)h);
    
    // notify listeners
    for (auto i = guiListeners.begin(); i != guiListeners.end(); i++) {
        if ((*i)->windowReshaped(this, this->window_shape)) break;
    }
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
    
    makeCurrent();
    
    pickMutex.lock();
    while (not picks.empty()) {
        PickEvt evt = picks.front();
        picks.pop();
        DepthCondition cond;
        Vec3d pt = unprojectClickInValidContext(evt.pt, &cond);
        for (auto i = pickListeners.begin(); i != pickListeners.end(); i++) {
            if ((*i)->pointPicked(this, evt.button, evt.mods, evt.time, evt.pt, pt, cond)) break;
        }
    }
    pickMutex.unlock();
}

void GLWindow::makeCurrent() {
    if (glfwGetCurrentContext() != this->window) {
        glfwMakeContextCurrent(this->window);
        // this needs to be called on every context change.
        // not necessary for mac/linux, but needed on windows.
        // (not dangerous to call more than once otherwise).
        glewInit();
    }
}

Vec3d GLWindow::unprojectClickInValidContext(
        const Vec2d &window_pt,
        DepthCondition* cond) {
    Vec3d outpt;
    int w, h;
    float z[9];
    
    // convert click coordinates (window space) to pixel coordinates
    glfwGetFramebufferSize(this->window, &w, &h);
    Rect<double,2> buffer_shape(Vec2d(), Vec2d(w, h));
    Vec2d win_coord = this->window_shape.unmap(window_pt);
    win_coord.y     = 1 - win_coord.y; // buffer is y-down.
    Vec2i buf_coord = (Vec2i)(buffer_shape.remap(win_coord) + Vec2d(0.5));
    
    glReadBuffer(GL_FRONT);
    glReadPixels(
        std::max(0L, buf_coord.x - 1), // reading a 3x3 rectangle;
        std::max(0L, buf_coord.y - 1), // start at the top corner
        3, 3,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        z);
    
    // find the depth value nearest to camera in the 3x3 click window
    float nearest = 1.0;
    for (index_t i = 0; i < 9; i++) if (z[i] < nearest) nearest = z[i];
    
    // issue: this will not be the exact point we clicked, because we may have gotten
    // our nearest depth value from a neighbor pixel. it would be strictly more correct
    // to record the pixel from whence our depth came and unproject from there, 
    // as if we had clicked it.
    int vp[4] = { 0, 0, w, h };
    gluUnProject(buf_coord.x, buf_coord.y, nearest, last_mdlmtx, last_prjmtx, vp, &outpt.x, &outpt.y, &outpt.z);
    if      (nearest == 1) *cond = DEPTH_FARPLANE;
    else if (nearest == 0) *cond = DEPTH_NEARPLANE;
    else                   *cond = DEPTH_NONE;
    
    
    return outpt;
}

