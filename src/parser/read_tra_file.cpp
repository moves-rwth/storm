/*! read_tra_file.cpp
 * Provides functions for reading a *.tra file describing the transition
 * system of a Markov chain (DTMC) and saving it into a corresponding
 * matrix.
 *
 * Reuses code from the file "read_tra_file.c" from the old MRMC project.
 *
 * Created on: 20.11.2012
 *      Author: Gereon Kremer
 */

#include "parser.h"

#include "src/parser/read_tra_file.h"
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

// Disable C4996 - This function or variable may be unsafe.
#pragma warning(disable:4996)

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
static uint_fast32_t make_first_pass(char* buf)
{
	uint_fast32_t non_zero = 0;
	
	/*
		check file header and extract number of transitions
	*/
	if (strncmp(buf, "STATES ", 7) != 0) return 0;
	buf += 7; // skip "STATES "
	if (strtol(buf, &buf, 10) == 0) return 0;
	buf = skipWS(buf);
	if (strncmp(buf, "TRANSITIONS ", 12) != 0) return 0;
	buf += 12; // skip "TRANSITIONS "
	if ((non_zero = strtol(buf, &buf, 10)) == 0) return 0;
	
	/*
		check all transitions for non-zero diagonal entrys
	*/
	unsigned int row, col;
	double val;
	while (1)  
	{
		row = strtol(buf, &buf, 10);
		col = strtol(buf, &buf, 10);
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

sparse::StaticSparseMatrix<double> * read_tra_file(const char * filename) {
	if (setlocale( LC_NUMERIC, "de_DE" ) == 0)
	{
		fprintf(stderr, "could not set locale\n");
	}
	/*
		open file and map to memory
	*/
	struct stat st;
	int f = open(filename, O_RDONLY);
	if((f < 0) || (stat(filename, &st) != 0)) {
		pantheios::log_ERROR("File ", filename, " was not readable (Does it exist?)");
		throw exceptions::file_IO_exception("mrmc::read_tra_file: Error opening file! (Does it exist?)");
		return NULL;
	}
	char *data = (char*)mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, f, 0);
	if (data == (char*)-1)
	{
		pantheios::log_ERROR("Could not map the file to memory. Something went wrong with mmap.");
		throw exceptions::file_IO_exception("mrmc::read_tra_file: Error mapping file to memory");
		close(f);
		return NULL;
	}
	
	/*
		perform first pass
	*/
	uint_fast32_t non_zero = make_first_pass(data);
	if (non_zero == 0)
	{
		close(f);
		munmap(data, st.st_size);
		throw mrmc::exceptions::wrong_file_format();
		return NULL;
	}
	
	/*
		perform second pass
		
		from here on, we already know that the file header is correct
	*/
	char* buf = data;
	uint_fast32_t rows;
	sparse::StaticSparseMatrix<double> *sp = NULL;

	/*
		read file header, extract number of states
	*/
	buf += 7; // skip "STATES "
	rows = strtol(buf, &buf, 10);
	buf = skipWS(buf);
	buf += 12; // skip "TRANSITIONS "
	strtol(buf, &buf, 10);
	
	pantheios::log_DEBUG("Creating matrix with ",
                        pantheios::integer(rows), " rows and ",
                        pantheios::integer(non_zero), " Non-Zero-Elements");
                        
	/* Creating matrix
	 * Memory for diagonal elements is automatically allocated, hence only the number of non-diagonal
	 * non-zero elements has to be specified (which is non_zero, computed by make_first_pass)
	 */
	sp = new sparse::StaticSparseMatrix<double>(rows);
	if (sp == NULL)
	{
		close(f);
		munmap(data, st.st_size);
		throw std::bad_alloc();
		return NULL;
	}
	sp->initialize(non_zero);

	uint_fast64_t row, col;
	double val;
	/*
		read all transitions from file
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
	
	// clean up
	close(f);
	munmap(data, st.st_size);

	pantheios::log_DEBUG("Finalizing Matrix");
	sp->finalize();
	return sp;
}

} //namespace parser

} //namespace mrmc
