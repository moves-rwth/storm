/*
 * IllegalFunctionCallException.h
 *
 *  Created on: 09.08.2013
 *      Author: Philipp Berger
 */

#ifndef STORM_EXCEPTIONS_ILLEGALFUNCTIONCALLEXCEPTION_H_
#define STORM_EXCEPTIONS_ILLEGALFUNCTIONCALLEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace storm {

namespace exceptions {

/*!
 * @brief This exception is thrown when a function call is not allowed in this context
 */
STORM_EXCEPTION_DEFINE_NEW(IllegalFunctionCallException)

} // namespace exceptions

} // namespace storm
#endif // STORM_EXCEPTIONS_ILLEGALFUNCTIONCALLEXCEPTION_H_
