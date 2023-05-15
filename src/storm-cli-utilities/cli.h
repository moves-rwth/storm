#ifndef STORM_UTILITY_CLI_H_
#define STORM_UTILITY_CLI_H_

#include <cstdint>
#include <string>

namespace storm {
namespace cli {

/*!
 * Processes the options and returns the exit code.
 */
int64_t process(const int argc, const char** argv);

/*!
 * For a command-line argument, returns a quoted version
 * with single quotes if it contains unsafe characters.
 * Otherwise, just returns the unquoted argument.
 */
std::string shellQuoteSingleIfNecessary(const std::string& arg);

void printHeader(std::string const& name, const int argc, const char** argv);

void printVersion(std::string const& name);

void printTimeAndMemoryStatistics(uint64_t wallclockMilliseconds = 0);

/*!
 * Parses the given command line arguments.
 *
 * @param argc The argc argument of main().
 * @param argv The argv argument of main().
 * @return True iff the program should continue to run after parsing the options.
 */
bool parseOptions(const int argc, const char* argv[]);

void processOptions();

void setUrgentOptions();
}  // namespace cli
}  // namespace storm

#endif
