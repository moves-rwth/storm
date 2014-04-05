#ifndef STORM_EXCEPTIONS_INVALIDTYPEEXCEPTION_H_
#define STORM_EXCEPTIONS_INVALIDTYPEEXCEPTION_H_

#include "src/exceptions/BaseException.h"

namespace storm {
    
    namespace exceptions {
        
        /*!
         * @brief This exception is thrown when a type is invalid in this context
         */
        STORM_EXCEPTION_DEFINE_NEW(InvalidTypeException)
        
    } // namespace exceptions
    
} // namespace storm
#endif // STORM_EXCEPTIONS_INVALIDTYPEEXCEPTION_H_
