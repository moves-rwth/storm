#ifndef STORM_EXCEPTIONS_ILLEGALARGUMENTEXCEPTION_H_
#define STORM_EXCEPTIONS_ILLEGALARGUMENTEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace storm {

namespace exceptions {

/*!
 * @brief This exception is thrown when a parameter is invalid or illegal in this context
 */
STORM_EXCEPTION_DEFINE_NEW(IllegalArgumentException)

} // namespace exceptions

} // namespace storm
#endif // STORM_EXCEPTIONS_ILLEGALARGUMENTEXCEPTION_H_
