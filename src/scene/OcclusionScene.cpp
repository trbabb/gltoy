/*
 * OcclusionScene.cpp
 *
 *  Currently: A Crumple and a perlin heightfield, with a very noisy occ pass on top.
 *  
 *  Current questions: Why does all the noise magically smooth away when we are looking
 *  down on the perlin object?
 *
 *  Created on: May 28, 2011
 *      Author: tbabb
 */


#include <geomc/function/PerlinNoise.h>

#include "GLWindow.h"
#include "Manipulator.h"
#include "AnimTimer.h"
#include "RenderPass.h"
#include "Shader.h"

#include "toolbox/OcclusionPassCard.h"
#include "visible/VisAxis.h"
#include "visible/VisCrumple.h"
#include "visible/VisBall.h"
#include "visible/VisSurface.h"

#define RESOLUTION 1024

int main(int argc, char** argv){
    GLWindow win(&argc, argv, "OpenGL Toy Program", RESOLUTION, RESOLUTION);
    Camera prepass_cam;
    prepass_cam.setPosition(X_AXIS3d * 20);
    prepass_cam.setCenterOfInterest(ZERO_VEC3d);
    prepass_cam.setNear(1);
    prepass_cam.setFar(1000);
    prepass_cam.setUp(Z_AXIS3d);
    
    Camera& cam = win.cam;
    cam.setPosition(Z_AXIS3d * 0.5 + cam.getCenterOfInterest());
    cam.setCenterOfInterest(Vec3d(0.5,0.5,0));
    cam.setNear(0.1);
    Manipulator manip(&win, &prepass_cam); //manipulator will spin the pre-pass scene; not the card
    AnimTimer timer(&win);

    //scene
    ExprPtr(Vec2d,Vec3d) perlin = hypersurfacex(ExprPtr(Vec2d,double)(new PerlinNoiseExpr<double,2>()));
    VisCrumple crmp(1600, 4);
    VisAxis axes;
    VisSurface surf(perlin, Rect2d(ONE_VEC2d * -15, ONE_VEC2d * 15), Vec2i(150,150));
    RenderPass prepass = RenderPass(&prepass_cam, RESOLUTION, RESOLUTION, GL_RGB, true, true);
    prepass.scene_objs.push_back(&crmp);
    prepass.scene_objs.push_back(&axes);
    prepass.scene_objs.push_back(&surf);
    prepass.setClearColor(ONE_VEC4d);
    OcclusionPassCard card(&prepass);
    card.nsamples = 6;
    card.tint = Vec3d(0.1,0.13,0.18);
    card.worldScale = 1.6;
    win.prePasses.push_back(&prepass);
    win.scene.push_back(&card);
    
    timer.begin();
    win.showAll();
    return 0;
}
