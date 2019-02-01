/*
 * Texture.cpp
 *
 *  Created on: May 30, 2011
 *      Author: tbabb
 */

#include <sstream>
#include "glHeaders.h"
#include "Texture.h"

//TODO: Shader->S=sendTexture(string varname, Texture &t, texNo=0)
//TODO: Test this in action
//TODO: TexParams may not have an effect after calling TexImage2d?

//SOMEDAY: Borders, sub-images, etc.

/*************************
 * Structors             *
 *************************/

TexInfo::TexInfo():isBuilt(false), isLoaded(false) {
    //do nothing. dummy structor
}

TexInfo::TexInfo(const GLvoid *data, int w, int h, 
        GLenum internalformat, GLenum datachannels,
        GLenum datatype):
            data(data),
            w(w), h(h),
            id(0),
            internalformat(internalformat),
            datachannels(datachannels),
            datatype(datatype),
            clamp_s(GL_CLAMP),
            clamp_t(GL_CLAMP),
            min_filter(GL_LINEAR),
            mag_filter(GL_LINEAR),
            modulate(true),
            isBuilt(false),
            isLoaded(false) {}  //do nothing else.

TexInfo::~TexInfo(){
    this->release();
}

// Texture Structors:

Texture::Texture(int w, int h):
        _texinfo(new TexInfo(NULL, w, h, GL_RGB,
                GL_RGB, GL_UNSIGNED_BYTE)),
        _doMipmaps(false) {
    if (_doMipmaps) {
        _texinfo->min_filter = GL_LINEAR_MIPMAP_LINEAR;
    }
}

Texture::Texture(int w, int h, GLenum internalfmt, GLenum incomingFmt, bool doMipmaps):
            _texinfo(new TexInfo(NULL, w, h, internalfmt,
                incomingFmt, GL_UNSIGNED_BYTE)),
            _doMipmaps(doMipmaps) {
    if (_doMipmaps) {
        _texinfo->min_filter = GL_LINEAR_MIPMAP_LINEAR;
    }
}

Texture::~Texture() {
    //opengl texture released on last destruction of TexInfo
}

/*************************
 * Functions             *
 *************************/

void Texture::setClamp(GLint mode){
    this->_texinfo->clamp_s = this->_texinfo->clamp_t = mode;
}

void Texture::setClamp(GLint mode_s, GLint mode_t){
    this->_texinfo->clamp_s = mode_s;
    this->_texinfo->clamp_t = mode_t;
}

void Texture::setMinFilter(GLint filter){
    this->_texinfo->min_filter = filter;
}

void Texture::setMagFilter(GLint filter){
    this->_texinfo->mag_filter = filter;
}

void Texture::setModulate(bool modulate){
    this->_texinfo->modulate = modulate;
}

GLuint Texture::getID(){
    return this->_texinfo->id;
}

void Texture::use(){
    TexInfo &t = *this->_texinfo;
    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);
    glEnable(GL_TEXTURE_2D);
    if (not _texinfo->isBuilt){
        this->build();
    } else if (not _texinfo->isLoaded) {
        this->_reload();
    } else {
        glBindTexture(GL_TEXTURE_2D, t.id);
    }
    if (t.modulate){
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    //TODO: unclear if these do anything after texture has been built
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, t.min_filter);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, t.mag_filter);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, t.clamp_s);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t.clamp_t);
}

void Texture::disable(){
    glPopAttrib();
}

void Texture::reload() {
    // dirty the `loaded` state. reload on next use.
    this->_texinfo->isLoaded = false;
}

void Texture::build(){
    TexInfo &t = *this->_texinfo;
    glGenTextures(1, &t.id);
    glBindTexture(GL_TEXTURE_2D, t.id);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, t.min_filter);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, t.mag_filter);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, t.clamp_s);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t.clamp_t);
    
    this->reload();
    
    this->_texinfo->isBuilt = true;
}


void Texture::_reload() {
    TexInfo& t = *this->_texinfo;
    glBindTexture(GL_TEXTURE_2D, t.id);
    if (this->_doMipmaps){
        gluBuild2DMipmaps(GL_TEXTURE_2D, t.internalformat, t.w, t.h, t.datachannels, t.datatype, t.data);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, t.internalformat, t.w, t.h, 0, t.datachannels, t.datatype, t.data);
    }
    this->_texinfo->isLoaded = true;
}


Vec<index_t,2> Texture::dimensions() const {
    Vec<index_t,2>(this->_texinfo->w, this->_texinfo->h);
}

/*************************
 * TexInfo Functions     *
 *************************/

void TexInfo::release(){
    //TODO: this
}
