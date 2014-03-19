/*
 * VisPath.cpp
 *
 *  Created on: Jul 15, 2011
 *      Author: tbabb
 */

#include <OpenGL/gl.h>
#include "VisPath.h"

VisPath::VisPath():
    path(new PathExpr<double,3>()), 
    range(0,1), 
    segs(1){
    //do nothing else
}

VisPath::VisPath(boost::shared_ptr< Expr<double, Vec3d> > p, Vec2d range, int segs):
        path(p), range(range), segs(segs){
    //do nothing else.
}

VisPath::VisPath(const Path3d &p, Vec2d range, int segs):
		path(new PathExpr<double,3>(p)), range(range), segs(segs) {}

VisPath::~VisPath() {}


void VisPath::draw(){
    double domain = range[1] - range[0];
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segs; i++){
        double t = ((i * domain) / segs) + range[0];
        Vec3d  v = path->eval(t);
        glVertex3d(v.x, v.y, v.z);
    }
    glEnd();
}
