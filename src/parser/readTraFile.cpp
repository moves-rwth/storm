/*! read_tra_file.cpp
 * Provides functions for reading a *.tra file describing the transition
 * system of a Markov chain (DTMC) and saving it into a corresponding
 * matrix.
 *
 * Created on: 20.11.2012
 *      Author: Gereon Kremer
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
#include <sys/mman.h>
#include <fcntl.h>
#include <locale.h>

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>
#include <pantheios/inserters/real.hpp>

namespace mrmc {

namespace parser{

char* skipWS(char* buf)
{
	while(1)
	{
		if ((buf[0] != ' ') && (buf[0] != '\t') && (buf[0] != '\n') && (buf[0] != '\r')) return buf;
		buf++;
	}
}

/*!
 * This method does the first pass through the .tra file and computes
 * the number of non-zero elements that are not diagonal elements,
 * which correspondents to the number of transitions that are not
 * self-loops.
 * (Diagonal elements are treated in a special way).
 *
 * @return The number of non-zero elements that are not on the diagonal
 * @param buf Data to scan. Is expected to be some char array.
 */
static uint_fast32_t makeFirstPass(char* buf, uint_fast32_t &maxnode)
{
	uint_fast32_t non_zero = 0;
	
	/*!
	 *	check file header and extract number of transitions
	 */
	if (strncmp(buf, "STATES ", 7) != 0) return 0;
	buf += 7; // skip "STATES "
	if (strtol(buf, &buf, 10) == 0) return 0;
	buf = skipWS(buf);
	if (strncmp(buf, "TRANSITIONS ", 12) != 0) return 0;
	buf += 12; // skip "TRANSITIONS "
	if ((non_zero = strtol(buf, &buf, 10)) == 0) return 0;
	
	/*!
	 *	check all transitions for non-zero diagonal entrys
	 */
	uint_fast32_t row, col;
	double val;
	maxnode = 0;
	while (1)  
	{
		row = strtol(buf, &buf, 10);
		if (row > maxnode) maxnode = row;
		col = strtol(buf, &buf, 10);
		if (col > maxnode) maxnode = col;
		val = strtod(buf, &buf);
		if (val == 0.0) break;
		if (row == col) non_zero--;
	}

	return non_zero;
}



/*!Reads a .tra file and produces a sparse matrix representing the described Markov Chain.
 *
 * Matrices created with this method have to be freed with the delete operator.
 * @param filename input .tra file's name.
 * @return a pointer to the created sparse matrix.
 */

sparse::StaticSparseMatrix<double> * readTraFile(const char * filename) {
	/*!
	*	enforce locale where decimal point is '.'
	*/
	setlocale( LC_NUMERIC, "C" );
	
	MappedFile file(filename);
	
	/*!
	 *	perform first pass, i.e. count entries that are not zero and not on the diagonal
	 */
	uint_fast32_t maxnode;
	uint_fast32_t non_zero = makeFirstPass(file.data, maxnode);
	if (non_zero == 0)
	{
		/*!
		 *	first pass returned zero, this means the file format was wrong
		 */
		throw mrmc::exceptions::wrong_file_format();
		return NULL;
	}
	
	/*!
	 *	perform second pass
	 *	
	 *	from here on, we already know that the file header is correct
	 */
	char* buf = file.data;
	sparse::StaticSparseMatrix<double> *sp = NULL;

	/*!
	 *	read file header, extract number of states
	 */
	buf += 7; // skip "STATES "
	strtol(buf, &buf, 10);
	buf = skipWS(buf);
	buf += 12; // skip "TRANSITIONS "
	strtol(buf, &buf, 10);
	
	pantheios::log_DEBUG("Creating matrix with ",
                        pantheios::integer(maxnode + 1), " maxnodes and ",
                        pantheios::integer(non_zero), " Non-Zero-Elements");
                        
	/*!	
	 *	Creating matrix
	 *	Memory for diagonal elements is automatically allocated, hence only the number of non-diagonal
	 *	non-zero elements has to be specified (which is non_zero, computed by make_first_pass)
	 */
	sp = new sparse::StaticSparseMatrix<double>(maxnode + 1);
	if (sp == NULL)
	{
		/*!
		 *	creating the matrix failed
		 */
		throw std::bad_alloc();
		return NULL;
	}
	sp->initialize(non_zero);

	uint_fast64_t row, col;
	double val;
	/*!
	 *	read all transitions from file
	 */
	while (1)
	{
		row = strtol(buf, &buf, 10);
		col = strtol(buf, &buf, 10);
		val = strtod(buf, &buf);
		if (val == 0.0) break;
		pantheios::log_DEBUG("Write value ",
							pantheios::real(val),
							" to position ",
							pantheios::integer(row), " x ",
							pantheios::integer(col));
		sp->addNextValue(row,col,val);	
	}
	
	/*!
	 * clean up
	 */	
	pantheios::log_DEBUG("Finalizing Matrix");
	sp->finalize();
	return sp;
}

} //namespace parser

} //namespace mrmc
