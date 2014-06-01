/*
 * Camera.cpp
 *
 *  Created on: Jun 30, 2010
 *      Author: tbabb
 */

#include "glHeaders.h"
#include <GLUT/glut.h>
#include "Camera.h"

/**********************
 * Structors          *
 **********************/

Camera::Camera(double aspect):
            position(0,0,0),
            direction(0,0,1),
            up(0,0,1),
            znear(0.0001),
            zfar(10000),
            aspect(1),
            inUse(false),
            mode(CAM_PERSPECTIVE),
            fov(45),
            orthoWidth(1) {
	//do nothing else
}

Camera::~Camera() {
	//do nothing
}


/**********************
 * GL Stuff           *
 **********************/

void Camera::use() {
	if (inUse) disable();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (mode == CAM_PERSPECTIVE) {
        float x = tan(fov/2) * znear;
        // l,r,b,t are specified on the near plane.
        //        L   R   bottom    top       near   far
        glFrustum(-x, x, -x/aspect, x/aspect, znear, zfar);
    } else {
        double x = orthoWidth / 2;
        double y = x / aspect;
        glOrtho(-x, x, -y, y, znear, zfar);
    }
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
    Vec3d at = getCenterOfInterest();
	gluLookAt(position.x, position.y, position.z, at.x, at.y, at.z, up.x, up.y, up.z);
    
	inUse = true;
}

void Camera::disable() {
	glPopMatrix();
	inUse = false;
}

void Camera::getBillboardMatrix(float modelview[16]) {
	Vec3d right = direction.cross(up).unit();
	for (int i = 0; i < 3; i++) {
		Vec3d v;
		switch(i) {
			case 0:
				v = right;
				break;
			case 1:
				v = right.cross(direction).unit();
				break;
			case 2:
				v = direction.unit();
				break;
			case 3:
				v = ZERO_VEC3d;
				break;
		}
		modelview[i*4]     = v.x;
		modelview[i*4 + 1] = v.y;
		modelview[i*4 + 2] = v.z;
		modelview[i*4 + 3] = 0;
	}
	modelview[15] = 1;
}


/**********************
 * Accessors          *
 **********************/

Vec3d Camera::getDirection() const {
    return this->direction;
}

void Camera::setDirection(const Vec3d &v) {
    this->direction = v;
}

Vec3d Camera::getCenterOfInterest() const {
    return this->position + this->direction;
}

void Camera::setCenterOfInterest(const Vec3d &p) {
    this->direction = p - this->position;
}

void Camera::setFar(double zfar) {
    this->zfar = zfar;
}

double Camera::getFar() const {
    return zfar;
}

void Camera::setNear(double znear) {
    this->znear = znear;
}

double Camera::getNear() const {
    return znear;
}

void Camera::setAspect(double aspect) {
    this->aspect = aspect;
}

double Camera::getAspect() const {
    return aspect;
}

void Camera::setPosition(const Vec3d &position) {
    this->position = position;
}

Vec3d Camera::getPosition() const {
    return position;
}

void Camera::setProjectionMode(ProjectionMode mode) {
    this->mode = mode;
}

ProjectionMode Camera::getProjectionMode() const {
    return mode;
}

void Camera::setOrthoWidth(double orthoWidth) {
    this->orthoWidth = orthoWidth;
}

double Camera::getOrthoWidth() const {
    return orthoWidth;
}

void Camera::setUp(Vec3d up) {
    this->up = up;
}

Vec3d Camera::getUp() const {
    return up;
}

void Camera::setFov(double fov) {
    this->fov = fov;
}

double Camera::getFov() const {
    return fov;
}
