/* 
 * File:   VisTransformed.cpp
 * Author: tbabb
 * 
 * Created on August 28, 2013, 12:01 AM
 */

#include "VisTransformed.h"
#include <OpenGL/gl.h>

VisTransformed::VisTransformed(Drawable *obj) : obj(obj) { }

VisTransformed::VisTransformed(Drawable *obj, const AffineTransform<double,3> &xf) :
        xf(xf), obj(obj){}

VisTransformed::~VisTransformed() {}

void VisTransformed::draw() {
    glPushMatrix();
    glMultMatrix(xf.mat);
    obj->draw();
    glPopMatrix();
}

