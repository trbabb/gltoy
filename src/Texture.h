/*
 * Texture.h
 *
 *  Created on: May 30, 2011
 *      Author: tbabb
 * 
 * This class is SAFE to copy by value.
 * 
 * CAVEATS:
 *   - input texture buffer data is not freed automatically
 *   - input texture buffer data is not read until first use of texture.
 *     - if this is a problem, consider adding copybuffer flag to Texture(Image)
 *     - would need to also tell TexInfo to delete[] buffer
 */

//TODO: textures not released.

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <boost/shared_ptr.hpp>
#include "glHeaders.h"
#include <sstream>

#include "GLTypeInfo.h"
#include "Image.h"

/*************************
 * TexInfo class         *
 *************************/

class TexInfo {
public: 
    TexInfo();
    TexInfo(const GLvoid *data, int w, int h, 
            GLenum internalformat, 
            GLenum datachannels,
            GLenum datatype);
    virtual ~TexInfo();
    void release();
    
    const GLvoid *data;
    int w, h;
    GLuint id;
    GLenum internalformat; //internal repr: GL_RGBA? GL_RGBA16F_ARB?
    GLenum datachannels;   //incoming data: RGB? RGBA? RED? LUMINANCE? ALPHA?
    GLenum datatype;       //incoming data: float or int, e.g.?
    GLint clamp_s;
    GLint clamp_t;
    GLint min_filter;
    GLint mag_filter;
    GLint modulate;
    bool isBuilt;
    bool isLoaded;
};

typedef boost::shared_ptr<TexInfo> TexInfoPtr;


/*************************
 * Texture Class         *
 *************************/

class Texture {
public:
    Texture(int w, int h);
    Texture(int w, int h, GLenum internalfmt, GLenum incomingFmt, bool doMipmaps=true);
    
    template <typename T, index_t N> 
    Texture(const Image<T,N>& img, GLenum internalfmt, bool doMipmaps=true) {
        load(img, internalfmt, doMipmaps);
    }
    
    virtual ~Texture();
    
    template <typename T, index_t N> 
    void load(const Image<T,N>& img, GLenum internalfmt, bool doMipmaps=true) {
        int format; //how is the incoming data packed?
        switch(N) {
            case 1:
                if (internalfmt == GL_ALPHA || internalfmt == GL_ALPHA4 ||
                        internalfmt == GL_ALPHA8 || internalfmt == GL_ALPHA12 ||
                        internalfmt == GL_ALPHA16 || internalfmt == GL_COMPRESSED_ALPHA ||
                        internalfmt == GL_ALPHA32F_ARB || internalfmt == GL_ALPHA16F_ARB){
                    format = GL_ALPHA;
                } else {
                    format = GL_LUMINANCE;
                }
                break;
            case 2:
                format = GL_LUMINANCE_ALPHA;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                std::stringstream ss;
                ss << "Cannot create a GL texture with " << N << "channels";
                throw GraphicsException(ss.str());
        }
        GLenum datatype = GlTypeInfo<T>::type; //GL_INT? GL_FLOAT? etc.
        _texinfo   = TexInfoPtr(new TexInfo((const GLvoid*)img.begin(), img.w(), img.h(), internalfmt, format, datatype));
        _doMipmaps = doMipmaps;
        
        if (doMipmaps) {
            _texinfo->min_filter = GL_LINEAR_MIPMAP_LINEAR;
        }
    }
    
    // get a pointer to the CPU-side image buffer.
    // (call reload() after changing).
    // it's on you to ask for the right type of data.
    template <typename T>
    T* data() { return (T*)_texinfo->data; }
    
    void setClamp(GLint mode);
    void setClamp(GLint mode_s, GLint mode_t);
    void setMinFilter(GLint filter);
    void setMagFilter(GLint filter);
    void setModulate(bool modulate);
    GLuint getID();
    Vec<index_t,2> dimensions() const;
    
    // re-send the texture buffer data on next use (e.g. because you altered it.)
    void reload();
    // activate this texture for use
    void use();
    // stop using this texture
    void disable();
    // init this texture
    void build();
    
private:
    void _reload();
    
    TexInfoPtr _texinfo;
    bool _doMipmaps;
};

#endif /* TEXTURE_H_ */
