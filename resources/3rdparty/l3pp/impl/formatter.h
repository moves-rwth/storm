/**
 * @file formatter.h
 * @author Harold Bruintjes <h.bruintjes@cs.rwth-aachen.de>
 *
 * Implementation of Formatter classes
 */

#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstring> //strrchr

namespace l3pp {

namespace detail {
	/**
	 * Internal function to get wall-time
	 */
	inline static std::chrono::system_clock::time_point GetStartTime() {
		static std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
		return startTime;
	}
}

inline void Formatter::initialize() {
	// Init wall-time
	detail::GetStartTime();
}

inline std::string Formatter::format(EntryContext const& context, std::string const& msg) const {
	std::stringstream stream;
	stream << context.level << " - " << msg << '\n';
	return stream.str();
}

template<Field field, int Width, Justification j, char Fill>
inline void FieldStr<field, Width, j, Fill>::stream(std::ostream& os, EntryContext const& context, std::string const& msg) const {
	os << std::setw(Width);
	os << std::setfill(Fill);
	switch(j) {
		case Justification::LEFT:
			os << std::left;
		case Justification::RIGHT:
			os << std::right;
	}

	switch(field) {
		case Field::FileName:
#ifdef _WIN32
			os << strrchr(context.filename, '\\')+1;
#else
			os << strrchr(context.filename, '/')+1;
#endif
			break;
		case Field::FilePath:
			os << context.filename;
			break;
		case Field::Line:
			os << context.line;
			break;
		case Field::Function:
			os << context.funcname;
			break;
		case Field::LoggerName:
			os << context.logger->getName();
			break;
		case Field::Message:
			os << msg;
			break;
		case Field::LogLevel:
			os << context.level;
			break;
		case Field::WallTime:
			auto runtime = context.timestamp - detail::GetStartTime();
			os << std::chrono::duration_cast<std::chrono::milliseconds>(runtime).count();
			break;
	}
}

inline void TimeStr::stream(std::ostream& os, EntryContext const& context, std::string const&) const {
	auto time = std::chrono::system_clock::to_time_t(context.timestamp);
	auto timeinfo = localtime (&time);
#if __GNUC__ >= 5 || __clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 7) || _MSC_VER >= 1700
//TODO: Need better way to detect thing
	os << std::put_time(timeinfo, formatStr.c_str());
#else
	char buffer[1024];
	if (strftime(buffer, 1024, formatStr.c_str(), timeinfo)) {
		os << buffer;
	}
#endif
}

template<typename ... Formatters>
inline std::string TemplateFormatter<Formatters...>::format(EntryContext const& context, std::string const& msg) const {
	std::stringstream stream;

	formatTuple<0>(context, msg, stream);

	return stream.str();
}

}
