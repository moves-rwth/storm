#ifndef STORM_UTILITY_MACROS_H_
#define STORM_UTILITY_MACROS_H_

#include <cassert>
#include "storm-config.h"

#ifndef STORM_LOGGING_FRAMEWORK
#include <iostream>
#include <sstream>

extern int storm_runtime_loglevel;

#define STORM_LOGLEVEL_ERROR 0
#define STORM_LOGLEVEL_WARN 1
#define STORM_LOGLEVEL_INFO 2
#define STORM_LOGLEVEL_DEBUG 3
#define STORM_LOGLEVEL_TRACE 4

#define STORM_LOG_DEBUG(message)                                    \
do {                                                                \
    if(storm_runtime_loglevel >= STORM_LOGLEVEL_DEBUG) {            \
        std::stringstream __ss;                                     \
        __ss << message;                                            \
        std::cout << "LOG DBG: " << __ss.str() << std::endl;        \
    }                                                               \
} while (false)

#define STORM_LOG_TRACE(message)                                \
do {                                                            \
    if(storm_runtime_loglevel >= STORM_LOGLEVEL_TRACE) {        \
        std::stringstream __ss;                                 \
        __ss << message;                                        \
        std::cout << "LOG TRC: " << message << std::endl;       \
    }                                                           \
} while(false)  


// Define STORM_LOG_ASSERT which is only checked when NDEBUG is not set.
#ifndef NDEBUG
#define STORM_LOG_ASSERT(cond, message)                     \
do {                                                        \
if (!(cond)) {                                              \
std::cout << "ASSERT FAILED: " << message << std::endl;     \
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
        std::cout << "LOG ERR: " << message << std::endl;       \
        throw exception() << message;                           \
    }                                                           \
} while (false)                                


// Define STORM_LOG_WARN, STORM_LOG_ERROR and STORM_LOG_INFO to log the given message with the corresponding log levels.
#define STORM_LOG_WARN(message)                                \
do {                                                           \
    if(storm_runtime_loglevel >= STORM_LOGLEVEL_WARN) {        \
        std::stringstream __ss;                                \
        __ss << message;                                       \
        std::cout << "LOG WRN: " << message << std::endl;      \
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
        std::cout << "LOG INF: " << message << std::endl;       \
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
        std::cout << "LOG ERR: " << message << std::endl;       \
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

#define STORM_GLOBAL_LOGLEVEL_DEBUG()               \
do {                                                \
storm_runtime_loglevel = STORM_LOGLEVEL_DEBUG;      \
} while(false)
    
#define STORM_GLOBAL_LOGLEVEL_TRACE()               \
do {                                                \
storm_runtime_loglevel = STORM_LOGLEVEL_TRACE;      \
} while(false)

#else 
// Include the parts necessary for Log4cplus.
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;
/*!
 * Define the macros STORM_LOG_DEBUG and STORM_LOG_TRACE.
 */
#define STORM_LOG_DEBUG(message)                \
do {                                            \
    LOG4CPLUS_DEBUG(logger, message);           \
} while (false)                                 \

#define STORM_LOG_TRACE(message)                \
do {                                            \
    LOG4CPLUS_TRACE(logger, message);           \
} while (false)                                 \

// Define STORM_LOG_ASSERT which is only checked when NDEBUG is not set.
#ifndef NDEBUG
#define STORM_LOG_ASSERT(cond, message)         \
do {                                            \
if (!(cond)) {                                  \
LOG4CPLUS_ERROR(logger, message);               \
assert(cond);                                   \
}                                               \
} while (false)                                 \

#else
#define STORM_LOG_ASSERT(cond, message)         
#endif
// Define STORM_LOG_THROW to always throw the exception with the given message if the condition fails to hold.
#define STORM_LOG_THROW(cond, exception, message)     \
do {                                            \
    if (!(cond)) {                              \
        LOG4CPLUS_ERROR(logger, message);       \
        throw exception() << message;           \
    }                                           \
} while (false)                                 \


// Define STORM_LOG_WARN, STORM_LOG_ERROR and STORM_LOG_INFO to log the given message with the corresponding log levels.
#define STORM_LOG_WARN(message)                 \
do {                                            \
    LOG4CPLUS_WARN(logger, message);            \
} while (false)                                 \

#define STORM_LOG_WARN_COND(cond, message)      \
do {                                            \
    if (!(cond)) {                              \
        LOG4CPLUS_WARN(logger, message);        \
    }                                           \
} while (false)                                 \

#define STORM_LOG_INFO(message)                 \
do {                                            \
    LOG4CPLUS_INFO(logger, message);            \
} while (false)                                 \

#define STORM_LOG_INFO_COND(cond, message)      \
do {                                            \
    if (!(cond)) {                              \
        LOG4CPLUS_INFO(logger, message);        \
    }                                           \
} while (false)                                 \

#define STORM_LOG_ERROR(message)                \
do {                                            \
    LOG4CPLUS_ERROR(logger, message);           \
} while (false)                                 \

#define STORM_LOG_ERROR_COND(cond, message)     \
do {                                            \
    if (!(cond)) {                              \
        LOG4CPLUS_ERROR(logger, message);       \
    }                                           \
} while (false)                                 \


#define STORM_GLOBAL_LOGLEVEL_INFO() \
do { \
logger.getAppender("mainConsoleAppender")->setThreshold(log4cplus::INFO_LOG_LEVEL); \
LOG4CPLUS_INFO(logger, "Enabled verbose mode, log output gets printed to console."); \
} while (false)

#define STORM_GLOBAL_LOGLEVEL_DEBUG() \
do { \
logger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL); \
logger.getAppender("mainConsoleAppender")->setThreshold(log4cplus::DEBUG_LOG_LEVEL); \
LOG4CPLUS_INFO(logger, "Enabled very verbose mode, log output gets printed to console."); \
} while(false)
    
#define STORM_GLOBAL_LOGLEVEL_TRACE() \
do { \
logger.setLogLevel(log4cplus::TRACE_LOG_LEVEL); \
logger.getAppender("mainConsoleAppender")->setThreshold(log4cplus::TRACE_LOG_LEVEL); \
LOG4CPLUS_INFO(logger, "Enabled trace mode, log output gets printed to console."); \
} while(false)



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