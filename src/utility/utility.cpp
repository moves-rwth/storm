/*
 * utility.cpp
 *
 *  Created on: 17.10.2012
 *      Author: Thomas Heinemann
 */

#include "src/utility/utility.h"
#include "src/parser/read_tra_file.h"
#include "src/parser/read_lab_file.h"

#include "src/sparse/static_sparse_matrix.h"
#include "src/models/dtmc.h"

namespace mrmc {

namespace utility {

void dtmcToDot(mrmc::models::DTMC<double>* dtmc, const char* filename) {
   FILE *P;
   mrmc::sparse::StaticSparseMatrix<double>* matrix = dtmc->getTransitions();
   mrmc::dtmc::Labeling* labels = dtmc->getLabels();

   uint_fast64_t* row_indications = matrix->getRowIndications();
   uint_fast64_t* column_indications = matrix->getColumnIndications();
   double* value_storage = matrix->getStoragePointer();
   double* diagonal_storage = matrix->getDiagonalStorage();

   P = fopen(filename, "w");

   if (P == NULL) {
      pantheios::log_ERROR("File could not be opened.");
      throw mrmc::exceptions::file_IO_exception();
   }

   fprintf(P, "digraph dtmc {\n");

   //Specify the nodes and their labels
   for (uint_fast64_t i = 1; i <= dtmc->getNodeCount(); i++) {
      fprintf(P, "\t%Lu[label=\"%Lu\\n{", i, i);
      char komma=' ';
      for(auto it = labels->getPropositionMap()->begin();
               it != labels->getPropositionMap()->end();
               it++) {
         if(labels->nodeHasProposition(it->first, i)) {
            fprintf(P, "%c%s", komma, (it->first).c_str());
         }
         char komma=',';
      }

      fprintf(P, " }\"];\n");
   }

   uint_fast64_t row = 0;

   for (uint_fast64_t i = 0; i < matrix->getNonZeroEntryCount(); i++ ) {
      //Check whether we have to switch to the new row
      while (row_indications[row] <= i) {
         ++row;
         //write diagonal entry/self loop first
         if (diagonal_storage[row] != 0) {
            fprintf(P, "\t%Lu -> %Lu [label=%f]\n",
                  row, row, diagonal_storage[row]);
         }
      }
      fprintf(P, "\t%Lu -> %Lu [label=%f]\n",
            row, column_indications[i], value_storage[i]);
   }



   fprintf(P, "}\n");

   fclose(P);
}

mrmc::models::Dtmc<double>* parseDTMC(const char* tra_file, const char* lab_file) {
   mrmc::sparse::StaticSparseMatrix<double>* transition_matrix =
         mrmc::parser::read_tra_file(tra_file);
   uint_fast64_t node_count = transition_matrix->getRowCount();

   mrmc::dtmc::Labeling* labeling =
         mrmc::parser::read_lab_file(node_count, lab_file);

   mrmc::models::Dtmc<double>* result =
         new mrmc::models::Dtmc<double>(transition_matrix, labeling);
   return result;
}

}

}
