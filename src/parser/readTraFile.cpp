/*!
 *	readTraFile.cpp
 *
 *	Created on: 20.11.2012
 *		Author: Gereon Kremer
 */

#include "parser.h"

#include "src/parser/readTraFile.h"
#include "src/exceptions/file_IO_exception.h"
#include "src/exceptions/wrong_file_format.h"
#include "boost/integer/integer_mask.hpp"
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
#endif
#include <fcntl.h>
#include <locale.h>

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace mrmc {
namespace parser{

/*!
 *	@brief	Perform first pass through the file and obtain number of
 *	non-zero cells and maximum node id.
 *
 *	This method does the first pass through the .tra file and computes
 *	the number of non-zero elements that are not diagonal elements,
 *	which correspondents to the number of transitions that are not
 *	self-loops.
 *	(Diagonal elements are treated in a special way).
 *	It also calculates the maximum node id and stores it in maxnode.
 *
 *	@return The number of non-zero elements that are not on the diagonal
 *	@param buf Data to scan. Is expected to be some char array.
 *	@param maxnode Is set to highest id of all nodes.
 */
uint_fast64_t TraParser::firstPass(char* buf, uint_fast64_t &maxnode) {
	uint_fast64_t non_zero = 0;
	
	/*
	 *	check file header and extract number of transitions
	 */
	if (strncmp(buf, "STATES ", 7) != 0) {
		LOG4CPLUS_ERROR(logger, "Expected \"STATES\" but got \"" << std::string(buf, 0, 16) << "\".");
		return 0;
	}
	buf += 7; // skip "STATES "
	if (strtol(buf, &buf, 10) == 0) return 0;
	buf = skipWS(buf);
	if (strncmp(buf, "TRANSITIONS ", 12) != 0) {
		LOG4CPLUS_ERROR(logger, "Expected \"TRANSITIONS\" but got \"" << std::string(buf, 0, 16) << "\".");
		return 0;
	}
	buf += 12; // skip "TRANSITIONS "
	if ((non_zero = strtol(buf, &buf, 10)) == 0) return 0;
	
	/*
	 *	check all transitions for non-zero diagonal entrys
	 */
	uint_fast64_t row, col;
	double val;
	maxnode = 0;
	char* tmp;
	while (buf[0] != '\0') {
		/*
		 *	read row and column
		 */
		row = checked_strtol(buf, &buf);
		col = checked_strtol(buf, &buf);
		/*
		 *	check if one is larger than the current maximum id
		 */
		if (row > maxnode) maxnode = row;
		if (col > maxnode) maxnode = col;
		/*
		 *	read value. if value is 0.0, either strtod could not read a number or we encountered a probability of zero.
		 *	if row == col, we have a diagonal element which is treated seperately and this non_zero must be decreased.
		 */
		val = strtod(buf, &tmp);
		if (val == 0.0) {
			LOG4CPLUS_ERROR(logger, "Expected a positive probability but got \"" << std::string(buf, 0, 16) << "\".");
			return 0;
		}
		if (row == col) non_zero--;
		buf = skipWS(tmp);
	}

	return non_zero;
}



/*!
 *	Reads a .tra file and produces a sparse matrix representing the described Markov Chain.
 *
 *	Matrices created with this method have to be freed with the delete operator.
 *	@param filename input .tra file's name.
 *	@return a pointer to the created sparse matrix.
 */

TraParser::TraParser(const char * filename)
	: matrix(nullptr)
{
	/*
	*	enforce locale where decimal point is '.'
	*/
	setlocale( LC_NUMERIC, "C" );
	
	/*
	 *	open file
	 */
	MappedFile file(filename);
	char* buf = file.data;
	
	/*
	 *	perform first pass, i.e. count entries that are not zero and not on the diagonal
	 */
	uint_fast64_t maxnode;
	uint_fast64_t non_zero = this->firstPass(file.data, maxnode);
	/*
	 *	if first pass returned zero, the file format was wrong
	 */
	if (non_zero == 0)
	{
		LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": erroneous file format.");
		throw mrmc::exceptions::wrong_file_format();
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
	buf = skipWS(buf);
	buf += 12; // skip "TRANSITIONS "
	checked_strtol(buf, &buf);
	
	/*
	 *	Creating matrix
	 *	Memory for diagonal elements is automatically allocated, hence only the number of non-diagonal
	 *	non-zero elements has to be specified (which is non_zero, computed by make_first_pass)
	 */
	LOG4CPLUS_INFO(logger, "Attempting to create matrix of size " << (maxnode+1) << " x " << (maxnode+1) << ".");
	this->matrix = std::shared_ptr<mrmc::storage::SquareSparseMatrix<double>>(new mrmc::storage::SquareSparseMatrix<double>(maxnode + 1));
	if (this->matrix == NULL)
	{
		LOG4CPLUS_ERROR(logger, "Could not create matrix of size " << (maxnode+1) << " x " << (maxnode+1) << ".");
		throw std::bad_alloc();
	}
	this->matrix->initialize(non_zero);

	uint_fast64_t row, col;
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
		col = checked_strtol(buf, &buf);
		val = strtod(buf, &buf);
		
		/*
		 *	only values in (0, 1] are meaningful
		 */
		if ((val <= 0.0) || (val > 1.0))
		{
			LOG4CPLUS_ERROR(logger, "Found transition probability of " << val << ", but we think probabilities should be from (0,1].");
			throw mrmc::exceptions::wrong_file_format();
		}
		this->matrix->addNextValue(row,col,val);
		buf = skipWS(buf);
	}
	
	/*
	 * clean up
	 */	
	this->matrix->finalize();
}

} //namespace parser
} //namespace mrmc
