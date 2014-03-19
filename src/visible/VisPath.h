/*
 * VisPath.h
 *
 *  Created on: Jul 15, 2011
 *      Author: tbabb
 */

#ifndef VISPATH_H_
#define VISPATH_H_

#include <geomc/function/Expr.h>
#include <geomc/function/Path.h>
#include <geomc/linalg/Vec.h>
#include "Drawable.h"

using namespace geom;

class VisPath : virtual public Drawable {
public:
    VisPath();
    VisPath(boost::shared_ptr< Expr<double, Vec3d> > p, Vec2d range, int segs);
    VisPath(const Path3d &p, Vec2d range, int segs);
    virtual ~VisPath();
    
    void draw();
    
    boost::shared_ptr< Expr<double, Vec3d> > path;
    Vec2d range;
    int segs;
};

#endif /* VISPATH_H_ */
