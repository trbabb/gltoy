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

template <typename T> class GlTypeInfo {
public:
    static GLenum type;
};

#endif /* GLTYPEINFO_H_ */
