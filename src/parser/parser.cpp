#include "src/parser/parser.h"

#if defined LINUX || defined MACOSX
#	include <sys/mman.h>
#	include <unistd.h>
#elif defined WINDOWS
#endif

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <cstring>

#include "src/exceptions/file_IO_exception.h"
#include "src/exceptions/wrong_file_format.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

/*!
 *	Calls strtol() internally and checks if the new pointer is different
 *	from the original one, i.e. if str != *end. If they are the same, a
 *	mrmc::exceptions::wrong_file_format will be thrown.
 *	@param str String to parse
 *	@param end New pointer will be written there
 *	@return Result of strtol()
 */
uint_fast64_t mrmc::parser::Parser::checked_strtol(const char* str, char** end) {
	uint_fast64_t res = strtol(str, end, 10);
	if (str == *end) {
		LOG4CPLUS_ERROR(logger, "Error while parsing integer. Next input token is not a number.");
		LOG4CPLUS_ERROR(logger, "\tUpcoming input is: \"" << std::string(str, 0, 16) << "\"");
		throw mrmc::exceptions::wrong_file_format();
	}
	return res;
}

/*!
 *	Skips spaces, tabs, newlines and carriage returns. Returns pointer
 *	to first char that is not a whitespace.
 *	@param buf String buffer
 *	@return	pointer to first non-whitespace character
 */
char* mrmc::parser::Parser::skipWS(char* buf) {
	while ((*buf == ' ') || (*buf == '\t') || (*buf == '\n') || (*buf == '\r')) buf++;
	return buf;
}
	
/*!
 *	Will stat the given file, open it and map it to memory.
 *	If anything of this fails, an appropriate exception is raised
 *	and a log entry is written.
 *	@param filename file to be opened
 */
mrmc::parser::MappedFile::MappedFile(const char* filename) {
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
		LOG4CPLUS_ERROR(logger, "Error in stat(" << filename << ").");
		throw exceptions::file_IO_exception("mrmc::parser::MappedFile Error in stat()");
	}
	this->file = open(filename, O_RDONLY);

	if (this->file < 0) {
		LOG4CPLUS_ERROR(logger, "Error in open(" << filename << ").");
		throw exceptions::file_IO_exception("mrmc::parser::MappedFile Error in open()");
	}
			
	this->data = (char*) mmap(NULL, this->st.st_size, PROT_READ, MAP_PRIVATE, this->file, 0);
	if (this->data == (char*)-1) {
		close(this->file);
		LOG4CPLUS_ERROR(logger, "Error in mmap(" << filename << ").");
		throw exceptions::file_IO_exception("mrmc::parser::MappedFile Error in mmap()");
	}
	this->dataend = this->data + this->st.st_size;
#elif defined WINDOWS
	/*
	 *	Do file mapping for windows.
	 *	_stat64(), CreateFile(), CreateFileMapping(), MapViewOfFile()
	 */
	if (_stat64(filename, &(this->st)) != 0) {
		LOG4CPLUS_ERROR(logger, "Error in _stat(" << filename << ").");
		throw exceptions::file_IO_exception("mrmc::parser::MappedFile Error in stat()");
	}
		
	this->file = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (this->file == INVALID_HANDLE_VALUE) {
		LOG4CPLUS_ERROR(logger, "Error in CreateFileA(" << filename << ").");
		throw exceptions::file_IO_exception("mrmc::parser::MappedFile Error in CreateFileA()");
	}
			
	this->mapping = CreateFileMappingA(this->file, NULL, PAGE_READONLY, (DWORD)(st.st_size >> 32), (DWORD)st.st_size, NULL);
	if (this->mapping == NULL) {
		CloseHandle(this->file);
		LOG4CPLUS_ERROR(logger, "Error in CreateFileMappingA(" << filename << ").");
		throw exceptions::file_IO_exception("mrmc::parser::MappedFile Error in CreateFileMappingA()");
	}
			
	this->data = static_cast<char*>(MapViewOfFile(this->mapping, FILE_MAP_READ, 0, 0, this->st.st_size));
	if (this->data == NULL) {
		CloseHandle(this->mapping);
		CloseHandle(this->file);
		LOG4CPLUS_ERROR(logger, "Error in MapViewOfFile(" << filename << ").");
		throw exceptions::file_IO_exception("mrmc::parser::MappedFile Error in MapViewOfFile()");
	}
	this->dataend = this->data + this->st.st_size;
#endif
}
		
/*!
 *	Will unmap the data and close the file.
 */
mrmc::parser::MappedFile::~MappedFile() {
#if defined LINUX || defined MACOSX
	munmap(this->data, this->st.st_size);
	close(this->file);
#elif defined WINDOWS
	CloseHandle(this->mapping);
	CloseHandle(this->file);
#endif
}
