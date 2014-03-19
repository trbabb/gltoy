/*
 * TextureCard.cpp
 *
 *  Created on: Jun 11, 2011
 *      Author: tbabb
 */

#include "VisTextureCard.h"

VisTextureCard::VisTextureCard():t(0,0),valid(false),color(1){
    // do nothing else
}

VisTextureCard::VisTextureCard(Texture &t):t(t),valid(true),color(1){
    // do nothing else
}

VisTextureCard::~VisTextureCard() {
    // do nothing else
}

void VisTextureCard::setTexture(Texture &t){
    this->t = t;
}

Texture VisTextureCard::getTexture(){
    return this->t;
}

void VisTextureCard::draw(){
    glPushAttrib(GL_CURRENT_BIT);
    this->t.use();
    glBegin(GL_QUADS);
      glColor4d(color.x, color.y, color.z, color.w);
      glNormal3d(0,0,1);
      
      glTexCoord2d(0,0);
      glVertex2d(0,0);
      
      glTexCoord2d(1,0);
      glVertex2d(1,0);
      
      glTexCoord2d(1,1);
      glVertex2d(1,1);
      
      glTexCoord2d(0,1);
      glVertex2d(0,1);
    glEnd();
    this->t.disable();
    glPopAttrib();
}

void VisTextureCard::setColor(Vec4d c){
    this->color = c;
}


Rect3d VisTextureCard::bounds(){
    return Rect3d(Vec3d(0,0,-0.001), Vec3d(1,1,0.001));
}
