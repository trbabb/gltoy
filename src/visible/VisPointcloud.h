/* 
 * File:   VisPointcloud.h
 * Author: tbabb
 *
 * Created on August 27, 2013, 10:53 PM
 */

#ifndef VISPOINTCLOUD_H
#define	VISPOINTCLOUD_H

#include "VisBounded.h"
#include <boost/shared_array.hpp>
#include <geomc/linalg/Vec.h>

class VisPointcloud : virtual public VisBounded<double,3> {
public:
    VisPointcloud(index_t npts, Vec3d *pts);
    virtual ~VisPointcloud();
    
    virtual void draw();
    virtual Rect3d bounds();
    
private:
    index_t npts;
    Rect3d bnd;
    boost::shared_array<Vec3d> points;
};

#endif	/* VISPOINTCLOUD_H */

