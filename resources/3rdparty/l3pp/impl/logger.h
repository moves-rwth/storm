/**
 * @file logger.h
 *
 * Defines the base Logger class
 */

#pragma once

#include <vector>
#include <algorithm>
#include <map>
#include <sstream>

namespace l3pp {

namespace detail {
	/**
	 * Internal function to get all configured loggers. Should not be used
	 * directly, see Logger::getLogger()
	 */
	static inline std::map<std::string, LogPtr>& GetLoggers() {
		static std::map<std::string, LogPtr> loggers;
		return loggers;
	}
}

inline LogStream::~LogStream() {
	if (level != LogLevel::OFF) {
		logger.log(level, stream.str(), context);
	}
}

inline void Logger::logEntry(EntryContext const& context, std::string const& msg) {
	for(auto& sink: sinks) {
		sink->log(context, msg);
	}
	if (additive && parent) {
		parent->logEntry(context, msg);
	}
}

inline void Logger::removeSink(SinkPtr sink) {
	std::vector<SinkPtr>::iterator pos = std::find(sinks.begin(), sinks.end(), sink);
	if (pos != sinks.end()) {
		sinks.erase(pos);
	}
}

inline void Logger::log(LogLevel level, std::string const& msg, EntryContext context) {
	if (level < getLevel()) {
		return;
	}

	context.level = level;
	context.logger = this;
	logEntry(context, msg);
}

inline LogStream Logger::log(LogLevel level, EntryContext context) {
	if (level < getLevel()) {
		// Effectively disables the stream
		return LogStream(*this, LogLevel::OFF, context);
	} else {
		return LogStream(*this, level, context);
	}
}

inline void Logger::initialize() {
	// Setup root logger
	getRootLogger();
	// Set wall time
	Formatter::initialize();
}

inline void Logger::deinitialize() {
	detail::GetLoggers().clear();
	getRootLogger()->sinks.clear();
}

inline LogPtr Logger::getRootLogger() {
	static LogPtr rootLogger = LogPtr(new Logger());
	return rootLogger;
}

inline LogPtr Logger::getLogger(std::string name) {
	if (name.size() == 0) {
		// Root logger
		return getRootLogger();
	}
	auto& loggers = detail::GetLoggers();
	auto it = loggers.find(name);
	if (it != loggers.end()) {
		return it->second;
	} else {
		auto n = name.rfind('.');
		LogPtr parent;
		if (n == std::string::npos) {
			parent = getRootLogger();
		} else{
			parent = getLogger(name.substr(0, n));
		}
		LogPtr newLogger = LogPtr(new Logger(name, parent));
		loggers.emplace(name, newLogger);
		return newLogger;
	}
}

template<typename T>
inline LogStream const& operator<<(LogStream const& stream, T const& val) {
	if (stream.level != LogLevel::OFF) {
		stream.stream << val;
	}
	return stream;
}

inline LogStream const& operator<<(LogStream const& stream, std::ostream& (*F)(std::ostream&)) {
	if (stream.level != LogLevel::OFF) {
		stream.stream << F;
	}
	return stream;
}

}
