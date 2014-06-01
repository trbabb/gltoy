/*
 * VisBall.h
 *
 *  Created on: Oct 2, 2010
 *      Author: tbabb
 */

#ifndef VISBALL_H_
#define VISBALL_H_

#include "glHeaders.h"
#include "VisBounded.h"
#include <geomc/linalg/Vec.h>

using namespace geom;

class VisBall : public VisBounded<double,3> {
public:
	Vec3d pos;
	double r;
	int slices;
	int stacks;

	VisBall(Vec3d pos, double r, int slices, int stacks);
	VisBall(Vec3d pos, double r);
	VisBall();
	virtual ~VisBall();

	virtual void draw();
	virtual Rect3d bounds();
protected:
	GLUquadric* quadric;
};

#endif /* VISBALL_H_ */
