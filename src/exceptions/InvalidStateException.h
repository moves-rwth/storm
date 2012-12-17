#ifndef MRMC_EXCEPTIONS_INVALIDSTATEEXCEPTION_H_
#define MRMC_EXCEPTIONS_INVALIDSTATEEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace mrmc {

namespace exceptions {

/*! 
 * @brief This exception is thrown when a memory request can't be
 * fulfilled.
 */
class InvalidStateException : public BaseException<InvalidStateException> {
public:
	InvalidStateException() {
	}
	InvalidStateException(const char* cstr) : BaseException(cstr) {
	}
	InvalidStateException(const InvalidStateException& cp) : BaseException(cp) {
	}
};

} // namespace exceptions

} // namespace mrmc

#endif // MRMC_EXCEPTIONS_INVALIDSTATEEXCEPTION_H_
