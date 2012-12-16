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
#include <fcntl.h>
#include <locale.h>

#if defined LINUX || defined MACOSX
	#include <sys/mman.h>
#elif defined WINDOWS
	#define strncpy strncpy_s
#endif

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

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
LabParser::LabParser(uint_fast64_t node_count, const char * filename)
	: labeling(nullptr)
{
	/*
	 *	open file
	 */
	MappedFile file(filename);
	char* buf = file.data;

	/*
	 *	first run: obtain number of propositions
	 */
#ifdef WINDOWS
	char separator[] = " \r\n\t";
#else
	char separator[] = " \n\t";
#endif
	bool foundDecl = false, foundEnd = false;
	uint_fast32_t proposition_count = 0;
	{
		size_t cnt = 0;
		/*
		 *	iterate over tokens until we hit #END or end of file
		 */
		while(buf[0] != '\0') {
			buf += cnt;
			cnt = strcspn(buf, separator); // position of next separator
			if (cnt > 0) {
				/*
				 *	next token is #DECLARATION: just skip it
				 *	next token is #END: stop search
				 *	otherwise increase proposition_count
				 */
				if (strncmp(buf, "#DECLARATION", cnt) == 0) {
					foundDecl = true;
					continue;
				}
				else if (strncmp(buf, "#END", cnt) == 0) {
					foundEnd = true;
					break;
				}
				proposition_count++;
			} else buf++; // next char is separator, one step forward
		}
		
		/*
		 *	If #DECLARATION or #END were not found, the file format is wrong
		 */
		if (! (foundDecl && foundEnd)) {
			LOG4CPLUS_ERROR(logger, "Wrong file format in (" << filename << "). File header is corrupted.");
			if (! foundDecl) LOG4CPLUS_ERROR(logger, "\tDid not find #DECLARATION token.");
			if (! foundEnd) LOG4CPLUS_ERROR(logger, "\tDid not find #END token.");
			throw mrmc::exceptions::wrong_file_format();
		}
	}
	
	/*
	 *	create labeling object with given node and proposition count
	 */
	this->labeling = std::shared_ptr<mrmc::models::AtomicPropositionsLabeling>(new mrmc::models::AtomicPropositionsLabeling(node_count, proposition_count));
	
	/*
	 *	second run: add propositions and node labels to labeling
	 *
	 *	first thing to do: reset file pointer
	 */	
	buf = file.data;
	{
		/*
		 *	load propositions
		 *	As we already checked the file format, we can be a bit sloppy here...
		 */
		char proposition[128]; // buffer for proposition names
		size_t cnt = 0;
		do {
			buf += cnt;
			cnt = strcspn(buf, separator); // position of next separator
			if (cnt >= sizeof(proposition)) {
				/*
				 *	if token is longer than our buffer, the following strncpy code might get risky...
				 */
				LOG4CPLUS_ERROR(logger, "Wrong file format in (" << filename << "). Atomic proposition with length > " << (sizeof(proposition)-1) << " was found.");
				throw mrmc::exceptions::wrong_file_format();
			} else if (cnt > 0) {
				/*
				 *	next token is: #DECLARATION: just skip it
				 *	next token is: #END: stop search
				 *	otherwise: copy token to buffer, append trailing null byte and hand it to labeling
				 */
				if (strncmp(buf, "#DECLARATION", cnt) == 0) continue;
				if (strncmp(buf, "#END", cnt) == 0) break;
				strncpy(proposition, buf, cnt);
				proposition[cnt] = '\0';
				this->labeling->addAtomicProposition(proposition);
			} else cnt = 1; // next char is separator, one step forward
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
		uint_fast64_t node;
		char proposition[128];
		size_t cnt;
		/*
		 *	iterate over nodes
		 */
		while (buf[0] != '\0') {
			/*
			 *	parse node number, then iterate over propositions
			 */
			node = checked_strtol(buf, &buf);
			while ((buf[0] != '\n') && (buf[0] != '\0')) {
				cnt = strcspn(buf, separator);
				if (cnt == 0) {
					/*
					 *	next char is a separator
					 *	if it's a newline, we continue with next node
					 *	otherwise we skip it and try again
					 */
					if (buf[0] == '\n') break;
					buf++;
				} else {
					/*
					 *	copy proposition to buffer and add it to labeling
					 */
					strncpy(proposition, buf, cnt);
					proposition[cnt] = '\0';
					this->labeling->addAtomicPropositionToState(proposition, node);
					buf += cnt;
				}
			}
			buf = skipWS(buf);
		}
	}
}

} //namespace parser
} //namespace mrmc
