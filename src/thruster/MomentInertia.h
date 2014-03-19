/*
 * MomentInertia.h
 *
 *  Created on: Aug 5, 2010
 *      Author: tbabb
 */

#ifndef MOMENTINERTIA_H_
#define MOMENTINERTIA_H_

#include <geomc/linalg/Vec.h>

using namespace geom;

class MomentInertia {
public:
	MomentInertia();
	virtual ~MomentInertia();

	virtual double mi(Vec3d axis) = 0;
};

#endif /* MOMENTINERTIA_H_ */
