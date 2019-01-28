/*
 * GUIListener.h
 *
 *  Created on: Sep 22, 2010
 *      Author: tbabb
 */

#ifndef GUILISTENER_H_
#define GUILISTENER_H_

#define LEFT_MOUSE_MASK   1
#define MIDDLE_MOUSE_MASK 2
#define RIGHT_MOUSE_MASK  4

#include <geomc/linalg/Vec.h>
#include <geomc/shape/Rect.h>

class GLWindow; //fwd decl

enum DepthCondition {
    DEPTH_NONE = 0,
    DEPTH_FARPLANE,
    DEPTH_NEARPLANE,
};

class GUIListener {
public:
	GUIListener();
	virtual ~GUIListener();

	// a listener function shall return TRUE if it consumed the event successfully.
    // returning FALSE will allow other listeners to respond to the same event.
	
	/** 
	 * Called when a mouse button is pressed or released
	 *  - window: the window that generated the event
     *  - x, y: window coordinates of the event. These are not pixels!
	 *  - button: which button changed (one of GLFW_MOUSE_BUTTON_{LEFT,MIDDLE,RIGHT}, or a numbered button).
	 *  - down: whether the button was pressed (true) or released (false)
	 *  - modifiers: bitflags for currently-pressed modifier keys; any of GLFW_MOD_{SHIFT,CONTROL,ALT,SUPER}
	 */
	virtual bool mouseEvent(GLWindow* window, double x, double y, int button, bool down, int modifiers);
	
	// Called when the mouse moves over the window
	//  - window: the window that generated the event
	//  - x, y: position of the cursor in window coordinates (+y is down). These are not pixels!
    //  - button_masks: bitmasks representing press state for each button: {LEFT,MIDDLE,RIGHT}_MOUSE_MASK
	virtual bool mouseMotion(GLWindow* window, double x, double y, int button_masks);
	
	// Called when a key is struck or released
	//  - window: the window that generated the even
	//  - keycode: a glfw key code
	//  - down: whether the key was just struck (true) or released (false)
	//  - mods: bitflags for modifier keys; any of GLFW_MOD_{SHIFT,CONTROL,ALT,SUPER}
	virtual bool keyEvent(GLWindow* window, int keycode, bool down, int mods);
    
    // Called when a character is generated. Accounts for key modifiers like capslock, 
    // shift; keyboard region; or special char input. Useful when you care about 
    // the semantic meaning rather than the physical key.
    virtual bool charEvent(GLWindow* window, unsigned int unicode_pt);
    
    // Called when the window changes dimension.
    virtual bool windowReshaped(GLWindow* window, geom::Rect<double,2> window_shape);
    
    // Called when the window is requested to be closed.
    // Return `false` if the window should not be closed.
    virtual bool windowClosing(GLWindow* window);
};

class PickListener {
public:
    PickListener();
    virtual ~PickListener();
    
    virtual bool pointPicked(GLWindow *window, 
                             int button, 
                             int modifiers, 
                             double evt_time, 
                             geom::Vec2d window_pos, 
                             geom::Vec3d world_pos,
                             DepthCondition cond);
};

#endif /* GUILISTENER_H_ */
