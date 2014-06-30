#ifndef STORM_EXCEPTIONS_INVALIDOPERATIONEXCEPTION_H_
#define STORM_EXCEPTIONS_INVALIDOPERATIONEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace storm {
    
    namespace exceptions {
        
        /*!
         * @brief This exception is thrown when an operation is invalid in this context
         */
        STORM_EXCEPTION_DEFINE_NEW(InvalidOperationException)
        
    } // namespace exceptions
} // namespace storm
#endif // STORM_EXCEPTIONS_INVALIDOPERATIONEXCEPTION_H_
