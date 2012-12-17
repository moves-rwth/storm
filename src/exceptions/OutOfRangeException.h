#ifndef MRMC_EXCEPTIONS_OUTOFRANGEEXCEPTION_H_
#define MRMC_EXCEPTIONS_OUTOFRANGEEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace mrmc {

namespace exceptions {

/*
 * @briefThis exception is thrown when a parameter is not in the range of valid values
 */
MRMC_EXCEPTION_DEFINE_NEW(OutOfRangeException)

} // namespace exceptions

} // namespace mrmc
#endif // MRMC_EXCEPTIONS_OUTOFRANGEEXCEPTION_H_
