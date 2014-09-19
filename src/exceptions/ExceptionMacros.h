
#include <cassert>

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;
#undef LOG_ASSERT
#ifndef NDEBUG
#define LOG_ASSERT(cond, message)               \
{                                               \
    if (!(cond)) {                              \
        LOG4CPLUS_ERROR(logger, message);       \
        assert(cond);                           \
    }                                           \
} while (false)
#undef LOG_DEBUG
#define LOG_DEBUG(message)                      \
{                                               \
    LOG4CPLUS_DEBUG(logger, message);           \
} while (false)
#else
#define LOG_ASSERT(cond, message) /* empty */
#define LOG_DEBUG(message) /* empty */
#endif
#undef LOG_THROW
#define LOG_THROW(cond, exception, message)     \
{                                               \
if (!(cond)) {                                  \
LOG4CPLUS_ERROR(logger, message);               \
throw exception() << message;                   \
}                                               \
} while (false)
