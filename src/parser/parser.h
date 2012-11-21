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

namespace mrmc
{
namespace parser
{

	class MappedFile
	{
		private:
			int file;
			struct stat st;
			
		public:
			 char* data;

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
		
		~MappedFile()
		{
			munmap(this->data, this->st.st_size);
			close(this->file);
		}
	};

}
}