#pragma once

#include <cstdint>

namespace storm {
namespace utility {
namespace cstring {

/*!
 *	@brief Parses integer and checks, if something has been parsed.
 */
uint_fast64_t checked_strtol(const char* str, char const** end);

/*!
 *	@brief Parses floating point and checks, if something has been parsed.
 */
double checked_strtod(const char* str, char const** end);

/*!
 * @brief Skips all non whitespace characters until the next whitespace.
 */
char const* skipWord(char const* buf);

/*!
 *	@brief Skips common whitespaces in a string.
 */
char const* trimWhitespaces(char const* buf);

/*!
 * @brief Encapsulates the usage of function @strcspn to forward to the end of the line (next char is the newline character).
 */
char const* forwardToLineEnd(char const* buffer);

/*!
 * @brief Encapsulates the usage of function @strchr to forward to the next line
 *
 * Note: All lines after the current, which do not contain any characters are skipped.
 */
char const* forwardToNextLine(char const* buffer);

}  // namespace cstring
}  // namespace utility
}  // namespace storm
