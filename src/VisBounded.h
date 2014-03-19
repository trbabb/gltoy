/*
 * VisBounded.h
 *
 *  Created on: Oct 8, 2010
 *      Author: tbabb
 */

#ifndef VISBOUNDED_H_
#define VISBOUNDED_H_

#include <geomc/shape/Bounded.h>
#include "Drawable.h"

using namespace geom;
template <typename T, int N>
class VisBounded : virtual public Bounded<T,N>, virtual public Drawable {
public:
	VisBounded(){
		//do nothing
	}
	virtual ~VisBounded() {
		// do nothing
	}
};

#endif /* VISBOUNDED_H_ */
