/*
 * VisAxis.h
 *
 *  Created on: Mar 26, 2011
 *      Author: tbabb
 */

#ifndef VISAXIS_H_
#define VISAXIS_H_

#include "VisBounded.h"
#include <geomc/linalg/Vec.h>

class VisAxis : virtual public VisBounded<double,3> {
public:
    VisAxis();
    virtual ~VisAxis();
    
    virtual void draw();
    virtual Rect3d bounds();
};

#endif /* VISAXIS_H_ */
