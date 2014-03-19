/*
 * TextureCard.h
 *
 *  Created on: Jun 11, 2011
 *      Author: tbabb
 */

#ifndef TEXTURECARD_H_
#define TEXTURECARD_H_

#include "VisBounded.h"
#include "Texture.h"

class VisTextureCard : virtual public VisBounded<double,3>{
public:
    VisTextureCard();
    VisTextureCard(Texture &t);
    virtual ~VisTextureCard();
    
    virtual void draw();
    virtual Rect3d bounds();
    
    void setTexture(Texture &t);
    void setColor(Vec4d c);
    Texture getTexture();
    
    Texture t;
    bool valid;
    Vec4d color;
};

#endif /* TEXTURECARD_H_ */
