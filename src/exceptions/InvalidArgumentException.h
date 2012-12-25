#ifndef STORM_EXCEPTIONS_INVALIDARGUMENTEXCEPTION_H_
#define STORM_EXCEPTIONS_INVALIDARGUMENTEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace storm {

namespace exceptions {

/*!
 * @brief This exception is thrown when a parameter is invalid in this context
 */
STORM_EXCEPTION_DEFINE_NEW(InvalidArgumentException)

} // namespace exceptions

} // namespace storm
#endif // STORM_EXCEPTIONS_INVALIDARGUMENTEXCEPTION_H_
