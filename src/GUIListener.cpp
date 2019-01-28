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

bool GUIListener::mouseEvent(GLWindow* window, double x, double y, int button, bool down, int modifiers) {
	//do not consume the event
	return false;
}

bool GUIListener::mouseMotion(GLWindow* window, double x, double y, int button_masks) {
	//do not consume the event
	return false;
}

bool GUIListener::keyEvent(GLWindow* window, int keycode, bool down, int mods) {
	//do not consume the event
	return false;
}

bool GUIListener::charEvent(GLWindow* window, unsigned int unicode_pt) {
  return false;
}

bool GUIListener::windowReshaped(GLWindow* window, geom::Rect<double,2> window_shape) {
    // do not consume the event
    return false;
}

bool GUIListener::windowClosing(GLWindow* window) {
  // allow the window to close by default
  return true;
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
