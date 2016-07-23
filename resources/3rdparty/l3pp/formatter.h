/**
 * @file formatter.h
 * @author Harold Bruintjes <h.bruintjes@cs.rwth-aachen.de>
 *
 * Define the Formatter class, used to format output of a sink
 */

#pragma once

#include <string>
#include <tuple>

namespace l3pp {

/**
 * Formats a log messages. This is a base class that simply print the message
 * with the log level prefix, see derived classes such as TemplatedFormatter
 * for more interesting data.
 */
class Formatter {
	friend class Logger;

	static void initialize();

	virtual std::string format(EntryContext const& context, std::string const& msg) const;
public:
	virtual ~Formatter() {}

	std::string operator()(EntryContext const& context, std::string const& msg) {
		return format(context, msg);
	}
};
typedef std::shared_ptr<Formatter> FormatterPtr;

/**
 * Possible fields for FieldStr instance
 */
enum class Field {
	/// Name of the file (everything following the last path separator)
	FileName,
	/// Full path of the file
	FilePath,
	/// Line number
	Line,
	/// Name of function currently executed
	Function,
	/// Name of the logger
	LoggerName,
	/// Message to be logged
	Message,
	/// Level of the log entry
	LogLevel,
	/// Number of milliseconds since the logger was initialized
	WallTime,
};

/**
 * Controls justification of formatted log fields.
 */
enum class Justification {
	/// Left align field
	LEFT,
	/// Right align field
	RIGHT
};



/**
 * Formatter for log entry fields, with the exception of time stamp formatting
 * (see TimeStr for that). The Field template argument determines which field
 * is printed, see logging::Field.
 * The other template arguments control the alignment of the output string.
 */
template<Field field, int Width = 0, Justification j = Justification::RIGHT, char Fill = ' '>
class FieldStr {
public:
	void stream(std::ostream& os, EntryContext const& context, std::string const& msg) const;
};

/**
 * Formatter for log time stamps. The constructor expects a single string
 * argument which is a formatter for the time stamp. For the specification of
 * this format string see the documentation for std::put_time . You can use for
 * example "%c" or "%T".
 * The template arguments control the alignment of the output string.
 */
class TimeStr {
	std::string formatStr;

public:
	TimeStr(char const* format) : formatStr(format) {
	}
	TimeStr(std::string const& format) : formatStr(format) {
	}

	void stream(std::ostream& os, EntryContext const& context, std::string const&) const;
};

/**
 * Formatter which formats the output based on the (templated) arguments given.
 * The arguments can be anything that implements the stream operator <<, but
 * more interestingly also the various FormatField subclasses. These classes
 * can output the various fields associated with a log entry.
 */
template<typename ... Formatters>
class TemplateFormatter : public Formatter {
	std::tuple<Formatters...> formatters;

	template <int N>
	typename std::enable_if<N < (sizeof...(Formatters))>::type
	formatTuple(EntryContext const& context, std::string const& msg, std::ostream& os) const {
		formatElement(std::get<N>(formatters), os, context, msg);
		formatTuple<N+1>(context, msg, os);
	}

	template <int N>
	typename std::enable_if<(N >= sizeof...(Formatters))>::type
	formatTuple(EntryContext const&, std::string const&, std::ostream&) const {
	}

	template<Field field, int Width, Justification j, char Fill>
	void formatElement(FieldStr<field, Width, j, Fill> const& t, std::ostream& stream, EntryContext const& context, std::string const& msg) const {
		t.stream(stream, context, msg);
	}

	void formatElement(TimeStr const& t, std::ostream& stream, EntryContext const& context, std::string const& msg) const {
		t.stream(stream, context, msg);
	}

	template<typename T>
	void formatElement(T const& t, std::ostream& stream, EntryContext const&, std::string const&) const {
		stream << t;
	}
public:
	TemplateFormatter(Formatters ... formatters) :
		formatters(std::forward<Formatters>(formatters)...)
	{
	}

	std::string format(EntryContext const& context, std::string const& msg) const override;
};

/**
 * Helper function to create a TemplateFormatter. Simply call with some
 * formatable arguments, e.g.,
 * @code{.cpp}
 * logging::makeTemplateFormatter(
 *     logging::FieldStr<logging::Field::LogLevel>(), " - ",
 *     logging::FieldStr<logging::Field::Message>(), "\n");
 * @endcode
 */
template<typename ... Formatters>
FormatterPtr makeTemplateFormatter(Formatters&& ... formatters) {
	return std::make_shared<TemplateFormatter<Formatters...>>(std::forward<Formatters>(formatters)...);
}

}
