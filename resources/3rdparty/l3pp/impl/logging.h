/**
 * @file logging.h
 *
 * Implementation of general logging functionality
 */

#pragma once

#include <ostream>

namespace l3pp {

/**
 * Streaming operator for LogLevel.
 * @param os Output stream.
 * @param level LogLevel.
 * @return os.
 */
inline std::ostream& operator<<(std::ostream& os, LogLevel level) {
	switch (level) {
		case LogLevel::TRACE:   return os << "TRACE";
		case LogLevel::DEBUG:   return os << "DEBUG";
		case LogLevel::INFO:    return os << "INFO";
		case LogLevel::WARN:    return os << "WARN";
		case LogLevel::ERR:     return os << "ERROR";
		case LogLevel::FATAL:   return os << "FATAL";
		case LogLevel::OFF:     return os << "OFF";
		default:                return os << "???";
	}
}

}
