/*
 * StateUtils.h
 * 
 * Some functions which can be passed to a VisFunc to set state.
 * 
 *  Created on: Jul 11, 2011
 *      Author: tbabb
 */

#ifndef STATEUTILS_H_
#define STATEUTILS_H_

#include <geomc/linalg/Vec.h>
#include <geomc/linalg/AffineTransform.h>

#include "Drawable.h"

using namespace geom;

void glDoSetColor3d(Vec3d c);
void glDoSetColor4d(Vec4d c);
void glDoTransform3d(AffineTransform3d tx3);
void glDoTranslate3d(Vec3d tx);
void glDoScale3d(Vec3d sx);
void glDoSetWireframe(bool wire);

class VisTransform : virtual public Drawable {
public:
    AffineTransform3d xf;
    Drawable *obj;
    
    VisTransform();
    VisTransform(Drawable *obj);
    VisTransform(AffineTransform3d xf, Drawable *obj);
    virtual ~VisTransform();
    
    virtual void draw();
};

#endif /* STATEUTILS_H_ */
