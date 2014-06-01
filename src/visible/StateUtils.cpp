/*
 * StateUtils.cpp
 *
 *  Created on: Jul 11, 2011
 *      Author: tbabb
 */

#include "StateUtils.h"
#include "glHeaders.h"

void glDoSetColor3d(Vec3d c){
    glColor3d(c.x, c.y, c.z);
}

void glDoSetColor4d(Vec4d c){
    glColor4d(c.x, c.y, c.z, c.w);
}

void glDoTransform3d(AffineTransform3d tx3){
    glMultMatrixd(tx3.mat.begin());
}

void glDoTranslate3d(Vec3d tx){
    glTranslated(tx.x, tx.y, tx.z);
}

void glDoScale3d(Vec3d sx){
    glScaled(sx.x, sx.y, sx.z);
}

void glDoSetWireframe(bool wire){
    glPolygonMode(GL_FRONT_AND_BACK, wire?GL_LINE:GL_FILL);
}


VisTransform::VisTransform():obj(NULL){};

VisTransform::VisTransform(Drawable *obj):obj(obj){};

VisTransform::VisTransform(AffineTransform3d xf, Drawable *obj){
    VisTransform *vxf = dynamic_cast<VisTransform*>(obj);
    if (vxf != NULL){
        this->xf  = xf * vxf->xf;
        this->obj = vxf->obj;
    } else {
        this->xf = xf;
        this->obj = obj;
    }
}

VisTransform::~VisTransform(){};

void VisTransform::draw(){
    if (obj != NULL){
        glPushMatrix();
        glMultTransposeMatrixd(&(xf.mat[0][0]));
        obj->draw();
        glPopMatrix();
    }
}
