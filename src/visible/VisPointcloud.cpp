/* 
 * File:   VisPointcloud.cpp
 * Author: tbabb
 * 
 * Created on August 27, 2013, 10:53 PM
 */

#include <limits>
#include "glHeaders.h"
#include "VisPointcloud.h"
#include "glHelpers.h"

VisPointcloud::VisPointcloud(index_t npts, Vec3d *pts):
        npts(npts),
        bnd(Vec3d(std::numeric_limits<double>::max()), Vec3d(std::numeric_limits<double>::min())),
        points(new Vec3d[npts]) {
    std::copy(pts, pts+npts, points.get());
    
    for (index_t i = 0; i < npts; i++) {
        bnd.extendTo(points[i]);
    }
}

Rect3d VisPointcloud::bounds() {
    return bnd;
}

void VisPointcloud::draw() {
    glBegin(GL_POINTS);
    for (index_t i = 0; i < npts; i++) {
        glVertex(points[i]);
    }
    glEnd();
}

VisPointcloud::~VisPointcloud() { }

