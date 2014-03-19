/*
 * Drawable.h
 *
 * A <Drawable> is simply an object which executes OpenGL code and affects
 * the OpenGL state. A <Drawable> might be a 3D object, or an entire scene, or a
 * light, or even a render setting.
 *
 *  Created on: Jun 27, 2010
 *      Author: tbabb
 */

#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include "glHelpers.h"

class Drawable {
public:
	Drawable();
	virtual ~Drawable();

	virtual void draw() = 0;
};

#endif /* DRAWABLE_H_ */
