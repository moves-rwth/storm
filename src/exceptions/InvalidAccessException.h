#ifndef STORM_EXCEPTIONS_INVALIDACCESSEXCEPTION_H_
#define STORM_EXCEPTIONS_INVALIDACCESSEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace storm {

namespace exceptions {

/*! 
 * @brief This exception is thrown when a function is used/accessed that is forbidden to use (e.g. Copy Constructors)
 */
STORM_EXCEPTION_DEFINE_NEW(InvalidAccessException)

} // namespace exceptions

} // namespace storm

#endif // STORM_EXCEPTIONS_INVALIDACCESSEXCEPTION_H_
