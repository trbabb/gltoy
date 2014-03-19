/*
 * WavBuffer.cpp
 *
 *  Created on: Jun 24, 2011
 *      Author: tbabb
 */

#include <iostream>
#include "WavBuffer.h"

/***********************
 * Structors           *
 ***********************/

WavBuffer::WavBuffer(int len, int channels, int hz):
        inited(false),
        headptr(0),
        len(len),
        channels(channels),
        bufs(new short*[channels]),
        hz(hz),
        head_sample_num(0),
        time_sample_num(0),
        last_block_begin(0){
    for (int c = 0; c < channels; c++){
        bufs[c] = new short[len];
        //set to silence
        for (int i = 0; i < len; i++){
            bufs[c][i] = 0;
        }
    }
}
    
WavBuffer::~WavBuffer(){
    for (int c = 0; c < channels; c++){
        delete [] bufs[c];
    }
    delete [] bufs;
}

/***********************
 * Methods             *
 ***********************/

//channel data is currently assumed to be interleaved.
void WavBuffer::pushData(int smps, short *data){
    //we assume this callback will be triggered when
    //the previously buffered block starts playing:
    int channel_smps = smps/channels;
    time_sample_time = SDL_GetTicks();
    time_sample_num  = last_block_begin;
    
    //fill buffer circularly
    for (int i = 0; i < std::min(smps, len*channels); i++){
        int dst_idx = (headptr + (i/channels)) % len;
        int c = i % channels;
        bufs[c][dst_idx] = data[i];
    }
    headptr = (headptr + channel_smps) % len;
    last_block_begin = head_sample_num;
    head_sample_num += channel_smps;
    
    if (!inited){
        //if this is the first buffering op, we assume that playback will
        //begin as soon as the callback returns. (i.e. now)
        time_sample_time = SDL_GetTicks();
        time_sample_num = 0;
        inited = true;
    }
}

//get the global sample index of the most recently buffered sample
//i.e., the sample farthest in the future
int64_t WavBuffer::getLeadingSampleIndex(){
    return head_sample_num;
}

//get the global sample index of the currently playing sample
int64_t WavBuffer::getCurrentSampleIndex(){
    int64_t cur_time = SDL_GetTicks();
    int64_t smps_since_estimate = (cur_time - time_sample_time)*hz / 1000;
    return time_sample_num + smps_since_estimate;
}

//get the index into the internal buffer of the given global sample
int WavBuffer::getBufIndex(int globalIdx){
    int begin_num = head_sample_num - len;
    return (globalIdx - begin_num + headptr) % len;
}

//copy data from the internal buffer into the supplied destination buffer.
//copied data will be contiguous. Behavior when copying more samples than
//exist in the buffer is undefined.
void WavBuffer::getData(short *into, int64_t globalStartIndex, int samples, int chan){
    int beginIdx = getBufIndex(globalStartIndex);
    for (int i = 0; i < samples; i++){
        int src_idx = (beginIdx + i) % len;
        into[i] = bufs[chan][src_idx];
    }
}

//get the number of samples this buffer can store
int WavBuffer::getLength(){
    return len;
}

//get the raw circular buffer for the given channel.
//the "beginning" of the data may be anywhere inside the returned array.
const short *WavBuffer::getRawBuffer(int chan){
    return bufs[chan];
}

void WavBuffer::debug(){
    int begin = head_sample_num - len;
    int cur   = getCurrentSampleIndex();
    std::cout << "begin at: " << begin << " end at: " << head_sample_num << std::endl;
    std::cout << "current: " << cur << "; diff: " << head_sample_num - cur << std::endl;
    if (cur < begin){
        std::cout << "active data pushed off edge!" << std::endl;
    } else if (cur > head_sample_num){
        std::cout << "active data unbuffered!" << std::endl;
    }
    std::cout << std::endl;
}

void WavBuffer::debug(int64_t sample){
    if (sample > head_sample_num){
        std::cout << "unbuffered sample" << std::endl;
    } else if (sample < head_sample_num - len){
        std::cout << "stale sample" << std::endl;
    }
}
