/*
 * GraphicsException.h
 *
 *  Created on: Jun 11, 2011
 *      Author: tbabb
 */

#ifndef GRAPHICSEXCEPTION_H_
#define GRAPHICSEXCEPTION_H_

#include <stdexcept>

class GraphicsException : public std::runtime_error {
public:
    GraphicsException(const std::string &msg);
    virtual ~GraphicsException() throw();
};

#endif /* GRAPHICSEXCEPTION_H_ */
