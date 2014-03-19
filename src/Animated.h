/*
 * Animated.h
 *
 * An <Animated> is a thing that you want to change with time. Add it to an <AnimTimer>,
 * and its update() function will be called at relatively evenly-spaced intervals
 * with accurate timing information.
 *
 * An <Animated> need not define its state in terms of absolute time; it could just as
 * easily be a discrete simulation that advances in increments of dt.
 *
 *  Created on: Sep 21, 2010
 *      Author: tbabb
 */

#ifndef ANIMATED_H_
#define ANIMATED_H_

class Animated {
public:
	virtual ~Animated();
	virtual void update(double t, double dt) = 0;

	//called when animation begins. currently may happen more than once.
	virtual void init(double t);
};

/**
 * Class which wraps a callback and can act as an Animated.
 */
template <typename T> class AnimFunc : virtual public Animated {
public:
    T data;
    void (*animfunc)(double, double, T);
    
    AnimFunc(void (*animfunc)(double, double, T)):animfunc(animfunc){}
    
    AnimFunc(void (*animfunc)(double, double, T), T data):animfunc(animfunc),data(data){}
    
    virtual ~AnimFunc(){}
    
    virtual void update(double t, double dt){
        animfunc(t, dt, data);
    }
    
};

#endif /* ANIMATED_H_ */
