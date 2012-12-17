#ifndef MRMC_EXCEPTIONS_NOCONVERGENCEEXCEPTION_H_
#define MRMC_EXCEPTIONS_NOCONVERGENCEEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace mrmc {
namespace exceptions {

/*!
 * @brief This exception is thrown when an iterative solver failed to converge with the given maxIterations
 */
class NoConvergenceException : public BaseException<NoConvergenceException> {
public:
	NoConvergenceException() {
	}
	NoConvergenceException(const char* cstr) : BaseException(cstr) {
	}
	NoConvergenceException(const NoConvergenceException& cp) : BaseException(cp) {
	}
};

} // namespace exceptions
} // namespace mrmc

#endif // MRMC_EXCEPTIONS_NOCONVERGENCEEXCEPTION_H_
