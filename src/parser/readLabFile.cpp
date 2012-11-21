/*
 * readLabFile.cpp
 *
 *  Created on: 21.11.2012
 *      Author: Gereon Kremer
 */

#include "parser.h"

#include "readLabFile.h"

#include "src/exceptions/wrong_file_format.h"
#include "src/exceptions/file_IO_exception.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <clocale>
#include <iostream>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <locale.h>

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>

namespace mrmc {

namespace parser {


/*!
 * Reads a .lab file and puts the result in a labeling structure.
 *
 * Labelings created with this method have to be freed with the delete operator.
 * @param node_count the number of states.
 * @param filename   input .lab file's name.
 * @return The pointer to the created labeling object.
 */
mrmc::models::AtomicPropositionsLabeling * readLabFile(int node_count, const char * filename)
{
	/*!
	 *	open file and map to memory
	 */
	struct stat st;
	int f = open(filename, O_RDONLY);
	if ((f < 0) || (stat(filename, &st) != 0)) {
		/*!
		 * 
		 */
		pantheios::log_ERROR("File could not be opened.");
		throw mrmc::exceptions::file_IO_exception();
		return NULL;
	}
	
	char* data = (char*) mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, f, 0);
	if (data == (char*)-1)
	{
		pantheios::log_ERROR("File could not be mapped. Something went wrong with mmap.");
		close(f);
		throw mrmc::exceptions::file_IO_exception();
		return NULL;
	}

	char* buf = data;
	char sep[] = " \n\t";
	uint_fast32_t proposition_count = 0;
	size_t cnt = 0;

	do {
		buf += cnt;
		cnt = strcspn(buf, sep);
		if (cnt > 0) {
			if (strncmp(buf, "#DECLARATION", cnt) == 0) continue;
			if (strncmp(buf, "#END", cnt) == 0) break;
			proposition_count++;
		}
		else cnt = 1;
	} while (cnt > 0);
	
	mrmc::models::AtomicPropositionsLabeling* result = new mrmc::models::AtomicPropositionsLabeling(node_count, proposition_count);
	char proposition[128];
	buf = data;
	cnt = 0;
	do {
		buf += cnt;
		cnt = strcspn(buf, sep);
		if (cnt > 0) {
			if (strncmp(buf, "#DECLARATION", cnt) == 0) continue;
			if (strncmp(buf, "#END", cnt) == 0) break;
			strncpy(proposition, buf, cnt);
			proposition[cnt] = '\0';
			result->addAtomicProposition(proposition);
		}
		else cnt = 1;
	} while (cnt > 0);
	buf += 4;
	
	uint_fast32_t node;
	while (1) {
		node = strtol(buf, &buf, 10);
		while (*buf != '\0') {
			cnt = strcspn(buf, sep);
			if (cnt == 0) buf++;
			else break;
		}
		strncpy(proposition, buf, cnt);
		proposition[cnt] = '\0';
		result->addAtomicPropositionToState(proposition, node);
		buf += cnt;
	}
	
	munmap(data, st.st_size);
	close(f);	
	return result;
}

} //namespace parser

} //namespace mrmc
