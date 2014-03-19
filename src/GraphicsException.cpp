/*
 * GraphicsException.cpp
 *
 *  Created on: Jun 11, 2011
 *      Author: tbabb
 */

#include "GraphicsException.h"

GraphicsException::GraphicsException(const std::string &msg):std::runtime_error(msg) {}

GraphicsException::~GraphicsException() throw() {}
