#ifndef MRMC_EXCEPTIONS_OUTOFRANGEEXCEPTION_H_
#define MRMC_EXCEPTIONS_OUTOFRANGEEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace mrmc {

namespace exceptions {

/*
 * @briefThis exception is thrown when a parameter is not in the range of valid values
 */
class OutOfRangeException : public BaseException<OutOfRangeException> {
public:
	OutOfRangeException() {
	}
	OutOfRangeException(const char* cstr) : BaseException(cstr) {
	}
	OutOfRangeException(const OutOfRangeException& cp) : BaseException(cp) {
	}
};

} // namespace exceptions

} // namespace mrmc
#endif // MRMC_EXCEPTIONS_OUTOFRANGEEXCEPTION_H_
