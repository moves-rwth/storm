#ifndef STORM_PARSER_TRAPARSER_H_
#define STORM_PARSER_TRAPARSER_H_

#include "src/storage/SquareSparseMatrix.h"

#include "src/parser/Parser.h"
#include "src/utility/OsDetection.h"

#include <memory>

namespace storm {
namespace parser {
	
/*!
 *	@brief	Load a deterministic transition system from file and create a
 *	sparse adjacency matrix whose entries represent the weights of the edges
 */
class DeterministicSparseTransitionParser : public Parser {
	public:
		DeterministicSparseTransitionParser(std::string const &filename);
		
		std::shared_ptr<storm::storage::SquareSparseMatrix<double>> getMatrix() {
			return this->matrix;
		}
	
	private:
		std::shared_ptr<storm::storage::SquareSparseMatrix<double>> matrix;
		
		uint_fast64_t firstPass(char* buf, uint_fast64_t &maxnode);
	
};
		
} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_TRAPARSER_H_ */
