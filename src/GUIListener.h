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

	//a listener function shall return TRUE if it consumed the event successfully
	
	/** 
	 * Called when a mouse button is pressed or released
	 *  - window: the window that generated the event
	 *  - button: which button changed (one of GLUT_{LEFT,MIDDLE,RIGHT}_BUTTON)
	 *  - state:  is the new button position down or up?
	 *  - modifiers: which special keys are held down right now?
	 *      bitwise OR of GLUT_ACTIVE_ALT, GLUT_ACTIVE_SHIFT, GLUT_ACTIVE_CTRL
	 *  - x and y: position of event in the window.
	 */
	virtual bool mouseEvent(GLWindow* window, int button, bool down, int modifiers, int x, int y);
	
	//Called when the mouse moves over the window
	//  - window: the window that generated the event
	//  - x, y: position of the cursor in the window (+y is up)
	//  - buttons: which buttons are down now: {LEFT,MIDDLE,RIGHT}_MASK ?
	virtual bool mouseMotion(GLWindow* window, int x, int y, int buttons);
	
	//Called when a key is struck or released
	//  - window: the window that generated the event
	//  - key: ASCII character of keystroke, if exists. Sensitive to modifiers; i.e. "SHIFT + a" returns "A".
	//  - keycode: integer GLUT code for a special, non-ascii key, like SHIFT or the arrowkeys. Mutually exclusive with <key>
	//  - down: was the key just pressed, or released?
	//  - special: was the keystroke a non-ascii key?
	//  - x, y: position of the mouse cursor when the key was struck. +y is up.
	virtual bool keyEvent(GLWindow* window, unsigned char key, int keycode, bool down, bool special, int x, int y);
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
