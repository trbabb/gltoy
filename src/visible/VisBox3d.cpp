/*
 * Box.cpp
 *
 *  Created on: Oct 2, 2010
 *      Author: tbabb
 */

#include <OpenGL/gl.h>
#include "VisBox3d.h"
#include <geomc/linalg/Vec.h>

void draw_square(Vec2d c1, Vec2d c2, double z, int mode){
	glBegin(mode);
		glVertex3d(c1.x, c1.y, z);
		glVertex3d(c1.x, c2.y, z);
		glVertex3d(c2.x, c2.y, z);
		glVertex3d(c2.x, c1.y, z);
	glEnd();
}

/**********************************
 * Structors                      *
 **********************************/

VisBox3d::VisBox3d(Vec3d c1, Vec3d c2):Rect3d(c1,c2),wireframe(false) {
	//do nothing else
}

VisBox3d::VisBox3d(Rect3d box, bool wireframe):Rect<double,3>(box),wireframe(wireframe){
	//do nothing else
}

VisBox3d::~VisBox3d() {
	//do nothing
}

/**********************************
 * Methods                        *
 **********************************/

void VisBox3d::draw(){
	if (wireframe){
		this->draw_wireframe();
	} else {
		this->draw_solid();
	}
}

void VisBox3d::draw_solid(){
	glPushMatrix();
	Vec3d s = maxs - mins;
	glTranslated(mins.x, mins.y, mins.z);
	glScaled(s.x, s.y, s.z);
	//draw box sides
	int px[] = { 0, 1, 1,  0};
	int nx[] = { 0, 1, 0, -1};
	for (int i = 0; i < 4; i++){
		 //a strip would be more space efficient, but we cannot do per-face normals that way
		glBegin(GL_QUADS);
		  glNormal3d(nx[i], nx[(i+3)%4], 0);
		  glVertex3d(px[i], px[(i+3)%4], 0);
		  glVertex3d(px[i], px[(i+3)%4], 1);
		  glVertex3d(px[(i+1)%4], px[i], 1);
		  glVertex3d(px[(i+1)%4], px[i], 0);
		glEnd();
	}
	//draw top and bottom
	glBegin(GL_QUADS);
		glNormal3d(0,0,1);
		glVertex3d(0,0,1);
		glVertex3d(1,0,1);
		glVertex3d(1,1,1);
		glVertex3d(0,1,1);

		glNormal3d(0,0,-1);
		glVertex3d(0,0,0);
		glVertex3d(1,0,0);
		glVertex3d(1,1,0);
		glVertex3d(0,1,0);
	glEnd();
	glPopMatrix();

}

void VisBox3d::draw_wireframe(){
	Vec2d min2 = this->mins.resized<2>();
	Vec2d max2 = this->maxs.resized<2>();
	draw_square(min2, max2, mins.z, GL_LINE_LOOP);
	draw_square(min2, max2, maxs.z, GL_LINE_LOOP);
	for (int i = 0; i < 4; i++){
		double x = i & 1 ? mins.x : maxs.x;
		double y = i & 2 ? mins.y : maxs.y;
		glBegin(GL_LINES);
			glVertex3d(x, y, maxs.z);
			glVertex3d(x, y, mins.z);
		glEnd();
	}
}
