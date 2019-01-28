/*
 * Manipulator.cpp
 *
 * A <Manipulator> registers itself with a GLWindow, listens for input from it,
 * updates its camera, and posts a redisplay when motion is generated.
 *
 *  Created on: Sep 23, 2010
 *      Author: tbabb
 */

//TODO: reorient UP
//TODO: deal with redraw collisions if an animTimer is separately issuing redraw calls.
//      Could cause framerate to temporarily double when manipulation is happening
//      should probably have a flag: issueRedraw bool flag? optional animTimer* param,
//      to check if it has a pending redraw?
// TODO: Just rewrite this shit.

#include "Manipulator.h"
#include "visible/VisBall.h"
#include <limits>
#include <geomc/linalg/Quaternion.h>

Manipulator::Manipulator(GLWindow *window, Camera *cam) {
    _init(window, cam);
}

Manipulator::Manipulator(GLWindow *window) {
    _init(window, &(window->cam));
}

Manipulator::~Manipulator() {
	//do nothing
}

void Manipulator::_init(GLWindow* window, Camera *cam) {
    this->cam = cam;
    this->lastPickTime = -std::numeric_limits<double>::max();
    
    this->computeOrientation(*cam, this->cur_polar, this->axis_to_z);
    this->click_polar = this->cur_polar;
    this->scale = window->cam.getDirection().mag();
    window->guiListeners.push_back(this);
    window->pickListeners.push_back(this);
    this->window = window;
}

void Manipulator::computeOrientation(Camera &c, Vec3d &out_sph_coords, Quatd &axis_rotation) {
    Vec3d axis = c.getUp().unit() ^ Z_AXIS3d;
    axis_rotation = Quatd::rotFromAxisAngle(axis, asin(axis.mag()));
    out_sph_coords = (axis_rotation * c.getDirection()).toSpherical();
}

void Manipulator::updateCamera(Camera &c, Vec3d sph_coords, Quatd axis_to_z) {
    Quatd z_to_axis = axis_to_z.conj();
    Vec3d cam_look = z_to_axis * sph_coords.fromSpherical();
    Vec3d rotated_up = (-X_AXIS3d).rotate(Y_AXIS3d, sph_coords.y).rotate(Z_AXIS3d, sph_coords.z);
    Vec3d cam_at = c.getCenterOfInterest();
    c.setPosition(cam_at + cam_look);
    c.setCenterOfInterest(cam_at);
    c.setUp(z_to_axis * rotated_up);
    this->window->redraw();
}

bool Manipulator::mouseEvent(GLWindow* window, double x, double y, int button, bool down, int modifiers) {
	Vec2d wincoords = window->window_shape.unmap(Vec2d(x,y));
	clickPos[button] = wincoords;
	if (button == GLFW_MOUSE_BUTTON_LEFT && down) {
	    this->click_polar = this->cur_polar;
	} else if (button == GLFW_MOUSE_BUTTON_RIGHT && down) {
	    this->click_polar.x = this->cur_polar.x;
        this->click_orthowidth = this->cam->getOrthoWidth();
	}
	return false;
}

bool Manipulator::mouseMotion(GLWindow* window, double x, double y, int button_masks) {
    Vec2d wincoords = window->window_shape.unmap(Vec2d(x,y));
    bool updated = false;
    if (button_masks & LEFT_MOUSE_MASK) {
        Vec2d delt = this->clickPos[GLFW_MOUSE_BUTTON_LEFT] - wincoords;
        //polar == radius, elevation, azimuth about Z
        //todo: fmod
        this->cur_polar = this->click_polar + Vec3d(0, delt.y * 2.0 * PI, delt.x * 4.0 * PI);
        updated = true;
    }
    if (button_masks & RIGHT_MOUSE_MASK) {
        Vec2d delt = this->clickPos[GLFW_MOUSE_BUTTON_RIGHT] - wincoords;
        double scalefac = std::exp(delt.y * 2.3026);
        this->cur_polar.x = scalefac * this->click_polar.x;
        this->cam->setOrthoWidth(this->click_orthowidth * scalefac);
        updated = true;
    }
    if (updated) {
        this->cur_polar = this->cur_polar.clamp(Vec3d(0, 0, -INFINITY), Vec3d(INFINITY, PI, INFINITY));
        updateCamera(*(this->cam), this->cur_polar, this->axis_to_z);
    }
    
	//do not consume the event
	return false;
}

void Manipulator::draw() {
    // do nothing.
}

bool Manipulator::pointPicked(GLWindow* window, 
                              int button, 
                              int modifiers, 
                              double evt_time, 
                              Vec2d window_pos, 
                              Vec3d world_pos,
                              DepthCondition cond) {
    const double DOUBLECLICK_THESHOLD_SECS = 0.25;
    if (button == GLFW_MOUSE_BUTTON_LEFT){
        if (evt_time - lastPickTime < DOUBLECLICK_THESHOLD_SECS && cond == DEPTH_NONE) {
            Vec3d dir = cam->getDirection();
            cam->setPosition(world_pos - dir);
            lastPickTime = -std::numeric_limits<double>::max();
        } else {
            lastPickTime = evt_time;
        }
    }
    return false;
}
