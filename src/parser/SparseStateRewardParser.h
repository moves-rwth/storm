#ifndef STORM_PARSER_SPARSESTATEREWARDPARSER_H_
#define STORM_PARSER_SPARSESTATEREWARDPARSER_H_

#include <cstdint>
#include "src/parser/Parser.h"
#include <memory>
#include <vector>

namespace storm {

namespace parser {

/*!
 *	@brief Load state reward file and return vector of state rewards.
 */
std::vector<double> SparseStateRewardParser(uint_fast64_t stateCount, std::string const &filename);

} // namespace parser

} // namespace storm

#endif /* STORM_PARSER_SPARSESTATEREWARDPARSER_H_ */
