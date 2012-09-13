/*
 * read_lab_file.cpp
 *
 *  Created on: 10.09.2012
 *      Author: Thomas Heinemann
 */

#include "parser.h"

#include "read_lab_file.h"

#include "src/dtmc/labelling.h"

#include "src/exceptions/wrong_file_format.h"
#include "src/exceptions/file_IO_exception.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>


namespace mrmc {

namespace parser {


/*!
 * Reads a .lab file and puts the result in a labelling structure.
 * @param node_count the number of states.
 * @param filename   input .lab file's name.
 * @return returns a pointer to a labelling object.
 */
mrmc::dtmc::labelling * read_lab_file(int node_count, const char * filename)
{
   /* Note that this function uses strtok_r on char-array s.
    * This function will modify this string.
    * However, as only the result of its tokenization is used, this is not a problem.
    *
    * Thread-safety is ensured by using strtok_r instead of strtok.
    */
   FILE *P;
   char s[BUFFER_SIZE];              //String buffer
   char *saveptr = NULL;             //Buffer for strtok_r
   char sep[] = " \n\t";             //Separators for the tokens

   P = fopen(filename, "r");

   if (P == NULL) {
      throw mrmc::exceptions::file_IO_exception();
   }

   if (fgets(s, BUFFER_SIZE, P)) {
      if (strcmp(s, "#DECLARATION\n")) {
         fclose(P);
         throw mrmc::exceptions::wrong_file_format();
      }
   }


   mrmc::dtmc::labelling* result = new mrmc::dtmc::labelling(node_count);

   //Here, all propositions are to be declared...
   if (fgets(s, BUFFER_SIZE, P)) {
      char * proposition;
      for (proposition = strtok_r(s, sep, &saveptr);
           proposition != NULL;
           proposition = strtok_r(NULL, sep, &saveptr)) {
         result -> addProposition(proposition);
      }
   } else {
      fclose(P);
      delete result;
      throw mrmc::exceptions::wrong_file_format();
   }

   saveptr = NULL;                        //resetting save pointer for strtok_r

   if (fgets(s, BUFFER_SIZE, P)) {
      if (strcmp(s, "#END\n")) {
         fclose(P);
         delete result;
         throw mrmc::exceptions::wrong_file_format();
      }
   }

   while (fgets(s, BUFFER_SIZE, P)) {
      char * token = NULL;
      uint_fast32_t node = 0;
      /* First token has to be a number identifying the node,
       * hence it is treated differently from the other tokens.
       */
      token = strtok_r(s, sep, &saveptr);
      if (sscanf(token, "%u", &node) == 0) {
         fclose(P);
         delete result;
         throw mrmc::exceptions::wrong_file_format();
      }
      do {
         token = strtok_r(NULL, sep, &saveptr);
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
