/*
 * AnimTimer.h
 *
 * An <AnimTimer> is an object that accepts <Animated> objects, updates
 * them at a fixed framerate, and redraws a <GLWindow> at the end of each frame.
 *
 * Add your Animated and call AnimTimer.begin() to start your animation going.
 *
 * Future project: Give <Animated> a duration() method, which by default returns (-inf, inf),
 * and modify <AnimTimer> to start itself at the beginning of the earliest animation duration,
 * wait during empty intervals, drop animations after they finish.
 *
 * Future project: Allow <Animated>s to ask for hyperfast updates, and call update() on them
 * in a tight loop, only redrawing when the fps interval has timed out.
 *
 *  Created on: Sep 21, 2010
 *      Author: tbabb
 */

//TODO: The above project

#ifndef ANIMTIMER_H_
#define ANIMTIMER_H_

#include <vector>

#if __cplusplus <= 199711L
#include <tr1/unordered_map>
#else
#include <unordered_map>
#endif

#include "GLWindow.h"
#include "Animated.h"

class AnimTimer {
public:
	GLWindow* window;
	std::vector<Animated*> anims;
	double fps;
	double t;
	bool   running;
	int    id;

	AnimTimer(GLWindow* window, double fps=60);
	virtual ~AnimTimer();

	void begin();
	void stop();

protected:
    
#if __cplusplus <= 199711L
    typedef std::tr1::unordered_map<int, AnimTimer*> timer_map_t;
#else
    typedef std::unordered_map<int, AnimTimer*> timer_map_t;
#endif
    
	static timer_map_t timers_by_id;
	static int max_id;

	static AnimTimer* getTimer(int id);
	static void timerCallback(int id);

	void _doFrame(bool firstFrame=false);
	void _registerId();
};

#endif /* ANIMTIMER_H_ */
