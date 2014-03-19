/*
 * VisSurface.h
 *
 * This class is SAFE to copy by value.
 * 
 *  Created on: Apr 17, 2011
 *      Author: tbabb
 */

//TODO: Re-write to use VertexBuffer.
//TODO: Normals.
//TODO: Switch between ushort and uint for idx type based on numpts
//todo: make bounded
//TODO: inherit from VertexBuffer to simplify.

//TODO: Optimizations:
//   - We are sending the x,y coordinates over to the card. this is potentially unnecessary
//   - Each strip is the same pattern of indecies as the previous strip, but with a constant offset. 
//     sending the indecies for every single strip is redundant.
//   - We use multiple strips. Could use a snake pattern to send only one strip

#ifndef VISSURFACE_H_
#define VISSURFACE_H_

#include <OpenGL/gl.h>

#include "boost/shared_ptr.hpp"
#include <geomc/function/Expr.h>
#include <geomc/shape/Rect.h>

#include "Drawable.h"

using namespace geom;

/****************************************
 * Shared resource class                *
 ****************************************/

class SharedSurfaceInfo {
public:
    SharedSurfaceInfo(Rect2d region, Vec2i dicing);
    virtual ~SharedSurfaceInfo();
    
    void release();
    
    Rect2d region;
    Vec2i dicing;
    GLuint vtx_buf_id;
    GLuint idx_buf_id;
    bool valid; //set to false to "dirty" the buffer and re-build from scratch
};

typedef boost::shared_ptr<SharedSurfaceInfo> SharedSurfaceInfoPtr;

/****************************************
 * VisSurface class                     *
 ****************************************/

class VisSurface : public virtual Drawable {
public:
    VisSurface(ExprPtr(Vec2d,Vec3d) surface);
    VisSurface(ExprPtr(Vec2d,Vec3d) surface, Rect2d region, Vec2i divisions);
    virtual ~VisSurface();
    
    void draw();
    void setRegion(Rect2d region);
    void setDivisions(Vec2i dicing);
    
protected:
    
    //will return with the vertex buffers bound
    void build();
    
    ExprPtr(Vec2d, Vec3d) surface;
    SharedSurfaceInfoPtr sharedInfo;
};

#endif /* VISSURFACE_H_ */
