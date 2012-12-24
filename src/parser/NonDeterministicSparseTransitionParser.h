#ifndef MRMC_PARSER_NONDETTRAPARSER_H_
#define MRMC_PARSER_NONDETTRAPARSER_H_

#include "src/storage/SquareSparseMatrix.h"

#include "src/parser/Parser.h"
#include "src/utility/OsDetection.h"

#include <memory>
#include <vector>

namespace mrmc {
namespace parser {
	
/*!
 *	@brief	Load a nondeterministic transition system from file and create a
 *	sparse adjacency matrix whose entries represent the weights of the edges
 */
class NonDeterministicSparseTransitionParser : public Parser {
	public:
		NonDeterministicSparseTransitionParser(std::string const &filename);
		
		std::shared_ptr<mrmc::storage::SquareSparseMatrix<double>> getMatrix() {
			return this->matrix;
		}
	
	private:
		std::shared_ptr<mrmc::storage::SquareSparseMatrix<double>> matrix;
		
		std::unique_ptr<std::vector<uint_fast64_t>> firstPass(char* buf, uint_fast64_t &maxnode, uint_fast64_t &maxchoice);
	
};
		
} // namespace parser
} // namespace mrmc

#endif /* MRMC_PARSER_NONDETTRAPARSER_H_ */
