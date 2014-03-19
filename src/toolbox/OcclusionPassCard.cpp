/*
 * OcclusionPassCard.cpp
 *
 *  Created on: Mar 30, 2011
 *      Author: tbabb
 */

#include "OcclusionPassCard.h"
#include <geomc/random/RandomTools.h>

Shader OcclusionPassCard::occShader = shaderFromFile("Occlusion Pass", "shader/occlusion_pass");
bool   OcclusionPassCard::initStatic = false;
float  OcclusionPassCard::samples_array[NUM_OCC_SAMPLES * 3];
int    OcclusionPassCard::permute_array[NUM_OCC_SAMPLES];

/****************************************
 * Structors                            *
 ****************************************/

OcclusionPassCard::OcclusionPassCard(RenderPass *prepass):
        VisRenderPassCard(OcclusionPassCard::occShader),
        tint(ZERO_VEC3d),
        worldScale(10),
        nsamples(12){
    if (!initStatic){
        init_static();
    }
    attachPass(prepass, GL_COLOR_ATTACHMENT0_EXT, "color_input");
    attachPass(prepass, GL_DEPTH_ATTACHMENT_EXT, "depth_input");
}

OcclusionPassCard::OcclusionPassCard():
        VisRenderPassCard(OcclusionPassCard::occShader),
        tint(ZERO_VEC3d),
        worldScale(10),
        nsamples(12){
    if (!initStatic){
        init_static();
    }
    //do nothing else
}

OcclusionPassCard::~OcclusionPassCard() {
    //do nothing
}

/****************************************
 * Methods                              *
 ****************************************/

void OcclusionPassCard::init_static(){
    for (int i = 0; i < NUM_OCC_SAMPLES; i++){
        Vec3d v = Sampler<double>().solidball<3>();
        samples_array[i*3]     = v.x;
        samples_array[i*3 + 1] = v.y;
        samples_array[i*3 + 2] = v.z;
        permute_array[i] = i;
    }
    permute(permute_array, NUM_OCC_SAMPLES);
    initStatic = true;
}

void OcclusionPassCard::_additionalShaderParams(){
    RenderPass *pass = this->input_passes["color_input"].pass;
    glUniform1f(this->shader.getUniformID("z_near"), pass->cam->getNear());
    glUniform1f(this->shader.getUniformID("z_far"),  pass->cam->getFar());
    glUniform1f(this->shader.getUniformID("world_scale"), worldScale);
    glUniform4f(this->shader.getUniformID("tint"), tint.x, tint.y, tint.z, 1.0);
    glUniform1i(this->shader.getUniformID("nsamples"), nsamples);
    glUniform1i(this->shader.getUniformID("rnd_seed"), getRandom()->rand<int>(512));
    glUniform3fv(this->shader.getUniformID("samples"), NUM_OCC_SAMPLES, samples_array);
    glUniform1iv(this->shader.getUniformID("permute"), NUM_OCC_SAMPLES, permute_array);
}
