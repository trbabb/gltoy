/*
 * BoxMI.h
 *
 *  Created on: Aug 12, 2010
 *      Author: tbabb
 */

#ifndef BOXMI_H_
#define BOXMI_H_

#include "MomentInertia.h"

class BoxMI : public MomentInertia{
public:
	Vec3d dims;
	double m;
	BoxMI(Vec3d dims, double m);
	virtual ~BoxMI();

	virtual double mi(Vec3d axis);
};

#endif /* BOXMI_H_ */
