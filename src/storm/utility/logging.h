#ifndef STORM_UTILITY_LOGGING_H_
#define STORM_UTILITY_LOGGING_H_

// Include config to know whether CARL is available or not.
#include "storm-config.h"
#ifdef STORM_HAVE_CARL
// Load streaming operator from CARL
#include <carl/io/streamingOperators.h>
namespace l3pp {
using carl::operator<<;
}
#endif

#include <l3pp.h>

#if !defined(STORM_LOG_DISABLE_DEBUG) && !defined(STORM_LOG_DISABLE_TRACE)
#define STORM_LOG_TRACE(message) L3PP_LOG_TRACE(l3pp::Logger::getRootLogger(), message)
#else
#define STORM_LOG_TRACE(message) (void)(0)
#endif

#if !defined(STORM_LOG_DISABLE_DEBUG)
#define STORM_LOG_DEBUG(message) L3PP_LOG_DEBUG(l3pp::Logger::getRootLogger(), message)
#else
#define STORM_LOG_DEBUG(message) (void)(0)
#endif

// Define STORM_LOG_WARN, STORM_LOG_ERROR and STORM_LOG_INFO to log the given message with the corresponding log levels.
#define STORM_LOG_INFO(message) L3PP_LOG_INFO(l3pp::Logger::getRootLogger(), message)
#define STORM_LOG_WARN(message) L3PP_LOG_WARN(l3pp::Logger::getRootLogger(), message)
#define STORM_LOG_ERROR(message) L3PP_LOG_ERROR(l3pp::Logger::getRootLogger(), message)

#endif /* STORM_UTILITY_LOGGING_H_ */
