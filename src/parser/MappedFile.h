/*
 * MappedFile.h
 *
 *  Created on: Jan 21, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_PARSER_MAPPEDFILE_H_
#define STORM_PARSER_MAPPEDFILE_H_

#include <sys/stat.h>

#include "src/utility/OsDetection.h"

namespace storm {
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

		public:

			/*!
			 * Constructor of MappedFile.
			 * Will stat the given file, open it and map it to memory.
			 * If anything of this fails, an appropriate exception is raised
			 * and a log entry is written.
			 * @param filename file to be opened
			 */
			MappedFile(const char* filename);

			/*!
			 * Destructor of MappedFile.
			 * Will unmap the data and close the file.
			 */
			~MappedFile();

			/*!
			 *	@brief pointer to actual file content.
			 */
			char* data;

			/*!
			 *	@brief pointer to end of file content.
			 */
			char* dataend;

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
		};
	} // namespace parser
} // namespace storm



#endif /* STORM_PARSER_MAPPEDFILE_H_ */
