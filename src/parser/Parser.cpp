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
storm::parser::SupportedLineEndingsEnum storm::parser::findUsedLineEndings(std::string const& fileName) {
	storm::parser::SupportedLineEndingsEnum result = storm::parser::SupportedLineEndingsEnum::Unsupported;

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
	return storm::parser::SupportedLineEndingsEnum::Unsupported;
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
