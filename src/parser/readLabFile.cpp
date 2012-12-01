/*!
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
#if defined LINUX || defined MACOSX
	#include <sys/mman.h>
#elif defined WINDOWS
#define strncpy strncpy_s
#endif
#include <fcntl.h>
#include <locale.h>

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>

namespace mrmc {
namespace parser {


/*!
 *	Reads a label file and puts the result in a labeling structure.
 *
 *	Labelings created with this method have to be freed with the delete operator.
 *	@param node_count the number of states.
 *	@param filename   input .lab file's name.
 *	@return The pointer to the created labeling object.
 */
mrmc::models::AtomicPropositionsLabeling * readLabFile(int node_count, const char * filename)
{
	/*
	 *	open file
	 */
	MappedFile file(filename);
	char* buf = file.data;

	/*
	 *	first run: obtain number of propositions
	 */
	char separator[] = " \n\t";
	uint_fast32_t proposition_count = 0;
	{
		size_t cnt = 0;
		do
		{
			buf += cnt;
			cnt = strcspn(buf, separator); // position of next separator
			if (cnt > 0)
			{
				/*
				 *	next token is #DECLARATION: just skip it
				 *	next token is #END: stop search
				 *	otherwise increase proposition_count
				 */
				if (strncmp(buf, "#DECLARATION", cnt) == 0) continue;
				if (strncmp(buf, "#END", cnt) == 0) break;
				proposition_count++;
			}
			else cnt = 1; // next char is separator, one step forward
		} while (cnt > 0);
	}
	
	/*
	 *	create labeling object with given node and proposition count
	 */
	mrmc::models::AtomicPropositionsLabeling* result = new mrmc::models::AtomicPropositionsLabeling(node_count, proposition_count);
	
	/*
	 *	second run: add propositions and node labels to labeling
	 *
	 *	first thing to do: reset file pointer
	 */	
	buf = file.data;
	{
		/*
		 *	load propositions
		 */
		char proposition[128]; // buffer for proposition names
		size_t cnt = 0;
		do
		{
			buf += cnt;
			cnt = strcspn(buf, separator); // position of next separator
			if (cnt > 0)
			{
				/*
				 *	next token is: #DECLARATION: just skip it
				 *	next token is: #END: stop search
				 *	otherwise: copy token to buffer, append trailing null byte and hand it to labeling
				 */
				if (strncmp(buf, "#DECLARATION", cnt) == 0) continue;
				if (strncmp(buf, "#END", cnt) == 0) break;
				strncpy(proposition, buf, cnt);
				proposition[cnt] = '\0';
				result->addAtomicProposition(proposition);
			}
			else cnt = 1; // next char is separator, one step forward
		} while (cnt > 0);
		/*
		 *	Right here, the buf pointer is still pointing to our last token,
		 *	i.e. to #END. We want to skip this one...
		 */
		buf += 4;
	}
	
	{
		/*
		 *	now parse node label assignments
		 */
		uint_fast32_t node;
		char proposition[128];
		size_t cnt;
		do
		{
			node = strtol(buf, &buf, 10); // parse node number
			while (*buf != '\0') // while not at end of file
			{
				cnt = strcspn(buf, separator); // position of next separator
				if (cnt == 0) buf++; // next char is separator, one step forward
				else break;
			}
			/*
			 *	if cnt > 0, buf points to the next proposition
			 *	otherwise, we have reached the end of the file
			 */
			if (cnt > 0)
			{
				/*
				 *	copy proposition to buffer, add assignment to labeling
				 */
				strncpy(proposition, buf, cnt);
				proposition[cnt] = '\0';
				result->addAtomicPropositionToState(proposition, node);
			}
			else if (node == 0)
			{
				/*
				 *	If this is executed, we could read a number but there
				 *	was no proposition after that. This strongly suggests a
				 *	broken file, but it's not fatal...
				 */
				 pantheios::log_WARNING("The label file ended on a node number. Is the file malformated?");
			}
			buf += cnt;
		} while (cnt > 0);
	}
	
	return result;
}

} //namespace parser
} //namespace mrmc
