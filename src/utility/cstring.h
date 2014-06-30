/*
 * cstring.h
 *
 *  Created on: 30.01.2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_UTILITY_CSTRING_H_
#define STORM_UTILITY_CSTRING_H_

#include <cstdint>

namespace storm {
	namespace utility {
		namespace cstring {

		/*!
		 *	@brief Parses integer and checks, if something has been parsed.
		 */
		uint_fast64_t checked_strtol(const char* str, char** end);

		/*!
		 *	@brief Parses floating point and checks, if something has been parsed.
		 */
		double checked_strtod(const char* str, char** end);

		/*!
		 * @brief Skips all non whitespace characters until the next whitespace.
		 */
		char* skipWord(char* buf);

		/*!
		 *	@brief Skips common whitespaces in a string.
		 */
		char* trimWhitespaces(char* buf);

		/*!
		 * @brief Encapsulates the usage of function @strcspn to forward to the end of the line (next char is the newline character).
		 */
		char* forwardToLineEnd(char* buffer);

		/*!
		 * @brief Encapsulates the usage of function @strchr to forward to the next line
		 *
		 * Note: All lines after the current, which do not contain any characters are skipped.
		 */
		char* forwardToNextLine(char* buffer);

		} // namespace cstring
	} // namespace utility
} // namespace storm

#endif /* STORM_UTILITY_CSTRING_H_ */
