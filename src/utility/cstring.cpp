#include "src/utility/cstring.h"

#include <cstring>

#include "src/exceptions/WrongFormatException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {

namespace utility {

namespace cstring {

/*!
 *	Calls strtol() internally and checks if the new pointer is different
 *	from the original one, i.e. if str != *end. If they are the same, a
 *	storm::exceptions::WrongFormatException will be thrown.
 *	@param str String to parse
 *	@param end New pointer will be written there
 *	@return Result of strtol()
 */
uint_fast64_t checked_strtol(const char* str, char** end) {
	uint_fast64_t res = strtol(str, end, 10);
	if (str == *end) {
		LOG4CPLUS_ERROR(logger, "Error while parsing integer. Next input token is not a number.");
		LOG4CPLUS_ERROR(logger, "\tUpcoming input is: \"" << std::string(str, 0, 16) << "\"");
		throw storm::exceptions::WrongFormatException("Error while parsing integer. Next input token is not a number.");
	}
	return res;
}

/*!
 *	Calls strtod() internally and checks if the new pointer is different
 *	from the original one, i.e. if str != *end. If they are the same, a
 *	storm::exceptions::WrongFormatException will be thrown.
 *	@param str String to parse
 *	@param end New pointer will be written there
 *	@return Result of strtod()
 */
double checked_strtod(const char* str, char** end) {
	double res = strtod(str, end);
	if (str == *end) {
		LOG4CPLUS_ERROR(logger, "Error while parsing floating point. Next input token is not a number.");
		LOG4CPLUS_ERROR(logger, "\tUpcoming input is: \"" << std::string(str, 0, 16) << "\"");
		throw storm::exceptions::WrongFormatException("Error while parsing floating point. Next input token is not a number.");
	}
	return res;
}

/*!
 * Skips all numbers, letters and special characters.
 * Returns a pointer to the first char that is a whitespace.
 * @param buf The string buffer to operate on.
 * @return A pointer to the first whitespace character.
 */
char* skipWord(char* buf){
	while(!isspace(*buf) && *buf != '\0') buf++;
	return buf;
}

/*!
 *	Skips spaces, tabs, newlines and carriage returns. Returns a pointer
 *	to first char that is not a whitespace.
 *	@param buf The string buffer to operate on.
 *	@return	A pointer to the first non-whitespace character.
 */
char* trimWhitespaces(char* buf) {
	while (isspace(*buf)) buf++;
	return buf;
}

/*!
 * @brief Encapsulates the usage of function @strcspn to forward to the end of the line (next char is the newline character).
 */
char* forwardToLineEnd(char* buffer) {
	return buffer + strcspn(buffer, "\n\r\0");
}

/*!
 * @brief Encapsulates the usage of function @strchr to forward to the next line
 */
char* forwardToNextLine(char* buffer) {
	char* lineEnd = forwardToLineEnd(buffer);
	while((*lineEnd == '\n') || (*lineEnd == '\r')) lineEnd++;
	return lineEnd;
}

} // namespace cstring

} // namespace utility

} // namespace storm
