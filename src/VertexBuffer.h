 /*
 * VertexBuffer.h
 * 
 * It is SAFE to copy this class by value.
 *
 *  Created on: Jul 17, 2011
 *      Author: tbabb
 */

#ifndef VERTEXBUFFER_H_
#define VERTEXBUFFER_H_

#include <tr1/unordered_map>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <limits>
#include <algorithm>

#include "GLTypeInfo.h"
#include "Shader.h"

//TODO: Buffer objects not released upon destruction
//TODO: create constructors which accept buffer data

/* USAGE, from the perspective of a Drawable which uses a VertexBuffer:
 * 
 * build() {
 *     vbuf.setVertexData(...);
 *     vbuf.setIndexData(...);
 *     vbuf.defineAttribute("vertex", offsetof(T, field), GL_FLOAT, 3);
 *     vbuf.defineAttribute("color", ...);
 *     vbuf.defineAttribute("myAttribute", ...);
 * }
 * 
 * draw() {
 *    vbuf.use()
 *    
 *    Shader1.use();
 *    vbuf.bindShader(Shader1);
 *    vbuf.drawElements(GL_TRIANGLES, ...);
 *    
 *    Shader2.use();
 *    vbuf.bindShader(Shader2);
 *    vbuf.drawElements(GL_TRIANGLES, ...);
 *    
 *    // ...
 *    
 *    vbuf.disable();
 *  }
 */

/* SOMEDAY, in a subclass:
 * 
 * GprimPool.add("myobj", [[GL_TRIANGLES, start_idx, end_idx], ...], shader?)
 * GprimPool.add(box);
 * GprimPool.add(fish);
 * GprimPool.add(butt);
 * 
 * GprimPool.draw("fish");
 * ...or something.
 */

//SOMEDAY: redundant pointer calls with shaders is annoying. separate shader and builtin pointer calls?
//SOMEDAY: an optional dedicated dynamic attribute buffer
//SOMEDAY: per-element update with "dirty" list
//SOMEDAY: double buffering?

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

/*******************************
 * Attribute info container    *
 *******************************/

struct VertexAttribInfo {
    size_t offset;    //offset from start of host data structure
    GLint n_components; //1,2,3,4 ? i.e. are we a vector?
    GLenum datatype;  //GL_FLOAT? GL_INT? etc.
    GLuint texno; //GL_TEXTURE0? GL_TEXTURE1? etc.
};


typedef std::tr1::unordered_map<std::string, VertexAttribInfo> AttributeMap;

/*******************************
 * Shared vertex buffer object *
 *******************************/

//shared object:
template<typename T>
class VBOinfo {
public:
    VBOinfo(boost::shared_array<T> data, int n, GLenum rw_hint=GL_STATIC_DRAW, bool isIndex=false):
            id(0),
            n_elems(n),
            rw_hint(rw_hint),
            elemData(data),
            offsets(),
            isIndexData(isIndex),
            allocated(false),
            data_is_dirty(true) {
        //do nothing else
    }
    
    VBOinfo(bool isIndex=false, GLenum rw_hint=GL_STATIC_DRAW):id(0),
            n_elems(0),
            rw_hint(rw_hint),
            elemData((T*)NULL),
            offsets(),
            isIndexData(isIndex),
            allocated(false),
            data_is_dirty(false) {
        //do nothing else
    }
    
    void allocate() {
        if (!allocated) {
            glGenBuffers(1, &id);
            allocated = true;
        }
    }
    
    void sendOffsets(GLuint shad_prog_id) {
        if (!isIndexData && allocated) {
            glBindBuffer(GL_ARRAY_BUFFER, id);
            for (AttributeMap::iterator i = offsets.begin(); i != offsets.end(); i++) {
                std::string name = i->first;
                VertexAttribInfo info = i->second;
                size_t stride = sizeof(T);
                if (name == "vertex") {
                    glVertexPointer(info.n_components, info.datatype, stride, BUFFER_OFFSET(info.offset));
                } else if (name == "normal") {
                    glNormalPointer(info.datatype, stride, BUFFER_OFFSET(info.offset));
                } else if (name == "texcoord") {
                    glClientActiveTexture(GL_TEXTURE0 + info.texno);
                    glTexCoordPointer(info.n_components, info.datatype, stride, BUFFER_OFFSET(info.offset));
                } else if (name == "color") {
                    glColorPointer(info.n_components, info.datatype, stride, BUFFER_OFFSET(info.offset));
                } else if (shad_prog_id != 0) {
                    GLint attrib_loc = glGetAttribLocation(shad_prog_id, name.c_str());
                    glEnableVertexAttribArray(attrib_loc);
                    glVertexAttribPointer(attrib_loc, info.n_components, info.datatype, false, stride, BUFFER_OFFSET(info.offset));
                }
            }
        }
    }
    
    void enableClientStates() {
        if (isIndexData) return;
        if (offsets.count("normal")) {
            glEnableClientState(GL_NORMAL_ARRAY);
        }
        if (offsets.count("texcoord")) {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        }
        if (offsets.count("color")) {
            glEnableClientState(GL_COLOR_ARRAY);
        }
        glEnableClientState(GL_VERTEX_ARRAY);
    }
    
    void sendData() {
        if (allocated && data_is_dirty) {
            GLenum target = isIndexData ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
            glBindBuffer(target, id);
            glBufferData(target, n_elems * sizeof(T), elemData.get(), rw_hint);
            data_is_dirty = false;
            elemData.reset();
        }
    }
    
    void release() {
        if (allocated and id) {
            glDeleteBuffers(1, &id);
            allocated = false;
            data_is_dirty = true;
            id = 0;
        }
    }
    
    GLuint id;
    size_t n_elems;
    GLenum rw_hint;
    boost::shared_array<T> elemData;
    AttributeMap offsets;
    bool isIndexData;
    bool allocated;
    bool data_is_dirty;
};


/*******************************
 * User interface to VBOs      *
 *******************************/

template <typename T>
class VertexBuffer{
public:
    VertexBuffer(GLenum vtx_read_hint=GL_STATIC_DRAW, GLenum idx_read_hint=GL_STATIC_DRAW):
        vtx_buffer_info(new VBOinfo<T>(false, vtx_read_hint)),
        idx_buffer_int(new VBOinfo<GLuint>(true, idx_read_hint)),
        idx_buffer_short(new VBOinfo<GLushort>(true, idx_read_hint)),
        use_short_idxs(new bool),
        enable_idx(false) {
        *use_short_idxs = true;
    }
    
    virtual ~VertexBuffer() {
        //todo: cleanup self/free buffer
    }
    
    //define new vertex data. The GL state will be updated at the
    //next call to use().
    void setVertexData(boost::shared_array<T> data, int n) {
        vtx_buffer_info->elemData = data;
        vtx_buffer_info->n_elems = n;
        vtx_buffer_info->data_is_dirty = true;
    }
    
    //define new index data. The GL state will be updated at the
    //next call to use(). Accepts shorts for use with smaller vertex counts.
    void setIndexData(boost::shared_array<GLushort> idxs, int n) {
        idx_buffer_short->elemData = idxs;
        idx_buffer_short->n_elems = n;
        idx_buffer_short->data_is_dirty = true;
        
        //close up int buffer
        idx_buffer_int->elemData.reset();
        idx_buffer_int->data_is_dirty = false;
        
        *use_short_idxs = true;
        enable_idx = true;
    }
    
    //define new index data. The GL state will be updated at the
    //next call to use(). Accepts ints for use with larger vertex counts.
    void setIndexData(boost::shared_array<GLuint> idxs, int n) {
        idx_buffer_int->elemData = idxs;
        idx_buffer_int->n_elems = n;
        idx_buffer_int->data_is_dirty = true;
        
        //close up short buffer
        idx_buffer_short->elemData.reset();
        idx_buffer_short->data_is_dirty = false;
        
        *use_short_idxs = false;
        enable_idx = true;
    }
    
    //define new index data. The GL state will be updated at the
    //next call to use(). If no preference is given, the implementation
    //will try to supply the data to GL as ushorts, which are more efficient.
    //Larger vertex counts will fall back to ints.
    void setIndexData(std::vector<int> &idxs, int useShorts=-1) {
        //if maximum value in idxs fits into a short, use them; they are faster/smaller
        //also use shorts of user has explicitly asked us to
        if (useShorts > 0 || *std::max_element(idxs.begin(), idxs.end()) < std::numeric_limits<unsigned short>::max()) {
            boost::shared_array<GLushort> new_array = boost::shared_array<GLushort>(new GLushort[idxs.size()]);
            for (int i = 0; i < idxs.size(); i++) {
                new_array[i] = idxs[i];
            }
            this->setIndexData(new_array, idxs.size());
        } else  {
            boost::shared_array<GLuint> new_array = boost::shared_array<GLuint>(new GLuint[idxs.size()]);
            for (int i = 0; i < idxs.size(); i++) {
                new_array[i] = idxs[i];
            }
            this->setIndexData(new_array, idxs.size());
        }
    }
    
    //Declare the existence of a vertex parameter, and its offset within the host data
    //structure. Allowed property names include "vertex", "normal", "texcoord", "color", and 
    //the name of vertex-varying shader parameter. TexID tells us which OpenGL texture unit
    //to assign to, if the given attribute is "texcoord" (default is 0).
    void defineAttribute(std::string name, size_t offset, GLenum datatype=GL_FLOAT, GLint n_components=3, GLuint texid=0) {
        VertexAttribInfo vinfo = {offset, n_components, datatype, texid};
        this->vtx_buffer_info->offsets[name] = vinfo;
    }
    
    //Draw elements of kind geom_kind. Wraps glDrawRangeElements. We abstract this because
    //the data type of the index array is internal to this VertexBuffer object.
    void drawElements(GLenum geom_kind, int min_idx, int max_idx, unsigned int num_pts, unsigned int start_idx) {
        GLenum dtype = (*use_short_idxs) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
        glDrawRangeElements(geom_kind, min_idx, max_idx, num_pts, dtype, BUFFER_OFFSET(start_idx));
    }
    
    void drawElements(GLenum geom_kind, unsigned int num_pts, unsigned int start_idx=0) {
        GLenum dtype = (*use_short_idxs) ? GL_UNSIGNED_SHORT: GL_UNSIGNED_INT;
        glDrawElements(geom_kind, num_pts, dtype, BUFFER_OFFSET(start_idx));
    }
    
    //Send the vertex data to the card if it's dirty, and otherwise ensure that this
    //VertexBuffer is ready to use.
    void use() {
        glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
        vtx_buffer_info->allocate(); //allocate buffer ID if not done so yet
        vtx_buffer_info->sendData(); //only does something if data is dirty
        vtx_buffer_info->enableClientStates();
        if (enable_idx) {
            if (*use_short_idxs) {
                idx_buffer_short->allocate();
                idx_buffer_short->sendData();
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buffer_short->id);
            } else {
                idx_buffer_int->allocate();
                idx_buffer_int->sendData();
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buffer_int->id);
            }
        }
        vtx_buffer_info->sendOffsets(0);
    }
    
    //Update the state (re-bind attribute locations) to reflect the
    //use of a new shader.
    void bindShader(Shader shad) {
        vtx_buffer_info->sendOffsets(shad.getProgramID());
    }
    
    void disable() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glPopClientAttrib();
        //todo: disable client state?
    }
    
    void release() {
        vtx_buffer_info->release();
        idx_buffer_int->release();
        idx_buffer_short->release();
    }
    
    int getNumPoints() {
        return vtx_buffer_info->n_elems;
    }
    
    boost::shared_ptr<VBOinfo<T> >        vtx_buffer_info;
    boost::shared_ptr<VBOinfo<GLuint> >   idx_buffer_int;
    boost::shared_ptr<VBOinfo<GLushort> > idx_buffer_short;
    boost::shared_ptr<bool> use_short_idxs;  //this is a dumb way to do it, but oh well
    bool enable_idx;
};

#endif /* VERTEXBUFFER_H_ */
