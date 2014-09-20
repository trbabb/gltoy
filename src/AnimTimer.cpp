/*
 * AnimTimer.cpp
 *
 *  Created on: Sep 21, 2010
 *      Author: tbabb
 */

#include <GLUT/glut.h>
#include "AnimTimer.h"
#include "Timing.h"

/****************************************
 * Structors                            *
 ****************************************/

AnimTimer::AnimTimer(GLWindow* window, double fps):
		window(window), fps(fps), running(false) {
	_registerId();
}

AnimTimer::~AnimTimer() {
	timers_by_id.erase(id);
}

/****************************************
 * Member Functions                     *
 ****************************************/

void AnimTimer::begin(){
	if (running) return; //don't start twice
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
		glutTimerFunc((unsigned int)(remaining*1000 + 0.5), timerCallback, id);
	}
}

void AnimTimer::_registerId(){
	id = max_id++;
	timers_by_id[id] = this;
}


/****************************************
 * Static Functions/Callbacks/Varaibles *
 ****************************************/

AnimTimer::timer_map_t AnimTimer::timers_by_id;
int AnimTimer::max_id = 1;

AnimTimer* AnimTimer::getTimer(int id){
	timer_map_t::iterator i = timers_by_id.find(id);
	if (i == timers_by_id.end()){
		return NULL;
	} else {
		return i->second;
	}
}

void AnimTimer::timerCallback(int id){
	AnimTimer* timer = getTimer(id);
	if (timer && timer->running){
		timer->_doFrame(false);
	}
}
