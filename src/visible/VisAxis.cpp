/*
 * VisAxis.cpp
 *
 *  Created on: Mar 26, 2011
 *      Author: tbabb
 */

#include "glHeaders.h"
#include "VisAxis.h"

using namespace geom;

//structors
VisAxis::VisAxis() {}
VisAxis::~VisAxis() {}

void VisAxis::draw(){
    glPushAttrib(GL_CURRENT_BIT);
    glBegin(GL_LINES);
      glColor3d(1,0,0);
      glVertex3d(0,0,0);
      glVertex3d(1,0,0);
      
      glColor3d(0,1,0);
      glVertex3d(0,0,0);
      glVertex3d(0,1,0);
    
      glColor3d(0,0,1);
      glVertex3d(0,0,0);
      glVertex3d(0,0,1);
    glEnd();
    glPopAttrib();
}

Rect3d VisAxis::bounds(){
    return Rect3d(ZERO_VEC3d, ONE_VEC3d);
}
