/*
 * GLTypeInfo.cpp
 *
 *  Created on: Jul 17, 2011
 *      Author: tbabb
 */

#include "GLTypeInfo.h"

template <> GLenum GlTypeInfo<float>::type          = GL_FLOAT;
template <> GLenum GlTypeInfo<double>::type         = GL_DOUBLE;
template <> GLenum GlTypeInfo<char>::type           = GL_BYTE;
template <> GLenum GlTypeInfo<unsigned char>::type  = GL_UNSIGNED_BYTE;
template <> GLenum GlTypeInfo<int>::type            = GL_INT;
template <> GLenum GlTypeInfo<unsigned int>::type   = GL_UNSIGNED_INT;
template <> GLenum GlTypeInfo<short>::type          = GL_SHORT;
template <> GLenum GlTypeInfo<unsigned short>::type = GL_UNSIGNED_SHORT;
