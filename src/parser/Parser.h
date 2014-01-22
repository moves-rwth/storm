/*
 * Parser.h
 *
 *  Created on: 21.11.2012
 *      Author: Gereon Kremer
 */

#ifndef STORM_PARSER_PARSER_H_
#define STORM_PARSER_PARSER_H_

#include "src/utility/OsDetection.h"

#include <sys/stat.h>
#include <vector>
#include <string>

namespace storm {

	/*!
	 *	@brief Contains all file parsers and helper classes.
	 *
	 *	This namespace contains everything needed to load data files (like
	 *	atomic propositions, transition systems, formulas, etc.) including
	 *	methods for efficient file access (see MappedFile).
	 */
	namespace parser {

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
	 *  @brief Tests whether the given file exists and is readable.
	 */
	bool fileExistsAndIsReadable(const char* fileName);

	/*!
	 * @brief Enum Class Type containing all supported file endings.
	 */
	enum class SupportedLineEndings : unsigned short {
		Unsupported = 0,
		SlashR,
		SlashN,
		SlashRN
	};

	/*!
	 * @briefs Analyzes the given file and tries to find out the used line endings.
	 */
	storm::parser::SupportedLineEndings findUsedLineEndings(std::string const& fileName, bool throwOnUnsupported = false);

	/*!
	 * @brief Encapsulates the usage of function @strcspn to forward to the end of the line (next char is the newline character).
	 */
	char* forwardToLineEnd(char* buffer, storm::parser::SupportedLineEndings lineEndings);

	/*!
	 * @brief Encapsulates the usage of function @strchr to forward to the next line
	 */
	char* forwardToNextLine(char* buffer, storm::parser::SupportedLineEndings lineEndings);

	/*!
	 * @brief Encapsulates the usage of function @sscanf to scan for the model type hint
	 * @param targetBuffer The Target for the hint, should be at least 64 bytes long
	 * @param buffer The Source Buffer from which the Model Hint will be read
	 */
	void scanForModelHint(char* targetBuffer, uint_fast64_t targetBufferSize, char const* buffer, storm::parser::SupportedLineEndings lineEndings);
	
	/*!
	 * @brief Returns the matching Separator-String in the format of "BLANK\t\NEWLINESYMBOL(S)\0
	 */
	void getMatchingSeparatorString(char* targetBuffer, uint_fast64_t targetBufferSize, storm::parser::SupportedLineEndings lineEndings);

	} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_PARSER_H_ */
