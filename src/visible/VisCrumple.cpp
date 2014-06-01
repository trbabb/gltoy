/*
 * DrawableWidget.cpp
 *
 *  Created on: Sep 24, 2010
 *      Author: tbabb
 */

#include "glHeaders.h"
#include <limits>
#include "VisCrumple.h"
#include "VisBox3d.h"
#include <geomc/random/RandomTools.h>

using namespace geom;

VisCrumple::VisCrumple(int npts, double spread):npts(npts),spread(spread){
    _init();
}

VisCrumple::VisCrumple():npts(1600),spread(4.0){
	_init();
}

VisCrumple::~VisCrumple() {
	delete[] pts;
}

void VisCrumple::_init(){
    pts = new Vec3d[npts];
    Vec3d dt = Z_AXIS3d;
    Vec3d ctr = ZERO_VEC3d;
    Vec3d maxs = -std::numeric_limits<double>::infinity() * ONE_VEC3d;
    Vec3d mins =  std::numeric_limits<double>::infinity() * ONE_VEC3d;
    Sampler<double> rvecs;

    for (int i = 0; i < npts; i++){
        pts[i] = ctr + rvecs.solidball<3>(spread);
        dt    += rvecs.solidball<3>(0.125);
        dt     = dt.unit();
        ctr   += dt/8.0;
        maxs   = maxs.max(pts[i]);
        mins   = mins.min(pts[i]);
    }
    bnds = Rect3d(mins, maxs);
}

void VisCrumple::draw(){
	Vec3d ctr = bnds.getCenter();
	glPushMatrix();
	glTranslated(-ctr.x, -ctr.y, -ctr.z);
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < npts; i++){
		Vec3d v = pts[i];
		Vec3d c = v.unit() * 0.5 + ONE_VEC3d*0.5;
		glColor3d(c.x, c.y, c.z);
		glVertex3d(v.x, v.y, v.z);
	}
	glEnd();
	glColor3d(1, 0, 0);
	VisBox3d visbounds(bnds);
	visbounds.wireframe = true;
	//visbounds.draw();
	glPopMatrix();
}

Rect3d VisCrumple::bounds(){
    //we draw centered, so we should report that
    Rect3d bnds_new = bnds;
    bnds_new.setCenter(ZERO_VEC3d);
	return bnds_new;
}
