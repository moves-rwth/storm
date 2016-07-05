/**
 * @file l3pp.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Harold Bruintjes <h.bruintjes@cs.rwth-aachen.de>
 *
 * The lightweight logging library for C++.
 *
 * This logging facility is fairly generic and is used as a simple and
 * header-only alternative to more advanced solutions like log4cplus or
 * boost::log.
 *
 * The basic components are Sinks, Formatters and Loggers.
 *
 * A Sink represents a logging output like a terminal or a log file.
 * This implementation provides a FileSink and a StreamSink, but the basic
 * Sink class can be extended as necessary.
 *
 * A Formatter is associated with a Sink and produces the actual string that is
 * sent to the Sink.
 * Usually, it adds auxiliary information like the current time, LogLevel and
 * source location to the string logged by the user.
 * The Formatter implements a reasonable default behavior for simple logging,
 * but it can be subclassed and modified as necessary.
 *
 * The Logger class finally plugs all these components together.
 * Individual loggers can log to one or more sinks (inheritable) and are
 * associated with an (inheritable) log level.
 *
 * Initial configuration may look like this:
 * @code{.cpp}
 * l3pp::Logger::initialize();
 * l3pp::SinkPtr sink = l3pp::StreamSink::create(std::clog);
 * l3pp::Logger::getRootLogger()->addSink(sink);
 * l3pp::Logger::getRootLogger()->setLevel(l3pp::LogLevel::INFO);
 * @endcode
 * 
 * Macro facilitate the usage:
 * <ul>
 * <li>`L3PP_LOG_<LVL>(logger, msg)` produces a normal log message where
 * logger should be string identifying the logger (or a LogPtr) and msg is the
 * message to be logged.</li>
 * </ul>
 * Any message (`msg`) can be an arbitrary expression that one would
 * stream to an `std::ostream` like `stream << (msg);`. The default formatter
 * adds newlines.
 * Manipulators are generally supported. However, for performance avoid std::endl
 * and use '\n' directly.
 */

#pragma once

#include <chrono>
#include <memory>

namespace l3pp {

/**
 * Indicated which log messages should be forwarded to some sink.
 * 
 * All messages which have a level that is equal or greater than the specified
 * value will be forwarded.
 */
enum class LogLevel {
	/// Log messages used for tracing the program flow in detail.
	TRACE,
	/// Log messages used for debugging.
	DEBUG,
	/// Log messages used for information.
	INFO,
	/// Log messages used to warn about an undesired state.
	WARN,
	/// Log messages used for errors that can be handled.
	ERR,
	/// Log messages used for errors that lead to program termination.
	FATAL,
	/// Log no messages.
	OFF,
	/// Parent level
	INHERIT,
	/// Default log level.
	DEFAULT = WARN,
	/// All log messages.
	ALL = TRACE
};

/**
 * Streaming operator for LogLevel.
 * @param os Output stream.
 * @param level LogLevel.
 * @return os.
 */
inline std::ostream& operator<<(std::ostream& os, LogLevel level);

class Logger;

/**
 * Contextual information for a new log entry, contains such this as location,
 * log info (level, logger) and the time of the event.
 * A context will be created automatically by using the macros
 */
struct EntryContext {
	// Program location
	const char* filename;
	size_t line;
	const char* funcname;

	// Time of entry
	std::chrono::system_clock::time_point timestamp;

	// Log event info
	Logger const* logger;
	LogLevel level;

	EntryContext(const char* filename, size_t line, const char* funcname) :
		filename(filename), line(line), funcname(funcname),
		timestamp(std::chrono::system_clock::now()), logger(nullptr),
		level(LogLevel::OFF)
	{
	}

	EntryContext() :
		filename(""), line(0), funcname(""),
		timestamp(std::chrono::system_clock::now()), logger(nullptr),
		level(LogLevel::OFF)
	{
	}
};

}

#include "formatter.h"
#include "sink.h"
#include "logger.h"

#include "impl/logging.h"
#include "impl/logger.h"
#include "impl/formatter.h"
#include "impl/sink.h"

#ifdef _MSC_VER
#define __func__ __FUNCTION__
#endif

/// Create a record info.
#define __L3PP_LOG_RECORD l3pp::EntryContext(__FILE__, __LINE__, __func__)
/// Basic logging macro.
#define __L3PP_LOG(level, channel, expr) do { \
    auto L3PP_channel = ::l3pp::Logger::getLogger(channel); \
    if (L3PP_channel->getLevel() <= level) { \
        L3PP_channel->log(level, __L3PP_LOG_RECORD) << expr; \
    } \
} while(false)

/// Log with level TRACE.
#define L3PP_LOG_TRACE(channel, expr) __L3PP_LOG(::l3pp::LogLevel::TRACE, channel, expr)
/// Log with level DEBUG.
#define L3PP_LOG_DEBUG(channel, expr) __L3PP_LOG(::l3pp::LogLevel::DEBUG, channel, expr)
/// Log with level INFO.
#define L3PP_LOG_INFO(channel, expr) __L3PP_LOG(::l3pp::LogLevel::INFO, channel, expr)
/// Log with level WARN.
#define L3PP_LOG_WARN(channel, expr) __L3PP_LOG(::l3pp::LogLevel::WARN, channel, expr)
/// Log with level ERROR.
#define L3PP_LOG_ERROR(channel, expr) __L3PP_LOG(::l3pp::LogLevel::ERR, channel, expr)
/// Log with level FATAL.
#define L3PP_LOG_FATAL(channel, expr) __L3PP_LOG(::l3pp::LogLevel::FATAL, channel, expr)
