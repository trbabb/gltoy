/*
 * Craft.cpp
 *
 *  Created on: Aug 5, 2010
 *      Author: tbabb
 */

#include <OpenGL/gl.h>
#include "visible/VisBox3d.h"
#include "Craft.h"

#define PI_CONST 3.14159265358979323846

//TODO: draw vectors: force, torque; velocity, spin
//TODO: draw "pointer" cone

/***************************
 * Craft Functions         *
 ***************************/

Craft::Craft(double m, MomentInertia *mi):
	m(m), x(0,0,0), v(0,0,0),
	mi(mi), orient(0, 0, 0, 1), spin(2.0){ //indentity rotation; no angular momentum
	//do nothing
}

Craft::~Craft() {
	// do nothing
}

void Craft::step(double t, double dt){
	if (dt == 0){
		return;
	}
	/*
	//Vec3d fAccum(0, 0, 0); //force
	//Vec3d tAccum(0, 0 ,0); //torque
	Forces summed;
	for (unsigned int i = 0; i < forceSystems.size(); i++){
		summed += forceSystems[i]->getForces(*this, t, dt);
	}

	//model acceleration as linearly changing between t-dt and t
	Vec3d a1 = fAccum / m;
	Vec3d a0 = fPrev / m;
	x += ((a1 - a0) / 6 + 0.5*a0)*dt*dt + v*dt;
	v +=  (a1 + a0) * dt;

	//model torque as linearly changing between t-dt and t
	//here is how this shit is supposed to work:
	//q`(t) = 0.5 * omega(t) * q(t)
	//L(t) = integral(torque, dt) = I(t) * omega(t)
	//I(t) = inertial_space_mi = q(t) * body_space_mi * q^-1(t)
	//I^-1(t) = q(t) * inverse_body_space_mi * q^-1(t)
	//L(t)_intertial = torque_0*t + t^2 * (torque_1 - torque_0) / torque_1
	//euler method: current state + derivative of state*time interval == too unstable (derivative easy to compute)
	//better method: euler halfway, sample derivative, use that.
	//some concerns: quaternion may go out of unit-space. This is an unhappy thing. can we define our operations such that this doesn't happen?
	//        instead of linear interpolation-- slerp??
	//ideal solution: Use mathematica to solve this shit fo reals. It is just linear transformations with differential equations. why so hard?
	Vec3d dL0 = mi->mi(tAccum); //angular accel.
	Vec3d dL1 = mi->mi(tPrev);  //should actually be matrix inverse-- this is only accurate for diagonal matrices.
	Vec3d dq  = ((dL1 - dL0) / 6 + 0.5*dL0)*dt*dt + spin*dt;
	spin     +=  (dL1 + dL0) * dt;
	orient    = qRotFromAxis(dq) ^ orient;

	//save forces for the next round
	previous_forces = summed;
	*/
}

void Craft::draw(){
	Vec4d rot = orient.rotToAxisAngle();
	glPushMatrix();
	glTranslated(x.x, x.y, x.z);
	glRotated(360*rot.w/(2*PI_CONST), rot.x, rot.y, rot.z);
    
	//create & draw crossbars
	Vec3d a(1, 0.05, 0.05);
	Vec3d b = a.swizzle("yxz");
	VisBox3d xbar(-a, a);
	VisBox3d ybar(-b, b);
	VisBox3d ctrBox(-0.125 * ONE_VEC3d, 0.125 * ONE_VEC3d);
    
	glColor3d(1,1,1);
	xbar.draw();
	ybar.draw();
	ctrBox.draw();

	glPopMatrix();

}
