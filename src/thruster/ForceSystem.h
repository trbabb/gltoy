/*
 * ForceSystem.h
 * 
 * Not sure what I was thinking here. this seems way too abstracted.
 * 
 * what about: Craft calls ForceSystem.applyForces(), and ForceSystem calls craft.ApplyBodySpace() or craft.ApplyInertialSpace()?
 *
 *  Created on: Aug 10, 2010
 *      Author: tbabb
 */

#ifndef FORCESYSTEM_H_
#define FORCESYSTEM_H_

#include <geomc/linalg/Vec.h>

using namespace geom;

class Craft; //fwd decl

class Forces {
public:
	Vec3d force_body;
	Vec3d torque_body;
	
	Vec3d force_inertial;
	Vec3d torque_inertial;

	void applyForce(Vec3d loc, Vec3d f);
	void applyForceBodySpace(Vec3d loc, Vec3d f); //ideally this would allow Craft to integrate body space forces more smaerter.
											      //but for the time being it could just as well xform to intertial space and accum
	void applyTorque(Vec3d loc, Vec3d t);
	void applyTorqueBodySpace(Vec3d loc, Vec3d t);
	
	Forces  operator+ (const Forces &fs);
    Forces& operator+=(const Forces& fs);
	
	/*
	 * More about what I mean by "smart body space force integration".
	 * Consider a body with a rocket on its side, affixed pointing tangentially. At time t the rocket/force is aligned with y+.
	 * At time t+dt, the body is rotated 180 degrees, orienting the rocket/force with y-. If we assume linear
	 * change in force between t and dt in inertial space, then the force will have passed linearly through 0. If we assume linear
	 * change in body space, then there will be *no* change in body space (correct), and a rotating force in
	 * interial space. Comprende?
	 */
};

class ForceSystem {
public:
	ForceSystem();
	virtual ~ForceSystem();

	virtual Forces getForces(Craft &c, double t, double dt) = 0;
};

#endif /* FORCESYSTEM_H_ */
