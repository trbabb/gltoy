/*
 * BoxMI.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: tbabb
 */

#include "BoxMI.h"

BoxMI::BoxMI(Vec3d dims, double m):dims(dims),m(m) {
	//do nothing;
}

BoxMI::~BoxMI() {
	// TODO Auto-generated destructor stub
}

double BoxMI::mi(Vec3d axis){
	double k = 1.0/12;
	Vec3d d2 = dims*dims;
	return axis.x*k*m*(d2.y + d2.z) +
		   axis.y*k*m*(d2.x + d2.z) +
		   axis.z*k*m*(d2.x + d2.y);
}
