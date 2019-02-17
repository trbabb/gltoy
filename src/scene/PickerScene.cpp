#include <iostream>
#include "glHeaders.h"
#include <algorithm>

#include <geomc/random/RandomTools.h>

#include "GLWindow.h"
#include "Manipulator.h"
#include "AnimTimer.h"
#include "RenderPass.h"
#include "Shader.h"

#include "visible/VisAxis.h"
#include "visible/VisBall.h"
#include "visible/VisPointcloud.h"
#include "visible/VisTransformed.h"
#include "visible/VisCallback.h"

#define RESOLUTION 1024

using namespace geom;

void enableAlpha(int dummy) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void setColor(Vec4d c) {
    glColor4d(c.x, c.y, c.z, c.w);
}

class Picker : public PickListener {
public:
    VisBall b;
    
    bool pointPicked(
            GLWindow *window,
            int button,
            int modifiers,
            double evt_time,
            Vec2d window_pos,
            Vec3d world_pos,
            DepthCondition cond) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            b.pos = world_pos;
            window->redraw();
            return true;
        }
        return false;
    }
};

int main(int argc, char** argv) {
    GLWindow win("OpenGL Toy Program", RESOLUTION, RESOLUTION / 1.777778);
    win.cam.setPosition(X_AXIS3d * 10);
    win.cam.setCenterOfInterest(ZERO_VEC3d);
    win.cam.setNear(0.1);
    win.cam.setFar(500);
    win.cam.setUp(Z_AXIS3d);
    
    Manipulator manip(&win, &win.cam);
    
    Picker pk;
    pk.b.r = 0.125;
    
    //build scene
    win.scene.push_back(new VisCallback<int>(&enableAlpha));
    win.scene.push_back(new VisCallback<Vec4d>(&setColor, Vec4d(1,0,0,1)));
    
    Sampler<double> rv;
    index_t count = 100000;
    Vec3d *pts = new Vec3d[count];
    for (int i = 0; i < count; i++) {
        pts[i] = rv.solidball<3>();
    }
    
    VisPointcloud *pc = new VisPointcloud(count, pts);
    
    Rect3d bnds = pc->bounds();
    double sz = (bnds.max() - bnds.min()).mag();
    win.scene.push_back(new VisTransformed(pc, scale(Vec3d(100 / sz)) * translation(-bnds.getCenter())));
    win.scene.push_back(pc);
    
    // add the picked point
    win.scene.push_back(new VisCallback<Vec4d>(&setColor, Vec4d(0,1,1,1)));
    win.scene.push_back(&pk.b);
    
    win.pickListeners.clear();
    win.pickListeners.push_back(&pk);
    
    win.setClearColor(Vec4d(0.11,0.11,0.11,1));
    
    win.showAll();
    return 0;
}
