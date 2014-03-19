/*
 * VisRenderPassCard.h
 * 
 * A VisRenderPassCard is a simple 1 x 1 card with a shader applied to it
 * that reads the output of a RenderPass. Card exists on the X,Y plane and has its lower
 * left corner at the origin.
 * 
 * RenderPasses can be "attached" to the shader's inputs through the constructor or the attachPass() function.
 * A RenderPass, the ID of the output (i.e. GL_COLOR_ATTACHMENT0_EXT), and the name of a shader texture variable 
 * must be given. 
 *
 *  Created on: Mar 27, 2011
 *      Author: tbabb
 */

#ifndef VISRENDERPASSCARD_H_
#define VISRENDERPASSCARD_H_

#include <tr1/unordered_map>
#include <string>
#include "VisBounded.h"
#include "RenderPass.h"
#include "Shader.h"

typedef struct RenderPassOutput {
    RenderPass *pass;
    int output_id;
} RenderPassOutput;

typedef std::tr1::unordered_map<std::string, RenderPassOutput> InputDictionary;

class VisRenderPassCard : virtual public VisBounded<double,3> {
public:
    
    InputDictionary input_passes; //shader input name -> (RenderPass, output_id)
    Shader shader;

    VisRenderPassCard(Shader shader);
    VisRenderPassCard(Shader shader, RenderPass *renderpass, std::string shader_input_name);
    VisRenderPassCard(Shader shader, RenderPass *renderpass, std::string color_input_name, std::string depth_input_name);
    VisRenderPassCard(Shader shader, RenderPass *renderpass, int output_id, std::string shader_input_name);
    virtual ~VisRenderPassCard();
    
    virtual void draw();
    virtual Rect3d bounds();
    bool attachPass(RenderPass *renderpass, int pass_output_id, std::string shader_input_name);
    
protected:
    
    //link the given texture to the shader variable called <varname>
    bool _sendTexture(std::string varname, Texture tex, int texNo);
    void _sendShaderParams();
    virtual void _additionalShaderParams(); //override to send things like uniforms, etc.
};

#endif /* VISRENDERPASSCARD_H_ */
