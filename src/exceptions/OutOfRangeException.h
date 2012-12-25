#ifndef STORM_EXCEPTIONS_OUTOFRANGEEXCEPTION_H_
#define STORM_EXCEPTIONS_OUTOFRANGEEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace storm {

namespace exceptions {

/*
 * @briefThis exception is thrown when a parameter is not in the range of valid values
 */
STORM_EXCEPTION_DEFINE_NEW(OutOfRangeException)

} // namespace exceptions

} // namespace storm
#endif // STORM_EXCEPTIONS_OUTOFRANGEEXCEPTION_H_
