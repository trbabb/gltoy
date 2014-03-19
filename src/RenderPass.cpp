/*
 * RenderPass.cpp
 *
 *  Created on: Mar 26, 2011
 *      Author: tbabb
 */

#include "RenderPass.h"

//all the color attachments there ever was
//we do this because glDrawBuffers() needs to be called at the beginning of each frame
//with a mapping of buffers to targets (e.g. ATTACHMENT0 <--> gl_FragColor[3])
//this class always does them in order, so there is no reason to ever allocate
//this array more than once.
GLuint RenderPass::attachmentIds16[16] = {
        GL_COLOR_ATTACHMENT0_EXT,  GL_COLOR_ATTACHMENT1_EXT,
        GL_COLOR_ATTACHMENT2_EXT,  GL_COLOR_ATTACHMENT3_EXT,
        GL_COLOR_ATTACHMENT4_EXT,  GL_COLOR_ATTACHMENT5_EXT,
        GL_COLOR_ATTACHMENT6_EXT,  GL_COLOR_ATTACHMENT7_EXT,
        GL_COLOR_ATTACHMENT8_EXT,  GL_COLOR_ATTACHMENT9_EXT,
        GL_COLOR_ATTACHMENT10_EXT, GL_COLOR_ATTACHMENT11_EXT,
        GL_COLOR_ATTACHMENT12_EXT, GL_COLOR_ATTACHMENT13_EXT,
        GL_COLOR_ATTACHMENT14_EXT, GL_COLOR_ATTACHMENT15_EXT};

/****************************************
 * Structors                            *
 ****************************************/

RenderPass::RenderPass(Camera *cam):
        cam(cam),
        _w(512),
        _h(512),
        _fbo_id(0),
        _n_color_outputs(1),
        _outputs(),
        _depth_output_tex(_w,_h, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, false), //this will never be used, since depth output is false
        _depth_output_buf(0),
        _do_depthtest(true),
        _do_depth_output(false),
        _is_built(false),
        _clearColor(ZERO_VEC4d){
    _outputs.push_back(Texture(_w,_h, GL_RGB, GL_RGB, false));
}

RenderPass::RenderPass(Camera *cam, int w, int h, 
            int n_color_outputs, int *data_types, 
            bool depth_output, bool depthTest):
        cam(cam),
        _w(w),
        _h(h),
        _fbo_id(0),
        _n_color_outputs(M_CLAMP(n_color_outputs, 0, 16)),
        _outputs(),
        _depth_output_tex(_w,_h, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, false), //never be used if depth output is false
        _depth_output_buf(0),
        _do_depthtest(depthTest),
        _do_depth_output(depth_output),
        _is_built(false),
        _clearColor(ZERO_VEC4d){
    
    for (int i = 0; i < _n_color_outputs; i++){
        _outputs.push_back(Texture(_w, _h, data_types[i], GL_RGB, false));
    }
}

RenderPass::RenderPass(Camera *cam, int w, int h,
            int data_type, bool depth_output, bool depthTest):
        cam(cam),
        _w(w),
        _h(h),
        _fbo_id(0),
        _n_color_outputs(1),
        _outputs(),
        _depth_output_tex(_w, _h, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, false), //never used if depth output is false
        _depth_output_buf(0),
        _do_depthtest(depthTest),
        _do_depth_output(depth_output),
        _is_built(false),
        _clearColor(ZERO_VEC4d){
    _outputs.push_back(Texture(_w, _h, data_type, GL_RGB, false));
}

RenderPass::~RenderPass() {
    glDeleteFramebuffersEXT(1, &_fbo_id);
    if (_do_depthtest && !_do_depth_output){
        glDeleteRenderbuffersEXT(1, &_depth_output_buf);
    }
    //this will crash if the destructor is called outside of a valid
    //opengl context. Need a place to register one-time actions for cleanup, etc.
}

/****************************************
 * Methods                              *
 ****************************************/

void RenderPass::build(){
    glGenFramebuffersEXT(1, &_fbo_id);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo_id);
    
    for (int i = 0; i < _n_color_outputs; i++){
        _outputs[i].build(); //binds to this texture
        GLuint tid = _outputs[i].getID();
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, tid, 0);
    }
    
    if (_do_depth_output){
        _depth_output_tex.build();
        GLenum dtex_id = _depth_output_tex.getID();
        
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, dtex_id, 0);
    } else if (_do_depthtest){
        glGenRenderbuffersEXT(1, &_depth_output_buf);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _depth_output_buf);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, _w, _h);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, _depth_output_buf);
    }
    
    int build_status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    _is_built = false;
    switch (build_status){
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            _is_built = true;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            std::cout << "Prepass build failed: Incomplete attachment" << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            std::cout << "Prepass build failed: Bad dimensions" << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            std::cout << "Prepass build failed: Missing attachment" << std::endl;
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            std::cout << "Prepass build failed: Unsupported framebuffer" << std::endl;
            break;
    }
}

void RenderPass::render(){
    //push, like, everything
    glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT);
    
    //build if not yet done so
    if (!_is_built){
        build();
        if (!_is_built){
            //ERROR. do not render
            glPopAttrib();
            return;
        }
    } else {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo_id);
    }
    
    //enable multiple rendering targets
    if (_n_color_outputs > 1){
        glDrawBuffers(_n_color_outputs, attachmentIds16);
    }
    
    //enable/disable depth testing; clear state for drawing
    int clearbits = 0;
    if (_do_depthtest){
        glEnable(GL_DEPTH_TEST);
        clearbits = clearbits | GL_DEPTH_BUFFER_BIT;
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    if (_n_color_outputs > 0){
        clearbits = clearbits | GL_COLOR_BUFFER_BIT;
    }
    glViewport(0,0,_w,_h);
    glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.w);
    glClear(clearbits);

    //render the scene
    cam->use();
    for (unsigned int i = 0; i < scene_objs.size(); i++){
        scene_objs[i]->draw();
    }
    cam->disable();
    
    //pop state
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glPopAttrib();
}

Texture RenderPass::getOutputTexture(int target){
    if (target == GL_DEPTH_ATTACHMENT_EXT){
        if (this->_do_depth_output){
            return this->_depth_output_tex;
        } else {
            throw GraphicsException("render pass has no depth output");
        }
    } else {
        int n = target - GL_COLOR_ATTACHMENT0_EXT;
        if (n >= 0 && n < this->_n_color_outputs){
            return this->_outputs[n];
        }
    }
    throw GraphicsException("render pass does not have requested output");
}

int RenderPass::getNumColorOutputs(){
    return this->_n_color_outputs;
}

bool RenderPass::hasOutput(int output){
    if (output == GL_DEPTH_ATTACHMENT_EXT){
        return this->_do_depth_output;
    } else {
        int i = output - GL_COLOR_ATTACHMENT0_EXT;
        return i >= 0 && i < this->_n_color_outputs;
    }
}

void RenderPass::setSize(int w, int h){
    //this is tricky. do we destroy everything? we will have to
    //re-gen and re-bind the textures. What happens to other
    //objects that are holding their IDs?
    //TODO: for now, do nothing
}

void RenderPass::setClearColor(Vec4d color){
    this->_clearColor = color;
}
