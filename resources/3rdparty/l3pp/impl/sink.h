/**
 * @file sink.h
 *
 * Implementation for Sinks
 */

#pragma once

namespace l3pp {

inline void Sink::log(EntryContext const& context, std::string const& message) const {
	if (context.level >= this->level) {
		logEntry((*formatter)(context, message));
	}
}

}

