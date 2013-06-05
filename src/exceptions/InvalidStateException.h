#ifndef STORM_EXCEPTIONS_INVALIDSTATEEXCEPTION_H_
#define STORM_EXCEPTIONS_INVALIDSTATEEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace storm {

namespace exceptions {

/*! 
 * @brief This exception is thrown when a memory request can not be
 * fulfilled.
 */
STORM_EXCEPTION_DEFINE_NEW(InvalidStateException)

} // namespace exceptions

} // namespace storm

#endif // STORM_EXCEPTIONS_INVALIDSTATEEXCEPTION_H_
