#ifndef STORM_EXCEPTIONS_EXCEPTIONMACROS_H_
#define STORM_EXCEPTIONS_EXCEPTIONMACROS_H_

#include <cassert>

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

#ifndef NDEBUG
#define LOG_ASSERT(cond, message)               \
{                                               \
    if (!(cond)) {                              \
        LOG4CPLUS_ERROR(logger, message);       \
        assert(cond);                           \
    }                                           \
} while (false)
#else
#define LOG_ASSERT(cond, message) /* empty */
#endif

#define LOG_THROW(cond, exception, message)     \
{                                               \
if (!(cond)) {                              \
LOG4CPLUS_ERROR(logger, message);       \
throw exception() << message;           \
}                                           \
} while (false)

#endif /* STORM_EXCEPTIONS_EXCEPTIONMACROS_H_ */