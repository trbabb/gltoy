/*
 * GLTypeInfo.h
 *
 *  Created on: Jul 17, 2011
 *      Author: tbabb
 */

#ifndef GLTYPEINFO_H_
#define GLTYPEINFO_H_

#include "glHeaders.h"

/*************************
 * Type Info Helper      *
 *************************/

template <typename T> struct GlTypeInfo {};

template <> struct GlTypeInfo<float>          { static GLenum type; };
template <> struct GlTypeInfo<double>         { static GLenum type; };
template <> struct GlTypeInfo<char>           { static GLenum type; };
template <> struct GlTypeInfo<unsigned char>  { static GLenum type; };
template <> struct GlTypeInfo<int>            { static GLenum type; };
template <> struct GlTypeInfo<unsigned int>   { static GLenum type; };
template <> struct GlTypeInfo<short>          { static GLenum type; };
template <> struct GlTypeInfo<unsigned short> { static GLenum type; };


#endif /* GLTYPEINFO_H_ */
