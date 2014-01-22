#include "src/parser/Parser.h"

#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cerrno>

#include <boost/integer/integer_mask.hpp>
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/utility/OsDetection.h"
#include "src/parser/MappedFile.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {

namespace parser {

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
 *  @brief Tests whether the given file exists and is readable.
 *  @return True iff the file exists and is readable.
 */
bool fileExistsAndIsReadable(const char* fileName) {
	std::ifstream fin(fileName);
	bool returnValue = !fin.fail();
	return returnValue;
}

/*!
 * Skips all numbers, letters and special characters.
 * Returns a pointer to the first char that is a whitespace.
 * @param buf The string buffer to operate on.
 * @return A pointer to the first whitespace character.
 */
char* skipWord(char* buf){
	while((*buf != ' ') && (*buf != '\t') && (*buf != '\n') && (*buf != '\r')) buf++;
	return buf;
}

/*!
 *	Skips spaces, tabs, newlines and carriage returns. Returns a pointer
 *	to first char that is not a whitespace.
 *	@param buf The string buffer to operate on.
 *	@return	A pointer to the first non-whitespace character.
 */
char* trimWhitespaces(char* buf) {
	while ((*buf == ' ') || (*buf == '\t') || (*buf == '\n') || (*buf == '\r')) buf++;
	return buf;
}

/*!
 * @briefs Analyzes the given file and tries to find out the used file endings.
 */
SupportedLineEndings findUsedLineEndings(std::string const& fileName, bool throwOnUnsupported) {
	MappedFile fileMap(fileName.c_str());
	char* buf = nullptr;
	char* const bufferEnd = fileMap.dataend;

	for (buf = fileMap.data; buf != bufferEnd; ++buf) {
		if (*buf == '\r') {
			// check for following \n
			if (((buf + sizeof(char)) < bufferEnd) && (*(buf + sizeof(char)) == '\n')) {
				return SupportedLineEndings::SlashRN;
			}
			return SupportedLineEndings::SlashR;
		} else if (*buf == '\n') {
			return SupportedLineEndings::SlashN;
		}
	}

	if (throwOnUnsupported) {
		LOG4CPLUS_ERROR(logger, "Error while parsing \"" << fileName << "\": Unsupported or unknown line-endings. Please use either of \\r, \\n or \\r\\n");
		throw storm::exceptions::WrongFormatException() << "Error while parsing \"" << fileName << "\": Unsupported or unknown line-endings. Please use either of \\r, \\n or \\r\\n";
	}
	LOG4CPLUS_WARN(logger, "Error while parsing \"" << fileName << "\": Unsupported or unknown line-endings. Please use either of \\r, \\n or \\r\\n");

	return SupportedLineEndings::Unsupported;
}

/*!
 * @brief Encapsulates the usage of function @strcspn to forward to the end of the line (next char is the newline character).
 */
char* forwardToLineEnd(char* buffer, SupportedLineEndings lineEndings) {
	switch (lineEndings) {
		case SupportedLineEndings::SlashN:
			return buffer + strcspn(buffer, "\n\0");
			break;
		case SupportedLineEndings::SlashR:
			return buffer + strcspn(buffer, "\r\0");
			break;
		case SupportedLineEndings::SlashRN:
			return buffer + strcspn(buffer, "\r\0");
			break;
		default:
		case SupportedLineEndings::Unsupported:
			// This Line will never be reached as the Parser would have thrown already.
			throw;
			break;
	}
	return nullptr;
}

/*!
 * @brief Encapsulates the usage of function @strchr to forward to the next line
 */
char* forwardToNextLine(char* buffer, SupportedLineEndings lineEndings) {
	switch (lineEndings) {
		case SupportedLineEndings::SlashN:
			return strchr(buffer, '\n') + 1;  
			break;
		case SupportedLineEndings::SlashR:
			return strchr(buffer, '\r') + 1;  
			break;
		case SupportedLineEndings::SlashRN:
			return strchr(buffer, '\r') + 2;
			break;
		default:
		case SupportedLineEndings::Unsupported:
			// This Line will never be reached as the Parser would have thrown already.
			throw;
			break;
	}
	return nullptr;
}

/*!
 * @brief Encapsulates the usage of function @sscanf to scan for the model type hint
 * @param targetBuffer The Target for the hint, must be at least 64 bytes long
 * @param buffer The Source Buffer from which the Model Hint will be read
 */
void scanForModelHint(char* targetBuffer, uint_fast64_t targetBufferSize, char const* buffer, SupportedLineEndings lineEndings) {
	if (targetBufferSize <= 4) {
		throw;
	}
	switch (lineEndings) {
		case SupportedLineEndings::SlashN:
#ifdef WINDOWS					
			sscanf_s(buffer, "%60s\n", targetBuffer, targetBufferSize);
#else
			sscanf(buffer, "%60s\n", targetBuffer);
#endif
			break;
		case SupportedLineEndings::SlashR:
#ifdef WINDOWS					
			sscanf_s(buffer, "%60s\r", targetBuffer, sizeof(targetBufferSize));
#else
			sscanf(buffer, "%60s\r", targetBuffer);
#endif
			break;
		case SupportedLineEndings::SlashRN:
#ifdef WINDOWS					
			sscanf_s(buffer, "%60s\r\n", targetBuffer, sizeof(targetBufferSize));
#else
			sscanf(buffer, "%60s\r\n", targetBuffer);
#endif
			break;
		default:
		case SupportedLineEndings::Unsupported:
			// This Line will never be reached as the Parser would have thrown already.
			throw;
			break;
	}
}

/*!
 * @brief Returns the matching Separator-String in the format of "BLANK\t\NEWLINESYMBOL(S)\0
 */
void getMatchingSeparatorString(char* targetBuffer, uint_fast64_t targetBufferSize, SupportedLineEndings lineEndings) {
	if (targetBufferSize < 5) {
		LOG4CPLUS_ERROR(logger, "getMatchingSeparatorString: The passed Target Buffer is too small.");
		throw;
	}
	switch (lineEndings) {
		case SupportedLineEndings::SlashN: {
			char source[] = " \n\t";
#ifdef WINDOWS			
			strncpy(targetBuffer, targetBufferSize, source, sizeof(source));
#else
			strncpy(targetBuffer, source, targetBufferSize);
#endif
			break;
											   }
		case SupportedLineEndings::SlashR: {
			char source[] = " \r\t";
#ifdef WINDOWS			
			strncpy(targetBuffer, targetBufferSize, source, sizeof(source));
#else
			strncpy(targetBuffer, source, targetBufferSize);
#endif
			break;
											   }
		case SupportedLineEndings::SlashRN: {
			char source[] = " \r\n\t";
#ifdef WINDOWS			
			strncpy(targetBuffer, targetBufferSize, source, sizeof(source));
#else
			strncpy(targetBuffer, source, targetBufferSize);
#endif
			break;
												}
		default:
		case SupportedLineEndings::Unsupported:
			// This Line will never be reached as the Parser would have thrown already.
			LOG4CPLUS_ERROR(logger, "getMatchingSeparatorString: The passed lineEndings were Unsupported. Check your input file.");
			throw;
			break;
	}
}

} // namespace parser

} // namespace storm
