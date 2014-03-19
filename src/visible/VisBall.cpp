/*
 * VisBall.cpp
 *
 *  Created on: Oct 2, 2010
 *      Author: tbabb
 */

#include <OpenGL/gl.h>
#include "VisBall.h"

VisBall::VisBall(Vec3d pos, double r, int slices, int stacks):
		pos(pos),r(r),slices(slices),stacks(stacks){
	quadric = gluNewQuadric();
}

VisBall::VisBall(Vec3d pos, double r):pos(pos),r(r),slices(24), stacks(12){
	quadric = gluNewQuadric();
}

VisBall::VisBall():pos(ZERO_VEC3d),r(1.0), slices(24), stacks(12){
	quadric = gluNewQuadric();
}

VisBall::~VisBall() {
	gluDeleteQuadric(quadric);
}

void VisBall::draw(){
	glPushMatrix();
	glTranslated(pos.x, pos.y, pos.z);
	gluSphere(quadric, r, slices, stacks);
	glPopMatrix();
}

Rect3d VisBall::bounds(){
	return Rect3d::fromCenter(pos, ONE_VEC3d * r * 2);
}
