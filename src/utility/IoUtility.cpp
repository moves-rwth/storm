/*
 * IoUtility.cpp
 *
 *  Created on: 17.10.2012
 *      Author: Thomas Heinemann
 */

#include "src/utility/IoUtility.h"
#include "src/parser/DeterministicSparseTransitionParser.h"
#include "src/parser/AtomicPropositionLabelingParser.h"

#include <fstream>

namespace storm {

namespace utility {

void dtmcToDot(storm::models::Dtmc<double> const &dtmc, std::string filename) {
   std::shared_ptr<storm::storage::SquareSparseMatrix<double>> matrix(dtmc.getTransitionProbabilityMatrix());
   double* diagonal_storage = matrix->getDiagonalStoragePointer();

   std::ofstream file;
   file.open(filename);

   file << "digraph dtmc {\n";

   //Specify the nodes and their labels
   for (uint_fast64_t i = 1; i < dtmc.getNumberOfStates(); i++) {
      file << "\t" << i << "[label=\"" << i << "\\n{";
      char komma=' ';
      std::set<std::string> propositions = dtmc.getPropositionsForState(i);
      for(auto it = propositions.begin();
               it != propositions.end();
               it++) {
      	file << komma << *it;
         komma=',';
      }

      file << " }\"];\n";

   }

   for (uint_fast64_t row = 0; row < dtmc.getNumberOfStates(); row++ ) {
   	//write diagonal entry/self loop first
   	if (diagonal_storage[row] != 0) {
   	            file << "\t" << row << " -> " << row << " [label=" << diagonal_storage[row] <<"]\n";
   	         }
   	//Then, iterate through the row and write each non-diagonal value into the file
   	for (	auto it = matrix->beginConstColumnNoDiagIterator(row);
   			it !=  matrix->endConstColumnNoDiagIterator(row);
   			it++) {
   		double value = 0;
   		matrix->getValue(row,*it,&value);
   		file << "\t" << row << " -> " << *it << " [label=" << value << "]\n";
   	}
   }

   file << "}\n";
   file.close();
}

//TODO: Should this stay here or be integrated in the new parser structure?
/*storm::models::Dtmc<double>* parseDTMC(std::string const &tra_file, std::string const &lab_file) {
	storm::parser::DeterministicSparseTransitionParser tp(tra_file);
	uint_fast64_t node_count = tp.getMatrix()->getRowCount();

	storm::parser::AtomicPropositionLabelingParser lp(node_count, lab_file);

	storm::models::Dtmc<double>* result = new storm::models::Dtmc<double>(tp.getMatrix(), lp.getLabeling());
	return result;
}*/

}

}
