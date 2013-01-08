#ifndef STORM_PARSER_NONDETTRAPARSER_H_
#define STORM_PARSER_NONDETTRAPARSER_H_

#include "src/storage/SparseMatrix.h"

#include "src/parser/Parser.h"
#include "src/utility/OsDetection.h"

#include <memory>
#include <vector>

namespace storm {
namespace parser {
	
/*!
 *	@brief	Load a nondeterministic transition system from file and create a
 *	sparse adjacency matrix whose entries represent the weights of the edges
 */
class NonDeterministicSparseTransitionParser : public Parser {
	public:
		NonDeterministicSparseTransitionParser(std::string const &filename);
		
		std::shared_ptr<storm::storage::SparseMatrix<double>> getMatrix() {
			return this->matrix;
		}
	
	private:
		std::shared_ptr<storm::storage::SparseMatrix<double>> matrix;
		
		std::unique_ptr<std::vector<uint_fast64_t>> firstPass(char* buf, uint_fast64_t &maxnode, uint_fast64_t &maxchoice);
	
};
		
} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_NONDETTRAPARSER_H_ */
