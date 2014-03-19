/*
 * Manipulator.h
 *
 *  Created on: Sep 23, 2010
 *      Author: tbabb
 */

#ifndef MANIPULATOR_H_
#define MANIPULATOR_H_

#include <geomc/linalg/Vec.h>

#include "GUIListener.h"
#include "Camera.h"
#include "GLWindow.h"
#include "Drawable.h"
#include "visible/VisBall.h"

class Manipulator : public GUIListener, public PickListener, public Drawable {
public:
    Manipulator(GLWindow* window, Camera *cam);
	Manipulator(GLWindow* window);
	virtual ~Manipulator();

	virtual bool mouseEvent(GLWindow* window, int button, bool down, int modifiers, int x, int y);
	virtual bool mouseMotion(GLWindow* window, int x, int y, int buttons);
	virtual bool keyEvent(GLWindow* window, unsigned char keycode, int key, bool down, bool special, int x, int y);
    virtual bool pointPicked(GLWindow *window, int button, int modifiers, 
                             double evt_time, Vec2d window_pos, Vec3d world_pos, DepthCondition cond);
    virtual void draw();
	
	void computeOrientation(Camera &c, Vec3d &out_sph_coords, Quatd &axis_rotation);
	void updateCamera(Camera &c, Vec3d sph_coords, Quatd axis_to_z);

protected:
	Vec2d clickPos[6]; //indexed by button number. why are there six.
    double lastPickTime;
	Camera* cam;
	
	Quatd axis_to_z; // if we want to rotate around an axis that isn't z
	double scale;
	
	//polar: radius, elevation, azimuth
	Vec3d cur_polar;
	Vec3d click_polar;
    double click_orthowidth;
	
	void _init(GLWindow *win, Camera *cam);
};

#endif /* MANIPULATOR_H_ */
