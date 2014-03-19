#include <iostream>
#include <OpenGL/gl.h>
#include <algorithm>

#include <geomc/function/PerlinNoise.h>
#include <geomc/random/RandomTools.h>

#include "GLWindow.h"
#include "Manipulator.h"
#include "AnimTimer.h"
#include "RenderPass.h"
#include "Shader.h"

#include "Image.h"
#include "Texture.h"
#include "visible/VisAxis.h"
#include "visible/VisTextureCard.h"
#include "visible/VisBall.h"
#include "visible/VisCallback.h"

#define RESOLUTION 1024

void enableAlpha(int dummy){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void setColor(Vec4d c){
    glColor4d(c.x, c.y, c.z, c.w);
}

void splat(Image<float,1> img){
    Vec2f ctr = Vec2f(img.w()/2.0, img.h()/2.0);
    for (int y = 0; y < img.h(); y++){
        for (int x = 0; x < img.w(); x++){
            Vec2f pt = Vec2d(x,y) - ctr;
            float mag = pt.mag() / (img.w()*0.5);
            mag = M_CLAMP(1-mag, 0.0f, 1.0f);
            img.set(x,y,mag*mag);
        }
    }
}

int main(int argc, char** argv){
    GLWindow win(&argc, argv, "OpenGL Toy Program", RESOLUTION, RESOLUTION);
    win.cam.at = ZERO_VEC3d;
    win.cam.pos = X_AXIS3d * 10;
    win.cam.znear = 0.1;
    win.cam.zfar  = 200;
    win.cam.up = Z_AXIS3d;
    
    Manipulator manip(&win, &win.cam);
    AnimTimer timer(&win);

    //gen texture
    ExprPtr(Vec2f,float) perlin = ExprPtr(Vec2f,float)(new PerlinNoiseExpr<float,2>()) * 0.5  + 0.5;
    Image<float,1> img = Image<float,1>(512,512);
    std::cout << "building texture..." << std::endl;
    img.fill(perlin, Rect2d(Vec2d(-15), Vec2d(15)), 1);
    std::cout << "done." << std::endl;
    
    Image<float,1> blob = Image<float,1>(128,128);
    splat(blob);

    //scene objects
    Texture tex = Texture(blob, GL_ALPHA, true);
    VisTextureCard tcard = VisTextureCard(tex);
    
    //build scene
    win.scene.push_back(new VisCallback<int>(&enableAlpha));
    win.scene.push_back(new VisCallback<Vec4d>(&setColor, Vec4d(0.09,1)));
    
    RandomVectors<double> rv;
    for (int i = 0; i < 100; i++){
        VisBall *b = new VisBall(rv.box(-ONE_VEC3d * 100, ONE_VEC3d * 100), rv.rng->rand<double>(0.01,10), 48, 24);
        win.scene.push_back(b);
    }

    win.scene.push_back(&tcard);
    
    win.setClearColor(Vec4d(0.11,0.11,0.11,1));
    
    timer.begin();
    win.showAll();
    return 0;
}
