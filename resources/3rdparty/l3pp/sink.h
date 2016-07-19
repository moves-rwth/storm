/**
 * @file sink.h
 *
 * Defines classes for log sinks (i.e. outputs)
 */

#pragma once

#include <ostream>
#include <fstream>

namespace l3pp {

/**
 * Base class for a logging sink. It can only log some log entry to which some
 * formatting is applied (see Formatter). A Sink may be given a log level,
 * which filters out all entries below that level. By default is logs all
 * entries.
 */
class Sink {
	LogLevel level;
	FormatterPtr formatter;

	virtual void logEntry(std::string const& entry) const = 0;
public:
	Sink() : level(LogLevel::ALL), formatter(std::make_shared<Formatter>()) {

	}
	Sink(FormatterPtr formatter) : level(LogLevel::ALL), formatter(formatter) {

	}
	/**
	 * Default destructor.
     */
	virtual ~Sink() {}

	LogLevel getLevel() const {
		return level;
	}

	void setLevel(LogLevel level) {
		this->level = level;
	}

	FormatterPtr getFormatter() const {
		return formatter;
	}

	void setFormatter(FormatterPtr formatter) {
		this->formatter = formatter;
	}

	/**
	 * Logs the given message with context info
	 */
	void log(EntryContext const& context, std::string const& message) const;
};
typedef std::shared_ptr<Sink> SinkPtr;

/**
 * Logging sink that wraps an arbitrary `std::ostream`.
 * It is meant to be used for streams like `std::cout` or `std::cerr`.
 */
class StreamSink: public Sink {
	/// Output stream.
	mutable std::ostream os;

	void logEntry(std::string const& entry) const override {
		os << entry << std::flush;
	}

	explicit StreamSink(std::ostream& _os) :
		os(_os.rdbuf()) {}
public:
	/**
	 * Create a StreamSink from some output stream.
     * @param os Output stream.
     */
	static SinkPtr create(std::ostream& os) {
		return SinkPtr(new StreamSink(os));
	}
};
/**
 * Logging sink for file output.
 */
class FileSink: public Sink {
	/// File output stream.
	mutable std::ofstream os;

	void logEntry(std::string const& entry) const override {
		os << entry << std::flush;
	}

	explicit FileSink(const std::string& filename) :
		os(filename, std::ios::out) {}
public:
	/**
	 * Create a FileSink that logs to the specified file.
	 * The file is truncated upon construction.
     * @param filename
     */
	static SinkPtr create(const std::string& filename) {
		return SinkPtr(new FileSink(filename));
	}
};

}

