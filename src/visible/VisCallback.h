/*
 * VisFunc.h
 * 
 * Draw something with a function pointer. May store data which will be
 * passed to the funciton.
 *
 *  Created on: Jun 11, 2011
 *      Author: tbabb
 */

#ifndef VISFUNC_H_
#define VISFUNC_H_

template <typename T> class VisCallback : virtual public Drawable {
public:
    void (*drawfnc)(T);
    T data;
    
    VisCallback(void (*drawfnc)(T)):drawfnc(drawfnc){}
    
    VisCallback(void (*drawfnc)(T), T data):drawfnc(drawfnc),data(data){}
    
    virtual ~VisCallback(){}
    
    virtual void draw(){
        drawfnc(data);
    }
    
};

#endif /* VISFUNC_H_ */
