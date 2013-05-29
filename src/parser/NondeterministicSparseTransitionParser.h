#ifndef STORM_PARSER_NONDETTRAPARSER_H_
#define STORM_PARSER_NONDETTRAPARSER_H_

#include "src/storage/SparseMatrix.h"

#include "src/parser/Parser.h"
#include "src/utility/OsDetection.h"

#include <utility>
#include <memory>
#include <vector>

namespace storm {
namespace parser {
	
/*! 
 * @brief Contains the Result of a call to the NondeterministicSparseTransitionParser function. The first part is the resulting matrix. The second part is the row mapping.
 */
typedef std::pair<storm::storage::SparseMatrix<double>, std::vector<uint_fast64_t>> NondeterministicSparseTransitionParserResult_t;

/*!
 *	@brief	Load a nondeterministic transition system from file and create a
 *	sparse adjacency matrix whose entries represent the weights of the edges
 */
NondeterministicSparseTransitionParserResult_t NondeterministicSparseTransitionParser(std::string const &filename, RewardMatrixInformationStruct* rewardMatrixInformation = nullptr);

} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_NONDETTRAPARSER_H_ */
