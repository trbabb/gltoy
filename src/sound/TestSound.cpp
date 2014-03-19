/*
 * TestSound.cpp
 *
 *  Created on: Jun 15, 2011
 *      Author: tbabb
 */

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <iostream>

int music_main(int argc, char **argv){
    SDL_Init(SDL_INIT_AUDIO);
    Mix_Music *musicObj;
    int audio_rate = 44100;
    int audio_format = AUDIO_S16;
    int audio_channels = 2;
    int audio_buffers = 4096;
    
    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)){
        std::cout << "SDL failed to init audio" << std::endl;
        exit(1);
    }
    
    musicObj = Mix_LoadMUS("/Users/tbabb/test/fishes.flac");
    if (musicObj == 0){
        std::cout << "Couldn't load music file." << std::endl;
    }
    Mix_PlayMusic(musicObj, 0);
    
    while(true){
        SDL_Delay(500);
    }
    
    return 0;
}
