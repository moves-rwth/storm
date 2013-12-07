#ifndef STORM_PARSER_SPARSESTATEREWARDPARSER_H_
#define STORM_PARSER_SPARSESTATEREWARDPARSER_H_

#include <cstdint>
#include <vector>
#include <string>

namespace storm {

namespace parser {

/*!
 * A class providing the functionality to parse a the state rewards of a model.
 */
class SparseStateRewardParser {
public:

	/*!
	 *	@brief Load state reward file and return vector of state rewards.
	 */
	static std::vector<double> parseSparseStateReward(uint_fast64_t stateCount, std::string const &filename);

};

} // namespace parser

} // namespace storm

#endif /* STORM_PARSER_SPARSESTATEREWARDPARSER_H_ */
