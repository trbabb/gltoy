/*
 * glHelpers.h
 * 
 * Make OpenGL look compatible with geom.linalg.
 * gl{Vertex,Color,Translate,Scale,Rotate,etc.} functions are created
 * which accept geom.linalg objects. Functions are inline and should be fast.
 *
 *  Created on: Dec 27, 2012
 *      Author: tbabb
 */

#ifndef GLHELPERS_H_
#define GLHELPERS_H_

#include <geomc/linalg/Vec.h>
#include <geomc/linalg/Matrix.h>
#include <geomc/linalg/AffineTransform.h>
#include <OpenGL/gl.h>

namespace geom {

/////////////// Mult Matrix ///////////////

inline void glMultMatrix(const SimpleMatrix<double,4,4> &m) {
    SimpleMatrix<double,4,4> tx;
    transpose(&tx, m);
	glMultMatrixd(tx.begin());
}

inline void glMultMatrix(const SimpleMatrix<float,4,4> &m) {
    SimpleMatrix<float,4,4> tx;
    transpose(&tx, m);
	glMultMatrixf(tx.begin());
}

/////////////// Load Matrix ///////////////

inline void glLoadMatrix(SimpleMatrix<double,4,4> &m) {
    SimpleMatrix<double,4,4> d;
    transpose(&d, m);
    glLoadMatrixd(d.begin());
}

inline void glLoadMatrix(SimpleMatrix<float,4,4> &m) {
    SimpleMatrix<float,4,4> d;
    transpose(&d, m);
    glLoadMatrixf(d.begin());
}


/////////////// Vertex ///////////////

inline void glVertex(Vec2d v) {
    glVertex2d(v.x, v.y);
}

inline void glVertex(Vec2f v) {
    glVertex2f(v.x, v.y);
}

inline void glVertex(Vec3d v) {
	glVertex3d(v.x, v.y, v.z);
}

inline void glVertex(Vec4d v) {
	glVertex4d(v.x, v.y, v.z, v.w);
}

inline void glVertex(Vec3f v) {
	glVertex3f(v.x, v.y, v.z);
}

inline void glVertex(Vec4f v) {
	glVertex4f(v.x, v.y, v.z, v.w);
}

/////////////// Color ///////////////

inline void glColor(Vec4d v) {
	glColor4d(v.x, v.y, v.z, v.w);
}

inline void glColor(Vec4f v) {
	glColor4f(v.x, v.y, v.z, v.w);
}

inline void glColor(Vec3d v) {
	glColor3d(v.x, v.y, v.z);
}

inline void glColor(Vec3f v) {
	glColor3f(v.x, v.y, v.z);
}

/////////////// Translate ///////////////

inline void glTranslate(Vec3d v) {
	glTranslated(v.x, v.y, v.z);
}

inline void glTranslate(Vec3f v) {
	glTranslatef(v.x, v.y, v.z);
}

inline void glTranslate(Vec2d v) {
	glTranslated(v.x, v.y, 0);
}

inline void glTranslate(Vec2f v) {
	glTranslatef(v.x, v.y, 0);
}

/////////////// Rotate ///////////////

inline void glRotate(Quatd q) {
	SimpleMatrix<double, 4, 4> m;
	rotmat(&m,q);
	glMultMatrix(m);
}

inline void glRotate(Quatf q) {
	SimpleMatrix<float, 4, 4> m;
	rotmat(&m,q);
	glMultMatrix(m);
}

inline void glRotate(double angle, Vec3d axis) {
	glRotated(angle, axis.x, axis.y, axis.z);
}

inline void glRotate(float angle, Vec3f axis) {
	glRotatef(angle, axis.x, axis.y, axis.z);
}

/////////////// Scale ///////////////

inline void glScale(Vec3d v) {
	glScaled(v.x, v.y, v.z);
}

inline void glScale(Vec3f v) {
	glScalef(v.x, v.y, v.z);
}

inline void glScale(Vec2d v) {
	glScaled(v.x, v.y, 1);
}

inline void glScale(Vec2f v) {
	glScalef(v.x, v.y, 1);
}

}; /* end namespace geom */


#endif /* GLHELPERS_H_ */
