/*
 * InternalTypeErrorException.h
 *
 *  Created on: 09.08.2013
 *      Author: Philipp Berger
 */

#ifndef STORM_EXCEPTIONS_INTERNALTYPEERROREXCEPTION_H_
#define STORM_EXCEPTIONS_INTERNALTYPEERROREXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace storm {

namespace exceptions {

/*!
 * @brief This exception is thrown when an internal function observes an unknown state or type, e.g. a missing case statement.
 */
STORM_EXCEPTION_DEFINE_NEW(InternalTypeErrorException)

}

}

#endif /* STORM_EXCEPTIONS_INTERNALTYPEERROREXCEPTION_H_ */
