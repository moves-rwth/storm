#ifndef STORM_EXCEPTIONS_NOCONVERGENCEEXCEPTION_H_
#define STORM_EXCEPTIONS_NOCONVERGENCEEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace storm {
namespace exceptions {

/*!
 * @brief This exception is thrown when an iterative solver failed to converge with the given maxIterations
 */
STORM_EXCEPTION_DEFINE_NEW(NoConvergenceException)

} // namespace exceptions
} // namespace storm

#endif // STORM_EXCEPTIONS_NOCONVERGENCEEXCEPTION_H_
