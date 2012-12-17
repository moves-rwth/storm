/*
 * WrongFileFormatException.h
 *
 *  Created on: 16.08.2012
 *      Author: Thomas Heinemann
 */

#ifndef MRMC_EXCEPTIONS_WRONGFILEFORMATEXCEPTION_H_
#define MRMC_EXCEPTIONS_WRONGFILEFORMATEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace mrmc {

namespace exceptions {

/*! 
 * @brief This exception is thrown when an input file
 * contains invalid or missing keys.
 */
MRMC_EXCEPTION_DEFINE_NEW(WrongFileFormatException)

} //namespace exceptions

} //namespace mrmc

#endif /* MRMC_EXCEPTIONS_WRONGFILEFORMATEXCEPTION_H_ */
