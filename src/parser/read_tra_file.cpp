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

/*!
* This method does the first pass through the .tra file and computes
* the number of non zero elements that are not diagonal elements,
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

   char  s[1024], states[7], transitions[12];
   int rows=0, row=0, col=0, non_zero=0;
   double val=0.0;

   //Reading No. of states
   if (fgets(s, 1024, p) != NULL) {
      if (sscanf( s, "STATES %d", &rows) == 0) {
         pantheios::log_WARNING(pantheios::integer(rows));
         throw mrmc::exceptions::wrong_file_format();
      }
   }

   //Reading No. of transitions
   if (fgets(s, 1024, p) != NULL) {
      if (sscanf( s, "TRANSITIONS %d", &non_zero) == 0) {
         throw mrmc::exceptions::wrong_file_format();
      }
   }

   //Reading transitions (one per line)
   //And increase number of transitions
   while (NULL != fgets( s, 1024, p ))
   {
      if (sscanf( s, "%d%d%lf", &row, &col, &val ) != 3) {
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
 * @param *filename input .tra file's name.
 * @return a pointer to a sparse matrix.
 */

sparse::StaticSparseMatrix<double> * read_tra_file(const char * filename) {
   FILE *p;
   char  s[1024];
   uint_fast32_t rows, row, col, nnz, non_zero;
   double val = 0.0;
   int count = 0;
   sparse::StaticSparseMatrix<double> *sp = NULL;

   p = fopen(filename, "r");
   if(p==NULL) {
      pantheios::log_ERROR("File ", filename, " was not readable (Does it exist?)");
      throw exceptions::file_IO_exception("mrmc::read_tra_file: Error opening file! (Does it exist?)");
   }
   non_zero = make_first_pass(p);

   //Set file reader back to the beginning
   rewind(p);

   //Reading No. of states
   if (fgets(s, 1024, p) != NULL) {
      if (sscanf( s, "STATES %d", &rows) == 0) {
         pantheios::log_WARNING(pantheios::integer(rows));
         throw mrmc::exceptions::wrong_file_format();
      }
   }

   //Reading No. of transitions
   if (fgets(s, 1024, p) != NULL) {
      if (sscanf( s, "TRANSITIONS %d", &nnz) == 0) {
         throw mrmc::exceptions::wrong_file_format();
      }
   }

   pantheios::log_DEBUG("Creating matrix with ",
                        pantheios::integer(rows), " rows and ",
                        pantheios::integer(non_zero), " Non-Zero-Elements");
   /* Creating matrix
    * Variable non_zero does NOT count any diagonal element,
    * But all diagonal elements are allocated, so the number of allocated
    * elements is non_zero
    */
   sp = new sparse::StaticSparseMatrix<double>(rows,non_zero);
   sp->initialize();
   if ( NULL == sp ) {
      throw std::bad_alloc();
      return NULL;
      }

   //Reading transitions (one per line) and saving the results in the matrix
   while (NULL != fgets( s, 1024, p ))
   {
      if (sscanf( s, "%d%d%lf", &row, &col, &val) != 3) {
         throw mrmc::exceptions::wrong_file_format();
      }
      pantheios::log_DEBUG("Write value ",
                           pantheios::real(val),
                           " to position ",
                           pantheios::integer(row), " x ",
                           pantheios::integer(col));
      sp->addNextValue(row,col,val);
      ++count;
   }

   (void)fclose(p);

   pantheios::log_DEBUG("Written ", pantheios::integer(count), " Elements");
   pantheios::log_DEBUG("Finalizing Matrix");
   sp->finalize();
   return sp;
}

} //namespace parser

} //namespace mrmc
