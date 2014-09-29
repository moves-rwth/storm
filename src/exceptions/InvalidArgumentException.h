#ifndef STORM_EXCEPTIONS_INVALIDARGUMENTEXCEPTION_H_
#define STORM_EXCEPTIONS_INVALIDARGUMENTEXCEPTION_H_

#include "src/exceptions/BaseException.h"
#include "src/exceptions/ExceptionMacros.h"

namespace storm {
    namespace exceptions {
        
        STORM_NEW_EXCEPTION(InvalidArgumentException)
        
    } // namespace exceptions
} // namespace storm

#endif /* STORM_EXCEPTIONS_INVALIDARGUMENTEXCEPTION_H_ */
