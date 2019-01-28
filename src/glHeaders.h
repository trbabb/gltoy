/* 
 * File:   glHeaders.h
 * Author: tbabb
 *
 * Created on June 1, 2014, 11:50 AM
 */

#ifndef GLHEADERS_H
#define	GLHEADERS_H

#ifndef GLTOY_OPENGL_HEADER

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// -- now not needed thanks to glfw: --

// #ifdef __APPLE__
// #include <OpenGL/gl.h>
// #include <OpenGL/glu.h>
// #include <GLUT/glut.h>
// #else
// #ifdef _WIN32
//   #include <windows.h>
// #endif
// #include <GL/gl.h>
// #include <GL/glu.h>
// #include <GL/glut.h>
// #endif

#else
#include GLTOY_OPENGL_HEADER
#endif

#endif	/* GLHEADERS_H */

