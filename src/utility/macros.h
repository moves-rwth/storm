#ifndef STORM_UTILITY_MACROS_H_
#define STORM_UTILITY_MACROS_H_

#include <cassert>
#include <string.h>

#include "storm-config.h"

#include <iostream>
#include <sstream>

extern int storm_runtime_loglevel;

#define STORM_LOGLEVEL_ERROR 0
#define STORM_LOGLEVEL_WARN 1
#define STORM_LOGLEVEL_INFO 2
#define STORM_LOGLEVEL_DEBUG 3
#define STORM_LOGLEVEL_TRACE 4

#ifdef STORM_LOG_DISABLE_DEBUG
#define STORM_LOG_DISABLE_TRACE
#endif


#define __SHORT_FORM_OF_FILE__ \
(strrchr(__FILE__,'/') \
? strrchr(__FILE__,'/')+1 \
: __FILE__ \
)

#ifndef STORM_LOG_DISABLE_DEBUG
#define STORM_LOG_DEBUG(message)                                    \
do {                                                                \
    if(storm_runtime_loglevel >= STORM_LOGLEVEL_DEBUG) {            \
        std::stringstream __ss;                                     \
        __ss << message;                                            \
        std::cout << "DEBUG (" << __SHORT_FORM_OF_FILE__ << ":" << __LINE__ << "): " <<  __ss.str() << std::endl;        \
    }                                                               \
} while (false)
#else
#define STORM_LOG_DEBUG(message)                                    \
do {                                                                \
} while (false)                                                        
#endif

#ifndef STORM_LOG_DISABLE_TRACE
#define STORM_LOG_TRACE(message)                                \
do {                                                            \
    if(storm_runtime_loglevel >= STORM_LOGLEVEL_TRACE) {        \
        std::stringstream __ss;                                 \
        __ss << message;                                        \
        std::cout << "TRACE (" << __SHORT_FORM_OF_FILE__ << ":" << __LINE__ << "): " <<  __ss.str() << std::endl;       \
    }                                                           \
} while(false)  
#else
#define STORM_LOG_TRACE(message)                                \
do {                                                            \
} while (false)    
#endif


// Define STORM_LOG_ASSERT which is only checked when NDEBUG is not set.
#ifndef NDEBUG
#define STORM_LOG_ASSERT(cond, message)                     \
do {                                                        \
if (!(cond)) {                                              \
std::cout << "ASSERT FAILED (" << __SHORT_FORM_OF_FILE__ << ":" << __LINE__ << "): " <<  message << std::endl;     \
assert(cond);                                               \
}                                                           \
} while (false)                                 

#else
#define STORM_LOG_ASSERT(cond, message)         
#endif
// Define STORM_LOG_THROW to always throw the exception with the given message if the condition fails to hold.
#define STORM_LOG_THROW(cond, exception, message)               \
do {                                                            \
    if (!(cond)) {                                              \
        std::cout << "ERROR (" << __SHORT_FORM_OF_FILE__ << ":" << __LINE__ << "): " <<  message << std::endl;       \
        throw exception() << message;                           \
    }                                                           \
} while (false)                                


// Define STORM_LOG_WARN, STORM_LOG_ERROR and STORM_LOG_INFO to log the given message with the corresponding log levels.
#define STORM_LOG_WARN(message)                                \
do {                                                           \
    if(storm_runtime_loglevel >= STORM_LOGLEVEL_WARN) {        \
        std::stringstream __ss;                                \
        __ss << message;                                       \
        std::cout << "WARN  (" << __SHORT_FORM_OF_FILE__ << ":" << __LINE__ << "): " <<  __ss.str() << std::endl;      \
    }                                                          \
} while (false)                               

#define STORM_LOG_WARN_COND(cond, message)                      \
do {                                                            \
    if (!(cond)) {                                              \
        STORM_LOG_WARN(message);                                 \
    }                                                           \
} while (false)                                 

#define STORM_LOG_INFO(message)                                 \
do {                                                            \
    if(storm_runtime_loglevel >= STORM_LOGLEVEL_INFO) {         \
        std::stringstream __ss;                                 \
        __ss << message;                                        \
        std::cout << "INFO  (" << __SHORT_FORM_OF_FILE__ << ":" << __LINE__ << "): " << __ss.str() << std::endl;       \
    }                                                           \
} while (false)                                 

#define STORM_LOG_INFO_COND(cond, message)                      \
do {                                                            \
    if (!(cond)) {                                              \
        STORM_LOG_INFO(message);                                \
    }                                                           \
} while (false)                                 

#define STORM_LOG_ERROR(message)                                \
do {                                                            \
    if(storm_runtime_loglevel >= STORM_LOGLEVEL_ERROR) {        \
        std::stringstream __ss;                                 \
        __ss << message;                                        \
        std::cout << "ERROR (" << __SHORT_FORM_OF_FILE__  << ":" << __LINE__ << "): " << __ss.str() << std::endl;       \
    }                                                           \
} while (false)                                                 \

#define STORM_LOG_ERROR_COND(cond, message)     \
do {                                            \
    if (!(cond)) {                              \
        STORM_LOG_ERROR(message);               \
    }                                           \
} while (false)                                 \

#define STORM_GLOBAL_LOGLEVEL_INFO()                \
do {                                                \
storm_runtime_loglevel = STORM_LOGLEVEL_INFO;       \
} while (false)

#ifndef STORM_LOG_DISABLE_DEBUG
#define STORM_GLOBAL_LOGLEVEL_DEBUG()               \
do {                                                \
storm_runtime_loglevel = STORM_LOGLEVEL_DEBUG;      \
} while(false)
#else 
#define STORM_GLOBAL_LOGLEVEL_DEBUG()               \
std::cout << "***** warning ***** loglevel debug is not compiled\n"
#endif 

#ifndef STORM_LOG_DISABLE_TRACE
#define STORM_GLOBAL_LOGLEVEL_TRACE()               \
do {                                                \
storm_runtime_loglevel = STORM_LOGLEVEL_TRACE;      \
} while(false)
#else
#define STORM_GLOBAL_LOGLEVEL_TRACE()               \
std::cout << "***** warning ***** loglevel trace is not compiled\n"
#endif


/*!
 * Define the macros that print information and optionally also log it.
 */
#define STORM_PRINT(message)                    \
{                                               \
    std::cout << message;                       \
}

#define STORM_PRINT_AND_LOG(message)            \
{                                               \
    STORM_LOG_INFO(message);                    \
    STORM_PRINT(message);                       \
}

#endif /* STORM_UTILITY_MACROS_H_ */
