/*
 * GUIListener.cpp
 *
 *  Created on: Sep 22, 2010
 *      Author: tbabb
 */

#include "GUIListener.h"

GUIListener::GUIListener() {
	//virtual function; do nothing
}

GUIListener::~GUIListener() {
	//virtual function; do nothing
}

bool GUIListener::mouseEvent(GLWindow* window, int button, bool down, int modifiers, int x, int y) {
	//do not consume the event
	return false;
}

bool GUIListener::mouseMotion(GLWindow* window, int x, int y, int buttons) {
	//do not consume the event
	return false;
}

bool GUIListener::keyEvent(GLWindow* window, unsigned char key, int keycode, bool down, bool special, int x, int y) {
	//do not consume the event
	return false;
}

bool GUIListener::windowReshaped(GLWindow* window, int w_new, int h_new) {
    // do not consume the event
    return false;
}

PickListener::PickListener() {}

PickListener::~PickListener() {}


bool PickListener::pointPicked(GLWindow* window, 
                               int button,
                               int modifiers,
                               double evt_time, 
                               geom::Vec2d window_pos, 
                               geom::Vec3d world_pos,
                               DepthCondition cond) {
    return false;
}
