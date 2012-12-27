/*
 * InvalidPropertyException.h
 *
 *  Created on: 27.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_EXCEPTIONS_INVALIDPROPERTYEXCEPTION_H_
#define STORM_EXCEPTIONS_INVALIDPROPERTYEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace storm {

namespace exceptions {

/*!
 * @brief This exception is thrown when a parameter is invalid in this context
 */
STORM_EXCEPTION_DEFINE_NEW(InvalidPropertyException)

} // namespace exceptions

} // namespace storm

#endif /* STORM_EXCEPTIONS_INVALIDPROPERTYEXCEPTION_H_ */
