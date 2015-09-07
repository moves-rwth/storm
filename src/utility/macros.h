#ifndef STORM_UTILITY_MACROS_H_
#define STORM_UTILITY_MACROS_H_

#include <cassert>

// Include the parts necessary for Log4cplus.
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

/*!
 * Define the macros STORM_LOG_DEBUG and STORM_LOG_TRACE.
 */
#define STORM_LOG_DEBUG(message)                \
{                                               \
    LOG4CPLUS_DEBUG(logger, message);           \
} while (false)                                 \

#define STORM_LOG_TRACE(message)                \
{                                               \
    LOG4CPLUS_TRACE(logger, message);           \
} while (false)                                 \

// Define STORM_LOG_ASSERT which is only checked when NDEBUG is not set.
#ifndef NDEBUG
#define STORM_LOG_ASSERT(cond, message)         \
{                                               \
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
{                                               \
    if (!(cond)) {                              \
        LOG4CPLUS_ERROR(logger, message);       \
        throw exception() << message;           \
    }                                           \
} while (false)                                 \


// Define STORM_LOG_WARN, STORM_LOG_ERROR and STORM_LOG_INFO to log the given message with the corresponding log levels.
#define STORM_LOG_WARN(message)                 \
{                                               \
    LOG4CPLUS_WARN(logger, message);            \
} while (false)                                 \

#define STORM_LOG_WARN_COND(cond, message)      \
{                                               \
    if (!(cond)) {                              \
        LOG4CPLUS_WARN(logger, message);        \
    }                                           \
} while (false)                                 \

#define STORM_LOG_INFO(message)                 \
{                                               \
    LOG4CPLUS_INFO(logger, message);            \
} while (false)                                 \

#define STORM_LOG_INFO_COND(cond, message)      \
{                                               \
    if (!(cond)) {                              \
        LOG4CPLUS_INFO(logger, message);        \
    }                                           \
} while (false)                                 \

#define STORM_LOG_ERROR(message)                \
{                                               \
    LOG4CPLUS_ERROR(logger, message);           \
} while (false)                                 \

#define STORM_LOG_ERROR_COND(cond, message)     \
{                                               \
    if (!(cond)) {                              \
        LOG4CPLUS_ERROR(logger, message);       \
    }                                           \
} while (false)                                 \

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