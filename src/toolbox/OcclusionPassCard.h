/*
 * OcclusionPassCard.h
 * 
 * Renders its pre-pass to on a 1x1 texture card, with ambient occlusion applied.
 * Supplied pass must have both a color and depth component.
 * 
 * This is a separate class so that additional information about the camera can be passed to the shader,
 * and so that setting up occlusion can be one simple operation.
 *
 *  Created on: Mar 30, 2011
 *      Author: tbabb
 */

//TODO: This class is having performance issues
//TODO: Would prefer to blur out the noise, but don't know how to get the GPU cycles for it

//options: 
//    nsamples
//    occlusion_only (no color)
//    strength/tint
//    tint
//    test about a hemisphere?
//      arbitrary shape?
//        sample the shape, and project the samples to image space?

#ifndef OCCLUSIONPASSCARD_H_
#define OCCLUSIONPASSCARD_H_

#include <vector>
#include "visible/VisRenderPassCard.h"
#include "Shader.h"

#define NUM_OCC_SAMPLES 64

class OcclusionPassCard : virtual public VisRenderPassCard{
public:
    OcclusionPassCard(RenderPass *prepass);
    OcclusionPassCard();
    virtual ~OcclusionPassCard();
    
    Vec3d tint;
    float worldScale;
    int nsamples;
protected:
    
    virtual void _additionalShaderParams();
    
    static void init_static();
    
    static Shader occShader;
    static float samples_array[NUM_OCC_SAMPLES * 3];
    static int   permute_array[NUM_OCC_SAMPLES];
    static bool initStatic;
    
};

#endif /* OCCLUSIONPASSCARD_H_ */
