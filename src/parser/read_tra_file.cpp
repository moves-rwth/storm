/*! read_tra_file.cpp
 * Provides functions for reading a *.tra file describing the transition
 * system of a Markov chain (DTMC) and saving it into a corresponding
 * matrix.
 *
 * Reuses code from the file "read_tra_file.c" from the old MRMC project.
 *
 *  Created on: 15.08.2012
 *      Author: Thomas Heinemann
 */

#include "parser.h"

#include "src/parser/read_tra_file.h"
#include "src/exceptions/file_IO_exception.h"
#include "src/exceptions/wrong_file_format.h"
#include "boost/integer/integer_mask.hpp"
#include <cstdlib>
#include <cstdio>

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>
#include <pantheios/inserters/real.hpp>

namespace mrmc {

namespace parser{

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
* @param p File stream to scan. Is expected to be opened, a NULL pointer will
*          be rejected!
*/
static uint_fast32_t make_first_pass(FILE* p) {
   if(p==NULL) {
      pantheios::log_ERROR("make_first_pass was called with NULL! (SHOULD NEVER HAPPEN)");
      throw exceptions::file_IO_exception ("make_first_pass: File not readable (this should be checked before calling this function!)");
   }
   char s[BUFFER_SIZE];                 //String buffer
   uint_fast32_t rows=0, non_zero=0;

   //Reading No. of states
   if (fgets(s, BUFFER_SIZE, p) != NULL) {
      if (sscanf( s, "STATES %d", &rows) == 0) {
         pantheios::log_WARNING(pantheios::integer(rows));
         (void)fclose(p);
         throw mrmc::exceptions::wrong_file_format();
      }
   }

   //Reading No. of transitions
   if (fgets(s, BUFFER_SIZE, p) != NULL) {
      if (sscanf( s, "TRANSITIONS %d", &non_zero) == 0) {
         (void)fclose(p);
         throw mrmc::exceptions::wrong_file_format();
      }
   }

   //Reading transitions (one per line)
   //And increase number of transitions
   while (NULL != fgets( s, BUFFER_SIZE, p ))
   {
      uint_fast32_t row=0, col=0;
      double val=0.0;
      if (sscanf( s, "%d%d%lf", &row, &col, &val ) != 3) {
         (void)fclose(p);
         throw mrmc::exceptions::wrong_file_format();
      }
      //Diagonal elements are not counted into the result!
      if(row == col) {
         --non_zero;
      }
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
	FILE *p = NULL;
	char s[BUFFER_SIZE];
	uint_fast32_t rows, non_zero;
	sparse::StaticSparseMatrix<double> *sp = NULL;

	p = fopen(filename, "r");
	if(p == NULL) {
		pantheios::log_ERROR("File ", filename, " was not readable (Does it exist?)");
		throw exceptions::file_IO_exception("mrmc::read_tra_file: Error opening file! (Does it exist?)");
		return NULL;
	}
	non_zero = make_first_pass(p);

	//Set file reader back to the beginning
	rewind(p);

	//Reading No. of states
	if ((fgets(s, BUFFER_SIZE, p) == NULL) || (sscanf(s, "STATES %d", &rows) == 0)) {
		pantheios::log_WARNING(pantheios::integer(rows));
		(void)fclose(p);
		throw mrmc::exceptions::wrong_file_format();
		return NULL;
	}

	/* Reading No. of transitions
	 * Note that the result is not used in this function as make_first_pass()
	 * computes the relevant number (non_zero)
	 */
	uint_fast32_t nnz=0;
	if ((fgets(s, BUFFER_SIZE, p) == NULL) || (sscanf(s, "TRANSITIONS %d", &nnz) == 0)) {
		(void)fclose(p);
		throw mrmc::exceptions::wrong_file_format();
		return NULL;
	}

   pantheios::log_DEBUG("Creating matrix with ",
                        pantheios::integer(rows), " rows and ",
                        pantheios::integer(non_zero), " Non-Zero-Elements");
	/* Creating matrix
	 * Memory for diagonal elements is automatically allocated, hence only the number of non-diagonal
	 * non-zero elements has to be specified (which is non_zero, computed by make_first_pass)
	 */
	sp = new sparse::StaticSparseMatrix<double>(rows);
	if ( NULL == sp ) {
		throw std::bad_alloc();
		return NULL;
	}
	sp->initialize(non_zero);

	//Reading transitions (one per line) and saving the results in the matrix
	while (NULL != fgets(s, BUFFER_SIZE, p )) {
		uint_fast32_t row=0, col=0;
		double val = 0.0;
		if (sscanf(s, "%d%d%lf", &row, &col, &val) != 3) {
			(void)fclose(p);
			throw mrmc::exceptions::wrong_file_format();
			// Delete Matrix to free allocated memory
			delete sp;
			return NULL;
		}
		pantheios::log_DEBUG("Write value ",
							pantheios::real(val),
							" to position ",
							pantheios::integer(row), " x ",
							pantheios::integer(col));
		sp->addNextValue(row,col,val);
	}

	(void)fclose(p);

	pantheios::log_DEBUG("Finalizing Matrix");
	sp->finalize();
	return sp;
}

} //namespace parser

} //namespace mrmc
