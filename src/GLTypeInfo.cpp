/*
 * GLTypeInfo.cpp
 *
 *  Created on: Jul 17, 2011
 *      Author: tbabb
 */

#include "GLTypeInfo.h"

GLenum GlTypeInfo<float>::type          = GL_FLOAT;
GLenum GlTypeInfo<double>::type         = GL_DOUBLE;
GLenum GlTypeInfo<char>::type           = GL_BYTE;
GLenum GlTypeInfo<unsigned char>::type  = GL_UNSIGNED_BYTE;
GLenum GlTypeInfo<int>::type            = GL_INT;
GLenum GlTypeInfo<unsigned int>::type   = GL_UNSIGNED_INT;
GLenum GlTypeInfo<short>::type          = GL_SHORT;
GLenum GlTypeInfo<unsigned short>::type = GL_UNSIGNED_SHORT;
