#include "src/parser/Parser.h"

#include <iostream>
#include <cstring>
#include <string>
#include <cerrno>

#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFormatException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

/*!
 *	Calls strtol() internally and checks if the new pointer is different
 *	from the original one, i.e. if str != *end. If they are the same, a
 *	storm::exceptions::WrongFormatException will be thrown.
 *	@param str String to parse
 *	@param end New pointer will be written there
 *	@return Result of strtol()
 */
uint_fast64_t storm::parser::checked_strtol(const char* str, char** end) {
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
double storm::parser::checked_strtod(const char* str, char** end) {
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
bool storm::parser::fileExistsAndIsReadable(const char* fileName) {
	std::ifstream fin(fileName);
	bool returnValue = !fin.fail();
	return returnValue;
}

/*!
 *	Skips spaces, tabs, newlines and carriage returns. Returns pointer
 *	to first char that is not a whitespace.
 *	@param buf String buffer
 *	@return	pointer to first non-whitespace character
 */
char* storm::parser::trimWhitespaces(char* buf) {
	while ((*buf == ' ') || (*buf == '\t') || (*buf == '\n') || (*buf == '\r')) buf++;
	return buf;
}

/*!
 * @briefs Analyzes the given file and tries to find out the used file endings.
 */
storm::parser::SupportedLineEndingsEnum storm::parser::findUsedLineEndings(std::string const& fileName, bool throwOnUnsupported) {
	MappedFile fileMap(fileName.c_str());
	char* buf = nullptr;
	char* const bufferEnd = fileMap.dataend;

	bool sawR = false;
	for (buf = fileMap.data; buf != bufferEnd; ++buf) {
		if (*buf == '\r') {
			// check for following \n
			if (((buf + sizeof(char)) < bufferEnd) && (*(buf + sizeof(char)) == '\n')) {
				return storm::parser::SupportedLineEndingsEnum::SlashRN; 
			}
			return storm::parser::SupportedLineEndingsEnum::SlashR; 
		} else if (*buf == '\n') {
			return storm::parser::SupportedLineEndingsEnum::SlashN;
		}
	}

	if (throwOnUnsupported) {
		LOG4CPLUS_ERROR(logger, "Error while parsing \"" << fileName << "\": Unsupported or unknown line-endings. Please use either of \\r, \\n or \\r\\n");
		throw storm::exceptions::WrongFormatException() << "Error while parsing \"" << fileName << "\": Unsupported or unknown line-endings. Please use either of \\r, \\n or \\r\\n";
	}
	LOG4CPLUS_WARN(logger, "Error while parsing \"" << fileName << "\": Unsupported or unknown line-endings. Please use either of \\r, \\n or \\r\\n");

	return storm::parser::SupportedLineEndingsEnum::Unsupported;
}

/*!
 * @brief Encapsulates the usage of function @strchr to forward to the next line
 */
char* storm::parser::forwardToNextLine(char* buffer, storm::parser::SupportedLineEndingsEnum lineEndings) {
	switch (lineEndings) {
		case storm::parser::SupportedLineEndingsEnum::SlashN:
			return strchr(buffer, '\n') + 1;  
			break;
		case storm::parser::SupportedLineEndingsEnum::SlashR:
			return strchr(buffer, '\r') + 1;  
			break;
		case storm::parser::SupportedLineEndingsEnum::SlashRN:
			return strchr(buffer, '\r') + 2;
			break;
		default:
		case storm::parser::SupportedLineEndingsEnum::Unsupported:
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
void storm::parser::scanForModelHint(char* targetBuffer, uint_fast64_t targetBufferSize, char const* buffer, storm::parser::SupportedLineEndingsEnum lineEndings) {
	if (targetBufferSize <= 4) {
		throw;
	}
	switch (lineEndings) {
		case storm::parser::SupportedLineEndingsEnum::SlashN:
#ifdef WINDOWS					
			sscanf_s(buffer, "%60s\n", targetBuffer, targetBufferSize);
#else
			sscanf(buffer, "%60s\n", targetBuffer);
#endif
			break;
		case storm::parser::SupportedLineEndingsEnum::SlashR:
#ifdef WINDOWS					
			sscanf_s(buffer, "%60s\r", targetBuffer, sizeof(targetBufferSize));
#else
			sscanf(buffer, "%60s\r", targetBuffer);
#endif
			break;
		case storm::parser::SupportedLineEndingsEnum::SlashRN:
#ifdef WINDOWS					
			sscanf_s(buffer, "%60s\r\n", targetBuffer, sizeof(targetBufferSize));
#else
			sscanf(buffer, "%60s\r\n", targetBuffer);
#endif
			break;
		default:
		case storm::parser::SupportedLineEndingsEnum::Unsupported:
			// This Line will never be reached as the Parser would have thrown already.
			throw;
			break;
	}
}

/*!
 * @brief Returns the matching Separator-String in the format of "BLANK\t\NEWLINESYMBOL(S)\0
 */
void storm::parser::getMatchingSeparatorString(char* targetBuffer, uint_fast64_t targetBufferSize, storm::parser::SupportedLineEndingsEnum lineEndings) {
	if (targetBufferSize < 5) {
		LOG4CPLUS_ERROR(logger, "storm::parser::getMatchingSeparatorString: The passed Target Buffer is too small.");
		throw;
	}
	switch (lineEndings) {
		case SupportedLineEndingsEnum::SlashN: {
			char source[] = " \n\t";
			strncpy(targetBuffer, targetBufferSize, source, sizeof(source));
			break;
											   }
		case SupportedLineEndingsEnum::SlashR: {
			char source[] = " \r\t";
			strncpy(targetBuffer, targetBufferSize, source, sizeof(source));
			break;
											   }
		case SupportedLineEndingsEnum::SlashRN: {
			char source[] = " \r\n\t";
			strncpy(targetBuffer, targetBufferSize, source, sizeof(source));
			break;
												}
		default:
		case SupportedLineEndingsEnum::Unsupported:
			// This Line will never be reached as the Parser would have thrown already.
			LOG4CPLUS_ERROR(logger, "storm::parser::getMatchingSeparatorString: The passed lineEndings were Unsupported. Check your input file.");
			throw;
			break;
	}
}

/*!
 *	Will stat the given file, open it and map it to memory.
 *	If anything of this fails, an appropriate exception is raised
 *	and a log entry is written.
 *	@param filename file to be opened
 */
storm::parser::MappedFile::MappedFile(const char* filename) {
#if defined LINUX || defined MACOSX
	/*
	 *	Do file mapping for reasonable systems.
	 *	stat64(), open(), mmap()
	 */
#ifdef MACOSX
	if (stat(filename, &(this->st)) != 0) {
#else
	if (stat64(filename, &(this->st)) != 0) {
#endif
		LOG4CPLUS_ERROR(logger, "Error in stat(" << filename << "): Probably, this file does not exist.");
		throw exceptions::FileIoException() << "storm::parser::MappedFile Error in stat(): Probably, this file does not exist.";
	}
	this->file = open(filename, O_RDONLY);

	if (this->file < 0) {
		LOG4CPLUS_ERROR(logger, "Error in open(" << filename << "): Probably, we may not read this file.");
		throw exceptions::FileIoException() << "storm::parser::MappedFile Error in open(): Probably, we may not read this file.";
	}

	this->data = reinterpret_cast<char*>(mmap(NULL, this->st.st_size, PROT_READ, MAP_PRIVATE, this->file, 0));
	if (this->data == reinterpret_cast<char*>(-1)) {
		close(this->file);
		LOG4CPLUS_ERROR(logger, "Error in mmap(" << filename << "): " << std::strerror(errno));
		throw exceptions::FileIoException() << "storm::parser::MappedFile Error in mmap(): " << std::strerror(errno);
	}
	this->dataend = this->data + this->st.st_size;
#elif defined WINDOWS
	/*
	 *	Do file mapping for windows.
	 *	_stat64(), CreateFile(), CreateFileMapping(), MapViewOfFile()
	 */
	if (_stat64(filename, &(this->st)) != 0) {
		LOG4CPLUS_ERROR(logger, "Error in _stat(" << filename << "): Probably, this file does not exist.");
		throw exceptions::FileIoException("storm::parser::MappedFile Error in stat(): Probably, this file does not exist.");
	}

	this->file = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (this->file == INVALID_HANDLE_VALUE) {
		LOG4CPLUS_ERROR(logger, "Error in CreateFileA(" << filename << "): Probably, we may not read this file.");
		throw exceptions::FileIoException("storm::parser::MappedFile Error in CreateFileA(): Probably, we may not read this file.");
	}

	this->mapping = CreateFileMappingA(this->file, NULL, PAGE_READONLY, (DWORD)(st.st_size >> 32), (DWORD)st.st_size, NULL);
	if (this->mapping == NULL) {
		CloseHandle(this->file);
		LOG4CPLUS_ERROR(logger, "Error in CreateFileMappingA(" << filename << ").");
		throw exceptions::FileIoException("storm::parser::MappedFile Error in CreateFileMappingA().");
	}

	this->data = static_cast<char*>(MapViewOfFile(this->mapping, FILE_MAP_READ, 0, 0, this->st.st_size));
	if (this->data == NULL) {
		CloseHandle(this->mapping);
		CloseHandle(this->file);
		LOG4CPLUS_ERROR(logger, "Error in MapViewOfFile(" << filename << ").");
		throw exceptions::FileIoException("storm::parser::MappedFile Error in MapViewOfFile().");
	}
	this->dataend = this->data + this->st.st_size;
#endif
}

/*!
 *	Will unmap the data and close the file.
 */
storm::parser::MappedFile::~MappedFile() {
#if defined LINUX || defined MACOSX
	munmap(this->data, this->st.st_size);
	close(this->file);
#elif defined WINDOWS
	CloseHandle(this->mapping);
	CloseHandle(this->file);
#endif
}
