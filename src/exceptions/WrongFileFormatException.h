/*
 * WrongFileFormatException.h
 *
 *  Created on: 16.08.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_EXCEPTIONS_WRONGFILEFORMATEXCEPTION_H_
#define STORM_EXCEPTIONS_WRONGFILEFORMATEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace storm {

namespace exceptions {

/*! 
 * @brief This exception is thrown when an input file
 * contains invalid or missing keys.
 */
STORM_EXCEPTION_DEFINE_NEW(WrongFileFormatException)

} //namespace exceptions

} //namespace storm

#endif /* STORM_EXCEPTIONS_WRONGFILEFORMATEXCEPTION_H_ */
