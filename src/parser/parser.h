/*
 * parser.h
 *
 *  Created on: 21.11.2012
 *      Author: Gereon Kremer
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "src/utility/osDetection.h"

#if defined LINUX || defined MACOSX
	#include <sys/mman.h>
#elif defined WINDOWS
#endif

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>

#include <pantheios/pantheios.hpp>
#include "src/exceptions/file_IO_exception.h"
#include "src/exceptions/wrong_file_format.h"

namespace mrmc {
namespace parser {
	
	/*!
	 *	@brief Parses integer and checks, if something has been parsed.
	 *
	 *	Calls strtol() internally and checks if the new pointer is different
	 *	from the original one, i.e. if str != *end. If they are the same, a
	 *	mrmc::exceptions::wrong_file_format will be thrown.
	 *	@param str String to parse
	 *	@param end New pointer will be written there
	 *	@return Result of strtol()
	 */
	inline uint_fast64_t checked_strtol(const char* str, char** end)
	{
		uint_fast64_t res = strtol(str, end, 10);
		if (str == *end) throw mrmc::exceptions::wrong_file_format();
		return res;
	}
	
	/*!
	 *	@brief Skips common whitespaces in a string.
	 *
	 *	Skips spaces, tabs, newlines and carriage returns. Returns pointer
	 *	to first char that is not a whitespace.
	 *	@param buf String buffer
	 *	@return	pointer to first non-whitespace character
	 */
	inline char* skipWS(char* buf)
	{
		while ((*buf == ' ') || (*buf == '\t') || (*buf == '\n') || (*buf == '\r')) buf++;
		return buf;
	}
	
	/*!
	 *	@brief Opens a file and maps it to memory providing a char*
	 *	containing the file content.
	 * 
	 *	This class is a very simple interface to read files efficiently.
	 *	The given file is opened and mapped to memory using mmap().
	 *	The public member data is a pointer to the actual file content.
	 *	Using this method, the kernel will take care of all buffering. This is
	 *	most probably much more efficient than doing this manually.
	 */

#if !defined LINUX && !defined MACOSX && !defined WINDOWS
	#error Platform not supported
#endif
	 
	class MappedFile
	{
		private:
			/*!
			 *	@brief file descriptor obtained by open().
			 */
#if defined LINUX || defined MACOSX
			int file;
#elif defined WINDOWS
			HANDLE file;
			HANDLE mapping;
#endif
			
			/*!
			 *	@brief stat information about the file.
			 */
#if defined LINUX || defined MACOSX
			struct stat64 st;
#elif defined WINDOWS
			struct __stat64 st;
#endif
			
		public:
			/*!
			 *	@brief pointer to actual file content.
			 */
			char* data;
			
			/*!
			 *	@brief pointer to end of file content.
			 */
			char* dataend;
		
		/*!
		 *	@brief Constructor of MappedFile.
		 *	
		 *	Will stat the given file, open it and map it to memory.
		 *	If anything of this fails, an appropriate exception is raised
		 *	and a log entry is written.
		 *	@param filename file to be opened
		 */
		MappedFile(const char* filename)
		{
#if defined LINUX || defined MACOSX
			/*
			 *	Do file mapping for reasonable systems.
			 *	stat64(), open(), mmap()
			 */
			if (stat64(filename, &(this->st)) != 0)
			{
				pantheios::log_ERROR("Could not stat ", filename, ". Does it exist? Is it readable?");
				throw exceptions::file_IO_exception("mrmc::parser::MappedFile Error in stat()");
			}

			this->file = open(filename, O_RDONLY);
			if (this->file < 0)
			{
				pantheios::log_ERROR("Could not open ", filename, ". Does it exist? Is it readable?");
				throw exceptions::file_IO_exception("mrmc::parser::MappedFile Error in open()");
			}
			
			this->data = (char*) mmap(NULL, this->st.st_size, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, this->file, 0);
			if (this->data == (char*)-1)
			{
				close(this->file);
				pantheios::log_ERROR("Could not mmap ", filename, ".");
				throw exceptions::file_IO_exception("mrmc::parser::MappedFile Error in mmap()");
			}
			this->dataend = this->data + this->st.st_size;
#elif defined WINDOWS
			/*
			 *	Do file mapping for windows.
			 *	_stat64(), CreateFile(), CreateFileMapping(), MapViewOfFile()
			 */
			if (_stat64(filename, &(this->st)) != 0)
			{
				pantheios::log_ERROR("Could not stat ", filename, ". Does it exist? Is it readable?");
				throw exceptions::file_IO_exception("mrmc::parser::MappedFile Error in stat()");
			}
			
			this->file = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (this->file == INVALID_HANDLE_VALUE)
			{
				pantheios::log_ERROR("Could not open ", filename, ". Does it exist? Is it readable?");
				throw exceptions::file_IO_exception("mrmc::parser::MappedFile Error in CreateFile()");
			}
			
			this->mapping = CreateFileMappingA(this->file, NULL, PAGE_READONLY, (DWORD)(st.st_size >> 32), (DWORD)st.st_size, NULL);
			if (this->mapping == NULL)
			{
				CloseHandle(this->file);
				pantheios::log_ERROR("Could not create file mapping for ", filename, ".");
				throw exceptions::file_IO_exception("mrmc::parser::MappedFile Error in CreateFileMapping()");
			}
			
			this->data = static_cast<char*>(MapViewOfFile(this->mapping, FILE_MAP_READ, 0, 0, this->st.st_size));
			if (this->data == NULL)
			{
				CloseHandle(this->mapping);
				CloseHandle(this->file);
				pantheios::log_ERROR("Could not create file map view for ", filename, ".");
				throw exceptions::file_IO_exception("mrmc::parser::MappedFile Error in MapViewOfFile()");
			}
			this->dataend = this->data + this->st.st_size;
#endif
		}
		
		/*!
		 *	@brief Destructor of MappedFile.
		 *	
		 *	Will unmap the data and close the file.
		 */
		~MappedFile()
		{
#if defined LINUX || defined MACOSX
			munmap(this->data, this->st.st_size);
			close(this->file);
#elif defined WINDOWS
			CloseHandle(this->mapping);
			CloseHandle(this->file);
#endif
		}
	};

} // namespace parser
} // namespace mrmc

#endif /* PARSER_H_ */