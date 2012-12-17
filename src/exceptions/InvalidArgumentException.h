#ifndef MRMC_EXCEPTIONS_INVALIDARGUMENTEXCEPTION_H_
#define MRMC_EXCEPTIONS_INVALIDARGUMENTEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace mrmc {

namespace exceptions {

/*!
 * @brief This exception is thrown when a parameter is invalid in this context
 */
MRMC_EXCEPTION_DEFINE_NEW(InvalidArgumentException)

} // namespace exceptions

} // namespace mrmc
#endif // MRMC_EXCEPTIONS_INVALIDARGUMENTEXCEPTION_H_
