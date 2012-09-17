/*
 * read_lab_file.cpp
 *
 *  Created on: 10.09.2012
 *      Author: Thomas Heinemann
 */

#include "parser.h"

#include "read_lab_file.h"

#include "src/exceptions/wrong_file_format.h"
#include "src/exceptions/file_IO_exception.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>

#ifdef WIN32
#	define STRTOK_FUNC strtok_s
#else
#	define STRTOK_FUNC strtok_r
#endif


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
mrmc::dtmc::labeling * read_lab_file(int node_count, const char * filename)
{
   /* Note that this function uses strtok_r on char-array s.
    * This function will modify this string.
    * However, as only the result of its tokenization is used, this is not a problem.
    *
    * Thread-safety is ensured by using strtok_r instead of strtok.
    */
   FILE *P;

   //TODO (Thomas Heinemann): handle files with lines that are longer than BUFFER_SIZE
   //TODO (Thomas Heinemann): Throw errors if fgets return NULL in the declaration.

   char s[BUFFER_SIZE];              //String buffer
   char *saveptr = NULL;             //Buffer for strtok_r
   char sep[] = " \n\t";             //Separators for the tokens

   P = fopen(filename, "r");

   if (P == NULL) {
      pantheios::log_ERROR("File could not be opened.");
      throw mrmc::exceptions::file_IO_exception();
   }

   if (fgets(s, BUFFER_SIZE, P)) {
      if (strcmp(s, "#DECLARATION\n")) {
         fclose(P);
         pantheios::log_ERROR("Wrong declaration section (\"#DECLARATION\" missing).");
         throw mrmc::exceptions::wrong_file_format();
      }
   }



   uint_fast32_t buffer_size_count = 1;
   uint_fast32_t buffer_start = 0;
   char* decl_buffer = new char[buffer_size_count*BUFFER_SIZE];
   bool need_next_iteration = true;

   do {
      decl_buffer[buffer_size_count*BUFFER_SIZE - 1] = '\xff';
      if (fgets(decl_buffer + buffer_start, buffer_size_count*BUFFER_SIZE - buffer_start, P)) {
         if ((decl_buffer[buffer_size_count*BUFFER_SIZE - 1] != '\xff') &&
             (decl_buffer[buffer_size_count*BUFFER_SIZE - 2] != '\n')) {
            //fgets changed the last bit -> read string has maximum length
            //AND line is longer than size of decl_buffer
            char* temp_buffer = decl_buffer;
            decl_buffer = NULL;

            buffer_size_count++;
            decl_buffer = new char[buffer_size_count*BUFFER_SIZE];

            buffer_start = (buffer_size_count - 1) * BUFFER_SIZE;

            memcpy(decl_buffer, temp_buffer, buffer_start - 1);
            delete[] temp_buffer;
         } else {
            need_next_iteration = false;
         }
      } else {
         pantheios::log_ERROR("EOF in the declaration section");
         throw mrmc::exceptions::wrong_file_format();
      }
   } while (need_next_iteration);

   uint_fast32_t proposition_count = 0;
   char * proposition;
   int pos = 0;

   if (decl_buffer[pos] != ' ' && decl_buffer[pos] != '\t' && decl_buffer[pos] != '\0') {
      proposition_count++;
   }

   while (decl_buffer[pos] != '\0') {
      if ((decl_buffer[pos] == ' ' || decl_buffer[pos] == '\t') &&
          (decl_buffer[pos + 1] != ' ' && decl_buffer[pos + 1] != '\t' && decl_buffer[pos + 1] != '\0')) {
         proposition_count++;
      }
      pos++;
   }

   mrmc::dtmc::labeling* result = new mrmc::dtmc::labeling(node_count, proposition_count);

   //Here, all propositions are to be declared...
   for (proposition = STRTOK_FUNC(decl_buffer, sep, &saveptr);
        proposition != NULL;
        proposition = STRTOK_FUNC(NULL, sep, &saveptr)) {
      result -> addProposition(proposition);
   }



   saveptr = NULL;                        //resetting save pointer for strtok_r

   if (fgets(s, BUFFER_SIZE, P)) {
      if (strcmp(s, "#END\n")) {
         fclose(P);
         delete result;
         pantheios::log_ERROR("Wrong declaration section (\"#END\" missing).");
         throw mrmc::exceptions::wrong_file_format();
      }
   }

   while (fgets(s, BUFFER_SIZE, P)) {
      char * token = NULL;
      uint_fast32_t node = 0;
      /* First token has to be a number identifying the node,
       * hence it is treated differently from the other tokens.
       */
      token = STRTOK_FUNC(s, sep, &saveptr);
      if (sscanf(token, "%u", &node) == 0) {
         fclose(P);
         delete result;
         pantheios::log_ERROR("Line assigning propositions does not start with a node number.");
         throw mrmc::exceptions::wrong_file_format();
      }
      do {
         token = STRTOK_FUNC(NULL, sep, &saveptr);
         if (token == NULL) {
            break;
         }
         result->addLabelToNode(token, node);
      } while (token != NULL);

   }

   fclose(P);

   return result;

}

} //namespace parser

} //namespace mrmc
