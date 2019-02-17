#include <iostream>

#include <geomc/function/PerlinNoise.h>

#include "GLWindow.h"
#include "GUIListener.h"
#include "Manipulator.h"
#include "visible/VisCallback.h"

PerlinNoise<double,1> perlin;

void draw(int dummy) {
    const index_t n = 500;
    const double  k = 25;
    glPushMatrix();
    glBegin(GL_LINES);
    // glLoadIdentity();
    glScaled(1/k, 1/k, 1/k);
    // glTranslated(0, 0, -10);
    for (index_t i = 0; i < n; ++i) {
        double s = k * i / (double)n;
        double v = perlin.eval(s);
        glVertex2d(s, v);
    }
    glEnd();
    glPopMatrix();
}


int main() {
    GLWindow win("Perlin", 800, 800);
    Manipulator manip(&win, &win.cam);
    VisCallback<int> cb(&draw);
    win.scene.push_back(&cb);
    win.showAll();
    return 0;
}