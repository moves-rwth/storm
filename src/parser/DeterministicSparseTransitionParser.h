#ifndef STORM_PARSER_TRAPARSER_H_
#define STORM_PARSER_TRAPARSER_H_

#include "src/storage/SparseMatrix.h"

#include "src/parser/Parser.h"
#include "src/utility/OsDetection.h"

#include <memory>

namespace storm {
namespace parser {
	
/*!
 *	@brief	Load a deterministic transition system from file and create a
 *	sparse adjacency matrix whose entries represent the weights of the edges
 */
storm::storage::SparseMatrix<double> DeterministicSparseTransitionParser(std::string const& filename, bool insertDiagonalEntriesIfMissing = true, RewardMatrixInformationStruct* rewardMatrixInformation = nullptr);

} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_TRAPARSER_H_ */
