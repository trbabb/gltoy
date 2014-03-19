/* 
 * File:   VisTransformed.h
 * Author: tbabb
 *
 * Created on August 28, 2013, 12:01 AM
 */

#ifndef VISTRANSFORMED_H
#define	VISTRANSFORMED_H

#include "Drawable.h"
#include <geomc/linalg/AffineTransform.h>

using namespace geom;

class VisTransformed : virtual public Drawable {
public:
    VisTransformed(Drawable *obj);
    VisTransformed(Drawable *obj, const AffineTransform<double,3> &xf);
    virtual ~VisTransformed();
    
    virtual void draw();
    
    AffineTransform<double,3> xf;
    
private:
    
    Drawable *obj;

};

#endif	/* VISTRANSFORMED_H */

