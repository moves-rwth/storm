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
#include <fcntl.h>
#include <errno.h>
#include <iostream>

#include <boost/integer/integer_mask.hpp>
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {

/*!
 *	@brief Contains all file parser and helper classes.
 *
 *	This namespace contains everything needed to load data files (like
 *	atomic propositions, transition systems, formulas, ...) including
 *	methods for efficient file access (see MappedFile).
 */
namespace parser {
	
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
	 
	class MappedFile {
		private:
#if defined LINUX || defined MACOSX
			/*!
			 *	@brief file descriptor obtained by open().
			 */
			int file;
#elif defined WINDOWS
			HANDLE file;
			HANDLE mapping;
#endif
			
#if defined LINUX
			/*!
			 *	@brief stat information about the file.
			 */
			struct stat64 st;
#elif defined MACOSX
			/*!
			 *	@brief stat information about the file.
			 */
			struct stat st;
#elif defined WINDOWS
			/*!
			 *	@brief stat information about the file.
			 */
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
		 */
		MappedFile(const char* filename);
		
		/*!
		 *	@brief Destructor of MappedFile.
		 */
		~MappedFile();
	};
	
	class Parser {
		protected:
			/*!
			 *	@brief Parses integer and checks, if something has been parsed.
			 */
			uint_fast64_t checked_strtol(const char* str, char** end);

			/*!
			 *	@brief Parses floating point and checks, if something has been parsed.
			 */
			double checked_strtod(const char* str, char** end);
	
			/*!
			 *	@brief Skips common whitespaces in a string.
			 */
			char* trimWhitespaces(char* buf);
	};

} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_PARSER_H_ */
