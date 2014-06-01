/*
 * SoundScene.cpp
 *
 *  Created on: Jun 19, 2011
 *      Author: tbabb
 */

#include <iostream>
#include "glHeaders.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include <geomc/function/PerlinNoise.h>
#include <geomc/random/RandomTools.h>

#include "GLWindow.h"
#include "Manipulator.h"
#include "AnimTimer.h"
#include "RenderPass.h"
#include "Shader.h"

#include "Image.h"
#include "Texture.h"
#include "visible/StateUtils.h"
#include "visible/VisAxis.h"
#include "visible/VisTextureCard.h"
#include "visible/VisBall.h"
#include "visible/VisCallback.h"
#include "sound/WavBuffer.h"

#define RESOLUTION 1024

typedef struct RenderChannel {
    WavBuffer *wb;
    int chan;
    Vec3d color;
} RenderChannel;


void effectCallback(int chan, void *sounddata, int len, void *userdata){
    WavBuffer *wavbuf = (WavBuffer*)userdata;
    short *sound_dat = (short*)sounddata;
    
    wavbuf->pushData(len/sizeof(short), sound_dat);
}

void renderWave(RenderChannel rc){
    WavBuffer *wb = rc.wb;
    int displayct = 8192/4;
    double w = 50;
    short buf[displayct];
    int64_t cur_sample = wb->getCurrentSampleIndex();
    wb->debug(cur_sample);
    wb->getData(buf, cur_sample - displayct, displayct, rc.chan);
    glColor3d(rc.color.x, rc.color.y, rc.color.z);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < displayct; i++){
        glVertex3d(i/(double)displayct * w - 0.5*w, ((double)buf[i])/0x7fff, 0.01*rc.chan);
    }
    glEnd();
}

void renderPhase(WavBuffer *wb){
    int displayct = 8192/4;
    short buf0[displayct];
    short buf1[displayct];
    int64_t cur_sample = wb->getCurrentSampleIndex();
    wb->getData(buf0, cur_sample - displayct, displayct, 0);
    wb->getData(buf1, cur_sample - displayct, displayct, 1);
    double w = 50;
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < displayct; i++){
    	Vec2d phz = Vec2d(((double)buf0[i])/0x7fff, ((double)buf1[i])/0x7fff);
    	Vec3d x = Vec3d(phz, i/(double)displayct * w - 0.5*w);
    	glVertex3d(x.x, x.y, x.z);
    }
    glEnd();
}

void renderBuffer(WavBuffer *wb){
    int displayct = wb->getLength();
    const short *buf = wb->getRawBuffer(0);
    double w = 100;
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < displayct; i++){
        glVertex2d(i/(double)displayct * w - 0.5*w, ((double)buf[i])/0x7fff);
    }
    glEnd();
}

WavBuffer *beginAudio(){
    SDL_Init(SDL_INIT_AUDIO);
    Mix_Music *musicObj;
    int audio_rate = 44100;
    int audio_format = AUDIO_S16SYS;
    int audio_channels = 2;
    int audio_buffers = 2048;
    
    WavBuffer *wbuf = new WavBuffer(16384*2, audio_channels, audio_rate);
    
    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)){
        std::cout << "SDL failed to init audio" << std::endl;
        exit(1);
    }
    
    musicObj = Mix_LoadMUS("/Users/tbabb/test/fishes.flac");
    if (musicObj == 0){
        std::cout << "Couldn't load music file." << std::endl;
    }
    Mix_RegisterEffect(MIX_CHANNEL_POST, &effectCallback, NULL, wbuf);
    Mix_PlayMusic(musicObj, 0);
    
    //DEBUG FIXME: sound off
    Mix_Volume(MIX_CHANNEL_POST, 0);
    
    return wbuf;
}

int main(int argc, char** argv){
    GLWindow win(&argc, argv, "OpenGL Toy Program", RESOLUTION, RESOLUTION);
    win.cam.setPosition(Z_AXIS3d * 20);
    win.cam.setCenterOfInterest(ZERO_VEC3d);
    win.cam.setNear(0.1);
    win.cam.setFar(500);
    win.cam.setUp(Y_AXIS3d);
    
    WavBuffer *wbuf   = beginAudio();
    RenderChannel rc0 = {wbuf, 0, Vec3d(0.66, 0.4, 0.15)};
    RenderChannel rc1 = {wbuf, 1, Vec3d(0.75)};
    
    Manipulator manip(&win, &win.cam);
    AnimTimer timer(&win, 60);

    //build scene
    win.scene.push_back(new VisCallback<Vec4d>(&glDoSetColor4d, Vec4d(0.9,1)));
    //win.scene.push_back(new VisCallback<RenderChannel>(&renderWave, rc0));
    //win.scene.push_back(new VisCallback<RenderChannel>(&renderWave, rc1));
    win.scene.push_back(new VisCallback<WavBuffer*>(&renderPhase, wbuf));
    win.scene.push_back(&manip);
    win.scene.push_back(new VisCallback<Vec4d>(&glDoSetColor4d, Vec4d(0.09,1)));
    Sampler<double> rv;
    for (int i = 0; i < 100; i++){
    	Vec3d c = rv.box<3>(-ONE_VEC3d * 100, ONE_VEC3d * 100);
        VisBall *b = new VisBall(c, rv.rng->rand<double>(0.01,10), 48, 24);
        win.scene.push_back(b);
    }
    
    win.setClearColor(Vec4d(0.11,0.11,0.11,1));
    timer.begin();
    win.showAll();
    return 0;
}
