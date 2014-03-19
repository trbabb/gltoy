/*
 * VisBox3d.h
 *
 *  Created on: Oct 2, 2010
 *      Author: tbabb
 */

#ifndef VISBOX3d_H_
#define VISBOX3d_H_

#include "VisBounded.h"
#include <geomc/linalg/Vec.h>

class VisBox3d : virtual public VisBounded<double,3>, virtual public Rect3d {
public:
	bool wireframe;

	VisBox3d(Vec3d c1, Vec3d c2);
	VisBox3d(Rect3d box, bool wireframe=false);
	virtual ~VisBox3d();

	virtual void draw();
	virtual void draw_wireframe();
	virtual void draw_solid();
};

#endif /* VISBOX3d_H_ */
