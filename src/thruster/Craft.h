/*
 * Craft.h
 *
 *  Created on: Aug 5, 2010
 *      Author: tbabb
 */

#ifndef CRAFT_H_
#define CRAFT_H_

#include <vector>
#include <geomc/linalg/Vec.h>
#include <geomc/linalg/Quaternion.h>

#include "ForceSystem.h"
#include "MomentInertia.h"
#include "Drawable.h"

using namespace geom;
using namespace std;

class ForceSystem; //fwd decl
class Forces;

class Craft : public Drawable {
public:
	Craft(double mass, MomentInertia *mi);
	virtual ~Craft();

	//linear movement
	double m;
	Vec3d x;
	Vec3d v;

	//rotational movement
	MomentInertia *mi;
	Quatd orient;
	Vec3d spin;

	//acting forces
	vector<ForceSystem*> forceSystems;
	Forces prev_forces;
    //really, it would be good to store body-space and interial-space forces separately
	//for more smaerter interpolation. currently discussed, but not implemented, in ForceSystem.
	void step(double t, double dt);
	virtual void draw();
};

#endif /* CRAFT_H_ */
