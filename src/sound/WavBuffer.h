/*
 * WavBuffer.h
 * 
 * WavBuffer is a circular buffer for audio data. It is designed to
 * intercept output from SDL_mixer, via an effects callback. This class can
 * use timer information and some assumptions about buffer usage to
 * estimate which sample is currently playing with reasonable accuracy. 
 * This can be useful for synchronizing graphics to audio.
 * 
 * Note that "now" in your drawing code is not neceassarily the
 * time at which the current frame will be displayed. It may be
 * useful to note the difference in time between when you call
 * wavBufffer.getCurrentSampleIndex() and when opengl's swapBuffers() 
 * returns. In short, you'll probably want to render for what /will/
 * be playing when the current frame hits the screen, not what's
 * playing /right now/.
 *
 *  Created on: Jun 24, 2011
 *      Author: tbabb
 */

#ifndef WAVBUFFER_H_
#define WAVBUFFER_H_

#include <stdint.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include <geomc/geomc_defs.h>

class WavBuffer {
public:
    
    WavBuffer(int len, int channels=2, int hz=44100);
    
    ~WavBuffer();
    
    void pushData(int smps, short *data);
    void debug();
    void debug(int64_t sample);
    int64_t getLeadingSampleIndex();
    int64_t getCurrentSampleIndex();
    int getBufIndex(int globalIdx);
    int getLength();
    const short *getRawBuffer(int chan);
    void getData(short *into, int64_t globalStartIndex, int samples, int chan);
    
private:
    
    bool    inited;
    int     headptr;  //ptr into buf of first "empty" space
    int     len;      //size of buf
    int     channels; //num channels
    short **bufs;     //buffers for sound samples (buf[channel][sample])
    int     hz;       //sample rate (e.g. 44100)
    int64_t  head_sample_num;  //global sample # of most recent sample in buffer
    int64_t  time_sample_num;  //global sample # of "known time" sample
    uint32_t time_sample_time; //time of "known" sample
    int64_t  last_block_begin; //at which sample did the most recent block begin?
    
};

#endif /* WAVBUFFER_H_ */
