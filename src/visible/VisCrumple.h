/*
 * VisCrumple.h
 *
 * A random, crumpled ribbon-type shape, for testing.
 *
 *  Created on: Sep 24, 2010
 *      Author: tbabb
 */

#ifndef VISCRUMPLE_H_
#define VISCRUMPLE_H_

#include "VisBounded.h"
#include <geomc/linalg/Vec.h>

using namespace geom;

class VisCrumple : virtual public VisBounded<double,3> {
public:
	VisCrumple();
	VisCrumple(int npts, double spread=2.0);
	virtual ~VisCrumple();
	
	virtual void draw();
	virtual Rect3d bounds();

protected:
	Vec3d* pts;
	Rect3d bnds;
	int npts;
	double spread;
	
	void _init();
};

#endif /* VISCRUMPLE_H_ */
