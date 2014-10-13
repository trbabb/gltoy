/*
 * Camera.h
 *
 *  Created on: Jun 30, 2010
 *      Author: tbabb
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <geomc/linalg/Vec.h>

// TODO: get cam matrices.
// TODO: this shit is cumbersome and I don't like it.

using namespace geom;

enum ProjectionMode {
    CAM_PERSPECTIVE,
    CAM_ORTHOGRAPHIC,
};

class Camera {
public:
	Camera(double aspect=1);
	virtual ~Camera();

	void   use();
	void   disable();
	void   getBillboardMatrix(float modelview[16]);
    AffineTransform3d getMatrix();
    
	Vec3d  getDirection() const;
    void   setDirection(const Vec3d &v);
    Vec3d  getCenterOfInterest() const;
    void   setCenterOfInterest(const Vec3d &p);  
    void   setFar(double zfar);
    double getFar() const;
    void   setNear(double znear);
    double getNear() const;
    void   setAspect(double aspect);
    double getAspect() const;
    void   setPosition(const Vec3d &position);
    Vec3d  getPosition() const;
    void   setProjectionMode(ProjectionMode mode);
    ProjectionMode getProjectionMode() const;
    void   setOrthoWidth(double orthoWidth);
    double getOrthoWidth() const;
    void   setUp(Vec3d up);
    Vec3d  getUp() const;
    void   setFov(double fov);
    double getFov() const;
    
    
protected:
    Vec3d  position;
    Vec3d  direction;
    Vec3d  up;
	double znear;
	double zfar;
	double aspect;
	bool   inUse;
    ProjectionMode mode;
    double fov;
    double orthoWidth;

};

#endif /* CAMERA_H_ */
