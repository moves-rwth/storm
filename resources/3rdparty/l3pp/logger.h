/**
 * @file logger.h
 *
 * Defines the base Logger class
 */

#pragma once

#include <vector>
#include <sstream>

namespace l3pp {

/**
 * LogStream is a logger object that can be streamed into, writing an entry
 * to the logger associated upon destruction. Instances of this classer are
 * returned by Logger log() functions, so they can be used as such:
 * logger->debug() << "Message";
 */
class LogStream {
	friend class Logger;

	Logger& logger;
	LogLevel level;
	EntryContext context;
	mutable std::ostringstream stream;

	LogStream(Logger& logger, LogLevel level, EntryContext context) :
		logger(logger), level(level), context(context)
	{
	}

	LogStream(const LogStream&) = delete;
	LogStream& operator=(const LogStream&) = delete;
public:
	LogStream(LogStream&& other) :
		logger(other.logger), level(other.level), context(std::move(other.context))/*,
		stream(std::move(other.stream))*/
	{
		stream.str(other.stream.str());
	}
	~LogStream();

	template<typename T>
	friend LogStream const& operator<<(LogStream const& stream, T const& val);
	friend LogStream const& operator<<(LogStream const& stream, std::ostream& (*F)(std::ostream&));
};

/**
 * Main logger class. Keeps track of all Logger instances, and can be used to
 * log various messages. Before the logging library is used, make sure to
 * call Logger::initialize(). Loggers are hierarchically nested, by means of
 * names separated by a period. All loggers are a (indirect) child of the root
 * logger, see Logger::getRootLogger() and Logger::getLogger().
 * A logger is associated with a LogLevel. Any entry with a level below this
 * level will be filtered out. A LogLevel of INHERIT means the parent log
 * level will be compared against instead.
 * A logger can be associated with 1 or more Sinks. A log entry is printed to
 * each associated sink. If the Logger is set additive (see getAdditive(),
 * setAdditive()) parent sinks are logged to as well (by default true).
 * Logging can be performed either as a single string message, or by using a
 * stream. The latter requires the end() method to be called before the entry
 * is logged. For convenience, various logging macros are defined at the end
 * of this header.
 */
class Logger {
	friend class Formatter;

	typedef std::shared_ptr<Logger> LogPtr;

	LogPtr parent;
	std::string name;
	LogLevel level;
	std::vector<SinkPtr> sinks;
	bool additive;

	// Logger constructors are private
	Logger() : parent(nullptr), name(""), level(LogLevel::DEFAULT),
		additive(true)
	{

	}

	Logger(std::string const& name, LogPtr parent) : parent(parent), name(name),
		level(LogLevel::INHERIT), additive(true)
	{
	}

	void logEntry(EntryContext const& context, std::string const& msg);

public:
	void addSink(SinkPtr sink) {
		sinks.push_back(sink);
	}

	void removeSink(SinkPtr sink);

	void setLevel(LogLevel level) {
		if (level == LogLevel::INHERIT && !parent) {
			return;
		}
		this->level = level;
	}

	LogLevel getLevel() const {
		if (level == LogLevel::INHERIT) {
			return parent->getLevel();
		}
		return level;
	}

	std::string const& getName() const {
		return name;
	}

	bool getAdditive() const {
		return additive;
	}

	void setAdditive(bool additive) {
		this->additive = additive;
	}

	void log(LogLevel level, std::string const& msg, EntryContext context = EntryContext());

	void trace(std::string const& msg, EntryContext context = EntryContext()) {
		log(LogLevel::TRACE, msg, context);
	}
	void debug(std::string const& msg, EntryContext context = EntryContext()) {
		log(LogLevel::DEBUG, msg, context);
	}
	void info(std::string const& msg, EntryContext context = EntryContext()) {
		log(LogLevel::INFO, msg, context);
	}
	void warn(std::string const& msg, EntryContext context = EntryContext()) {
		log(LogLevel::WARN, msg, context);
	}
	void error(std::string const& msg, EntryContext context = EntryContext()) {
		log(LogLevel::ERR, msg, context);
	}
	void fatal(std::string const& msg, EntryContext context = EntryContext()) {
		log(LogLevel::FATAL, msg, context);
	}

	LogStream log(LogLevel level, EntryContext context = EntryContext());

	LogStream trace(EntryContext context = EntryContext()) {
		return log(LogLevel::TRACE, context);
	}
	LogStream debug(EntryContext context = EntryContext()) {
		return log(LogLevel::DEBUG, context);
	}
	LogStream info(EntryContext context = EntryContext()) {
		return log(LogLevel::INFO, context);
	}
	LogStream warn(EntryContext context = EntryContext()) {
		return log(LogLevel::WARN, context);
	}
	LogStream error(EntryContext context = EntryContext()) {
		return log(LogLevel::ERR, context);
	}
	LogStream fatal(EntryContext context = EntryContext()) {
		return log(LogLevel::FATAL, context);
	}

	static void initialize();
	static void deinitialize();

	static LogPtr getRootLogger();

	static LogPtr getLogger(LogPtr logger) {
		return logger;
	}

	static LogPtr getLogger(std::string name);
};
typedef std::shared_ptr<Logger> LogPtr;

}
