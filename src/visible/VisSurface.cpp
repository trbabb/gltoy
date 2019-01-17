/*
 * VisSurface.cpp
 *
 *  Created on: Apr 17, 2011
 *      Author: tbabb
 */

#include "VisSurface.h"

/****************************************
 * Structors                            *
 ****************************************/

SharedSurfaceInfo::SharedSurfaceInfo(Rect2d region, Vec2i dicing):
        region(region),
        dicing(dicing),
        vtx_buf_id(0),
        idx_buf_id(0),
        valid(false){
    //do nothing else
}

SharedSurfaceInfo::~SharedSurfaceInfo(){
    //todo: don't know what happens if OpenGL context isn't valid
    this->release();
}

VisSurface::VisSurface(ExprPtr(Vec2d,Vec3d) surface):
        surface(surface),
        sharedInfo(new SharedSurfaceInfo(Rect2d(ZERO_VEC2d, ONE_VEC2d), Vec2i(64,64))){
    //do nothing else
}

VisSurface::VisSurface(ExprPtr(Vec2d,Vec3d) surface, Rect2d region, Vec2i divisions):
        surface(surface),
        sharedInfo(new SharedSurfaceInfo(region, divisions)){
    //do nothing else
}

VisSurface::~VisSurface() {
    //do nothing: resource deallocation handled by shared_ptrs
}

/****************************************
 * Methods                              *
 ****************************************/

void VisSurface::build(){
    sharedInfo->release();
    
    glGenBuffers(1, &(sharedInfo->vtx_buf_id));
    glGenBuffers(1, &(sharedInfo->idx_buf_id));
    
    glBindBuffer(GL_ARRAY_BUFFER, sharedInfo->vtx_buf_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sharedInfo->idx_buf_id);
    
    //prepare for data generation
    Vec2i dicing   = sharedInfo->dicing;
    Vec2d dicingf  = Vec2d(dicing.x - 1, dicing.y - 1);
    int num_floats = dicing.x * dicing.y * 3;
    int num_idxs   = dicing.x * (dicing.y-1) * 2;
    float *vtxData = new float[num_floats];
    unsigned int *idxData = new unsigned int[num_idxs];
    
    //generate pt data
    for (int y = 0; y < dicing.y; y++){
        for (int x = 0; x < dicing.x; x++){
            Vec2d st = sharedInfo->region.remap(Vec2d(x,y) / dicingf);
            Vec3d pt = surface->eval(st);
            unsigned int vtx_num = (unsigned int)(dicing.x) * y + x;
            vtxData[3*vtx_num]   = pt.x;
            vtxData[3*vtx_num+1] = pt.y;
            vtxData[3*vtx_num+2] = pt.z;
            if (y < dicing.y - 1){
                idxData[vtx_num * 2] = vtx_num;
                idxData[vtx_num * 2 + 1] = vtx_num + dicing.x;
            }
        }
    }
    
    //TODO: normals (interleave; it's better)
    
    //send the data
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_floats, vtxData, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * num_idxs, idxData, GL_STATIC_DRAW);
    
    sharedInfo->valid = true;
    delete [] vtxData;
    delete [] idxData;
}

void VisSurface::draw(){
    if (!sharedInfo->valid){
        build();
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, sharedInfo->vtx_buf_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sharedInfo->idx_buf_id);
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    
    glColor3d(1,1,1);
    Vec2i dicing = sharedInfo->dicing;
    for (int strip = 0; strip < dicing.y - 1; strip++){
        GLuint offs = dicing.x * strip * 2 * sizeof(int);
        glDrawRangeElements(GL_TRIANGLE_STRIP, dicing.x * strip, (dicing.x + 2) * strip, dicing.x * 2, GL_UNSIGNED_INT, (char*)NULL + offs);
    }
    
    //clean up
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VisSurface::setRegion(Rect2d region){
    if (this->sharedInfo->region != region){
        this->sharedInfo->region = region;
        this->sharedInfo->valid = false; //flag for rebuild
    }
}

void VisSurface::setDivisions(Vec2i dicing){
    if (this->sharedInfo->dicing != dicing){
        this->sharedInfo->dicing = dicing;
        this->sharedInfo->valid = false; //flag for rebuild
    }
}

/****************************************
 * SharedSurfaceInfo Methods            *
 ****************************************/

void SharedSurfaceInfo::release(){
    glDeleteBuffers(1, &(this->idx_buf_id));
    glDeleteBuffers(1, &(this->vtx_buf_id));
    this->valid = false;
    this->idx_buf_id = 0;
    this->vtx_buf_id = 0;
}
