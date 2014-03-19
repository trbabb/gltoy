/*
 * VisRenderPassCard.cpp
 *
 *  Created on: Mar 27, 2011
 *      Author: tbabb
 */

#include "VisRenderPassCard.h"

/****************************************
 * Structors                            *
 ****************************************/

VisRenderPassCard::VisRenderPassCard(Shader shader):
        shader(shader){ 
    /* do nothing */ 
}

VisRenderPassCard::VisRenderPassCard(Shader shader, RenderPass *renderpass, std::string shader_input_name):
        shader(shader){
    attachPass(renderpass, GL_COLOR_ATTACHMENT0_EXT, shader_input_name);
}

VisRenderPassCard::VisRenderPassCard(Shader shader, RenderPass *renderpass, std::string color_input_name, std::string depth_input_name):
        shader(shader){
    attachPass(renderpass, GL_COLOR_ATTACHMENT0_EXT, color_input_name);
    attachPass(renderpass, GL_DEPTH_ATTACHMENT_EXT, depth_input_name);
}

VisRenderPassCard::VisRenderPassCard(Shader shader, RenderPass *renderpass, int output_id, std::string shader_input_name):
        shader(shader){
    attachPass(renderpass, output_id, shader_input_name);
}

VisRenderPassCard::~VisRenderPassCard() { 
    /* do nothing */ 
}

/****************************************
 * Methods                              *
 ****************************************/

void VisRenderPassCard::draw(){
    bool fail = false;
    glPushAttrib(GL_TEXTURE_BIT | GL_CURRENT_BIT);
    glEnable(GL_TEXTURE_2D);
    
    if (shader.use()){
        _sendShaderParams();
    } else {
        fail = true;
    }
    glColor3d(1,1,1);
    glBegin(GL_QUADS);
      glNormal3d(0,0,1);
      
      glTexCoord2d(0,0);
      glVertex2d(0,0);
      
      glTexCoord2d(1,0);
      glVertex2d(1,0);
      
      glTexCoord2d(1,1);
      glVertex2d(1,1);
      
      glTexCoord2d(0,1);
      glVertex2d(0,1);
    glEnd();
    if (!fail) shader.disable();
    glPopAttrib();
}

Rect3d VisRenderPassCard::bounds(){
    return Rect3d(Vec3d(0,0,-0.001), Vec3d(1,1,0.001));
}

bool VisRenderPassCard::attachPass(RenderPass *renderpass, int pass_output_id, std::string shader_input_name){
    RenderPassOutput outobj = {renderpass, pass_output_id};
    if (renderpass->hasOutput(pass_output_id)){ 
        input_passes[shader_input_name] = outobj;
        return true;
    } else {
        return false;
    }
}

bool VisRenderPassCard::_sendTexture(std::string varname, Texture tex, int texNo){
    int var_loc = shader.getUniformID(varname);
    int texID = tex.getID();
    if (var_loc != -1){
        glActiveTexture(GL_TEXTURE0 + texNo);
        glBindTexture(GL_TEXTURE_2D, texID);
        glUniform1i(var_loc, texNo);
        //TODO: check error state
        return true;
    } else {
        //TODO: should be an exception?
        std::cout << "WARNING: failure to link texture to variable '" << varname << "' "; 
        std::cout << "in shader " << shader.getName() << std::endl;
        return false;
    }
}

void VisRenderPassCard::_sendShaderParams(){
    int textures_used = 0;
    for (InputDictionary::iterator i = input_passes.begin(); i != input_passes.end(); i++){
        std::string name = i->first;
        RenderPassOutput output = i->second;
        
        //for this shader input, take the pass we've associated with it, get the output that we want (e.g. DEPTH or COLORn), and find its texture ID.
        this->_sendTexture(name, output.pass->getOutputTexture(output.output_id), textures_used++);
    }
    this->_additionalShaderParams();
}

void VisRenderPassCard::_additionalShaderParams(){
    //nothing for the base class
}
