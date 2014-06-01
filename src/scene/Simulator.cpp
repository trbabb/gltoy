/*
 * Simulator.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: tbabb
 */
#include "glHeaders.h"
#include <GLUT/glut.h>
#include "Craft.h"
#include "BoxMI.h"
#include "Shader.h"
#include "Camera.h"

int win_width  = 800;
int win_height = 600;
Vec3d craft_dims(5,4,3);

Shader shad = shaderFromFile("basic","shader/basic");
Craft c(500, new BoxMI(craft_dims, 500)); //TODO: instantiate an MI
Camera sim_cam(win_width/(double)win_height);

double t          = 0;
double target_dt  = 1.0/60;
double next_frame = 0;

void updateCamera(double time){
	//arbitrarily spin & wobble the camera around for testing
	Vec3d axis = Y_AXIS3d.add(0.25,0,0);
	sim_cam.up = (Y_AXIS3d.add(0,0,0.03)).rotate(Y_AXIS3d, time*M_PI*2);
	axis = axis.rotate(Y_AXIS3d, time*M_PI*2/9);
	sim_cam.at = ZERO_VEC3d;
	sim_cam.pos = (15*Z_AXIS3d).rotate(axis, time*M_PI*2/21);
}

GLvoid reshape(int width, int height){
	if (height == 0) height = 1;
	win_width = width;
	win_height = height;
	glViewport(0,0,width,height);
	sim_cam.aspect = win_width/(double)win_height;
}

GLvoid displayLoop(){
	double now = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	double dt = t==0?target_dt:now-t;
	t = now; //t must be saved to determine dt


	//major problem: dt is usually 0
	//glut timing is shit; has 1/1000 sec resolution. We can run 10-20 iterations in that time.
	if (dt != 0){
		c.step(t, dt);
	}

	if (t >= next_frame){
		glLoadIdentity();
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.3,0.3,0.3,1);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		sim_cam.use();

		shad.use();
		c.draw();

		sim_cam.disable();

		glutSwapBuffers();

		double remaining = target_dt - (glutGet(GLUT_ELAPSED_TIME)/1000.0 - t);
		next_frame = t + remaining;
	}

	glutPostRedisplay();
}

int main(int argc, char** argv){
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(win_width,win_height);
	glutCreateWindow("gltoy");

	sim_cam.znear = 0.1;
	sim_cam.zfar  = 1000;
	sim_cam.pos   = Vec3d(0, -5, 2.5);
	sim_cam.at    = ZERO_VEC3d;
	sim_cam.up    = Z_AXIS3d;
	sim_cam.fov   = 45;

	shad.compile();
	shad.use();

	glutReshapeFunc(reshape);
	glutDisplayFunc(displayLoop);
	glutMainLoop();
	return 0;
}
