#pragma once

#include <cstdint>
#include <string>

namespace storm {
namespace cli {

/*!
 * For a command-line argument, returns a quoted version
 * with single quotes if it contains unsafe characters.
 * Otherwise, just returns the unquoted argument.
 */
std::string shellQuoteSingleIfNecessary(const std::string& arg);

void printHeader(std::string const& name, const int argc, const char** argv);

void printVersion();

void printTimeAndMemoryStatistics(uint64_t wallclockMilliseconds = 0);

}  // namespace cli
}  // namespace storm
