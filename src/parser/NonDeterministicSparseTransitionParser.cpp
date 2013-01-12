/*!
 *	TraParser.cpp
 *
 *	Created on: 20.11.2012
 *		Author: Gereon Kremer
 */

#include "src/parser/NonDeterministicSparseTransitionParser.h"

#include "src/utility/Settings.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFileFormatException.h"
#include "boost/integer/integer_mask.hpp"

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

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
namespace parser {

/*!
 *	@brief	Perform first pass through the file and obtain overall number of
 *	choices, number of non-zero cells and maximum node id.
 *
 *	This method does the first pass through the transition file.
 *
 *	It computes the overall number of nondeterministic choices, i.e. the
 *	number of rows in the matrix that should be created.
 *	It also calculates the overall number of non-zero cells, i.e. the number
 *	of elements the matrix has to hold, and the maximum node id, i.e. the
 *	number of columns of the matrix.
 *
 *	@param buf Data to scan. Is expected to be some char array.
 *	@param choices Overall number of choices.
 *	@param maxnode Is set to highest id of all nodes.
 *	@return The number of non-zero elements.
 */
uint_fast64_t NonDeterministicSparseTransitionParser::firstPass(char* buf, uint_fast64_t& choices, uint_fast64_t& maxnode) {
	/*
	 *	Check file header and extract number of transitions.
	 */
	buf = strchr(buf, '\n') + 1;  // skip format hint
	if (strncmp(buf, "STATES ", 7) != 0) {
		LOG4CPLUS_ERROR(logger, "Expected \"STATES\" but got \"" << std::string(buf, 0, 16) << "\".");
		return 0;
	}
	buf += 7; // skip "STATES "
	if (strtol(buf, &buf, 10) == 0) return 0;
	buf = trimWhitespaces(buf);
	if (strncmp(buf, "TRANSITIONS ", 12) != 0) {
		LOG4CPLUS_ERROR(logger, "Expected \"TRANSITIONS\" but got \"" << std::string(buf, 0, 16) << "\".");
		return 0;
	}
	buf += 12; // skip "TRANSITIONS "
	/*
	 *	Parse number of transitions.
	 *	We will not actually use this value, but we will compare it to the
	 *	number of transitions we count and issue a warning if this parsed
	 *	vlaue is wrong.
	 */
	uint_fast64_t parsed_nonzero = strtol(buf, &buf, 10);
	
	/*
	 *	Read all transitions.
	 */
	uint_fast64_t source, target;
	uint_fast64_t lastsource = 0;
	uint_fast64_t nonzero = 0;
	double val;
	choices = 0;
	maxnode = 0;
	while (buf[0] != '\0') {
		/*
		 *	Read source node and choice.
		 *	Check if current source node is larger than current maximum node id.
		 *	Increase number of choices.
		 *	Check if we have skipped any source node, i.e. if any node has no
		 *	outgoing transitions. If so, increase nonzero (and
		 *	parsed_nonzero).
		 */
		source = checked_strtol(buf, &buf);
		if (source > maxnode) maxnode = source;
		choices++;
		if (source > lastsource + 1) {
			nonzero += source - lastsource - 1;
			parsed_nonzero += source - lastsource - 1;
		}
		buf += strcspn(buf, " \t\n\r");  // Skip name of choice.
		
		/*
		 *	Read all targets for this choice.
		 */
		buf = trimWhitespaces(buf);
		while (buf[0] == '*') {
			buf++;
			/*
			 *	Read target node and transition value.
			 *	Check if current target node is larger than current maximum node id.
			 *	Check if the transition value is a valid probability.
			 */
			target = checked_strtol(buf, &buf);
			if (target > maxnode) maxnode = target;
			val = checked_strtod(buf, &buf);
			if ((val < 0.0) || (val > 1.0)) {
				LOG4CPLUS_ERROR(logger, "Expected a positive probability but got \"" << std::string(buf, 0, 16) << "\".");
				return 0;
			}
			
			/*
			 *	Increase number of non-zero values.
			 */
			nonzero++;
			 
			/*
			 *	Proceed to beginning of next line.
			 */
			buf = trimWhitespaces(buf);
		}
	}

	/*
	 *	Check if the number of transitions given in the file is correct.
	 */
	if (nonzero != parsed_nonzero) {
		LOG4CPLUS_WARN(logger, "File states to have " << parsed_nonzero << " transitions, but I counted " << nonzero << ". Maybe want to fix your file?");
	}
	return nonzero;
}



/*!
 *	Reads a .tra file and produces a sparse matrix representing the described Markov Chain.
 *
 *	Matrices created with this method have to be freed with the delete operator.
 *	@param filename input .tra file's name.
 *	@return a pointer to the created sparse matrix.
 */

NonDeterministicSparseTransitionParser::NonDeterministicSparseTransitionParser(std::string const &filename)
	: matrix(nullptr)
{
	/*
	*	enforce locale where decimal point is '.'
	*/
	setlocale( LC_NUMERIC, "C" );
	
	/*
	 *	open file
	 */
	MappedFile file(filename.c_str());
	char* buf = file.data;
	
	/*
	 *	perform first pass, i.e. count entries that are not zero and not on the diagonal
	 */
	uint_fast64_t maxnode, maxchoices;
	uint_fast64_t non_zero = this->firstPass(file.data, maxnode, maxchoices);
	
	/*
	 *	if first pass returned zero, the file format was wrong
	 */
	if (non_zero == 0)
	{
		LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": erroneous file format.");
		throw storm::exceptions::WrongFileFormatException();
	}
	
	/*
	 *	perform second pass
	 *	
	 *	from here on, we already know that the file header is correct
	 */

	/*
	 *	read file header, extract number of states
	 */
	buf += 7; // skip "STATES "
	checked_strtol(buf, &buf);
	buf = trimWhitespaces(buf);
	buf += 12; // skip "TRANSITIONS "
	checked_strtol(buf, &buf);
	
	/*
	 *	Creating matrix
	 *	Memory for diagonal elements is automatically allocated, hence only the number of non-diagonal
	 *	non-zero elements has to be specified (which is non_zero, computed by make_first_pass)
	 */
	LOG4CPLUS_INFO(logger, "Attempting to create matrix of size " << (maxnode+1) << " x " << (maxnode+1) << ".");
	this->matrix = std::shared_ptr<storm::storage::SparseMatrix<double>>(new storm::storage::SparseMatrix<double>(maxnode + 1));
	if (this->matrix == NULL)
	{
		LOG4CPLUS_ERROR(logger, "Could not create matrix of size " << (maxnode+1) << " x " << (maxnode+1) << ".");
		throw std::bad_alloc();
	}
	// TODO: put stuff in matrix / matrices.
	//this->matrix->initialize(*non_zero);

	uint_fast64_t row, col, ndchoice;
	double val;

	/*
	 *	read all transitions from file
	 */
	while (buf[0] != '\0')
	{
		/*
		 *	read row, col and value.
		 */
		row = checked_strtol(buf, &buf);
		ndchoice = checked_strtol(buf, &buf);
		col = checked_strtol(buf, &buf);
		val = strtod(buf, &buf);
		
		//this->matrix->addNextValue(row,col,val);
		buf = trimWhitespaces(buf);
	}
	
	/*
	 * clean up
	 */	
	//this->matrix->finalize();
}

} //namespace parser
} //namespace storm
