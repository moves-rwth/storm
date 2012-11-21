/*
 * parser.h
 *
 *  Created on: 21.11.2012
 *      Author: Gereon Kremer
 */

#pragma once

#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#include <pantheios/pantheios.hpp>
#include "src/exceptions/file_IO_exception.h"

namespace mrmc {
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
	 
	class MappedFile
	{
		private:
			/*!
			 *	@brief file descriptor obtained by open().
			 */
			int file;
			
			/*!
			 *	@brief stat information about the file.
			 */
			struct stat st;
			
		public:
			/*!
			 *	@brief pointer to actual file content.
			 */
			char* data;
		
		/*!
		 *	@brief Constructor of MappedFile.
		 *	
		 *	Will stat the given file, open it and map it to memory.
		 *	If anything of this fails, an appropriate exception is raised
		 *	and a log entry is written.
		 *	@filename file to be opened
		 */
		MappedFile(const char* filename)
		{
			if (stat(filename, &(this->st)) != 0)
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
		}
		
		/*!
		 *	@brief Destructor of MappedFile.
		 *	
		 *	Will unmap the data and close the file.
		 */
		~MappedFile()
		{
			munmap(this->data, this->st.st_size);
			close(this->file);
		}
	};

} // namespace parser
} // namespace mrmc