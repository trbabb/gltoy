/*
 * AnimTimer.cpp
 *
 *  Created on: Sep 21, 2010
 *      Author: tbabb
 */

#include "AnimTimer.h"
#include "Timing.h"


void anim_timer_callback(void* payload) {
	AnimTimer* timer = (AnimTimer*)payload;
	timer->_doFrame(false);
}


/****************************************
 * Structors                            *
 ****************************************/


AnimTimer::AnimTimer(GLWindow* window, double fps):
		window(window), fps(fps), running(false) {}

AnimTimer::~AnimTimer() {}

/****************************************
 * Member Functions                     *
 ****************************************/


void AnimTimer::begin(){
	if (running) return; // don't start twice
	running = true;
	_doFrame(true);
}

void AnimTimer::stop(){
	running = false;
}

/****************************************
 * Private Member Functions             *
 ****************************************/


void AnimTimer::_doFrame(bool firstFrame){
	if (firstFrame){
		t = now();
		for (unsigned int i = 0; i < anims.size(); i++){
			anims[i]->init(t);
		}
	} else {
		double cur_T = now();
		double dt = cur_T - t;
		t = cur_T;
		for (unsigned int i = 0; i < anims.size(); i++){
			anims[i]->update(t, dt);
		}
	}
	
	window->redraw();
	
	if (running){
		double target_dt = 1.0/fps;
		double remaining = std::max(0.0, target_dt - (now() - t));
		postTimerCallback(remaining, anim_timer_callback, this);
	}
}

