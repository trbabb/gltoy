/*
 * GLWindow.h
 *
 * A <GLWindow> is an object-oriented abstration of the boilerplate
 * GLFW windowing code. Each window holds a list of <Drawable> objects, which exist in
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
#include <vector>
#include <queue>
#include <string>
#include <mutex>

#if __cplusplus <= 199711L
#include <tr1/unordered_map>
#define UMAP_NAMESPACE std::tr1
#else
#include <unordered_map>
#define UMAP_NAMESPACE std
#endif

#include <geomc/linalg/Vec.h>
#include "Camera.h"
#include "Drawable.h"
#include "GUIListener.h"
#include "RenderPass.h"

using namespace geom;

class GLWindow {
private:
    
    friend void glfw_mouse_button(GLFWwindow* window, int button, int action, int mods);
    friend void glfw_window_needs_refresh(GLFWwindow* window);
    friend void redraw_callback(void* payload);
    
    struct PickEvt {
        Vec2d  pt;
        double time;
        int    button;
        int    mods;
    };
    
    std::queue<PickEvt> picks;
    std::mutex pickMutex;
    double     last_mdlmtx[16];
    double     last_prjmtx[16];
    bool       mtx_valid;
    bool       needs_redraw;
    
public:
	bool depthTest;
	int  clearBuffers;

	std::string winname;
	GLFWwindow* window;
    Rect<double,2> window_shape;

    Camera cam;
    Vec4d clear_clr;
    std::vector<Drawable*>     scene;
    std::vector<RenderPass*>   prePasses;
    std::vector<GUIListener*>  guiListeners;
    std::vector<PickListener*> pickListeners;

	//structors
	GLWindow();
	GLWindow(std::string name, int w, int h);
	virtual ~GLWindow();

	//member functions
	bool init();
	void redraw();                  // request a redraw
	void reshaped(int ww, int hh);
	bool removeObject(Drawable* obj);
    bool close();                   // return true if the window was successfully closed
    void setClearColor(Vec4d color);
    inline void setClearColor(Vec3d color) { setClearColor(Vec4d(color,1)); }

	//public static funcs
	static void showAll();

protected:
    
    void  draw();
    void  processPicks();
    Vec3d unprojectClickInValidContext(const Vec2d &p, 
                                       double mdlmtx[16], 
                                       double prjmtx[16], 
                                       DepthCondition *cond);

};

void postTimerCallback(double delay_seconds, void (*callback)(void*), void* payload);
void redrawAfter(GLWindow* window, double delay_seconds);

#endif /* GLWINDOW_H_ */
