#ifndef MRMC_PARSER_SPARSESTATEREWARDPARSER_H_
#define MRMC_PARSER_SPARSESTATEREWARDPARSER_H_

#include "boost/integer/integer_mask.hpp"
#include "src/parser/Parser.h"
#include <memory>
#include <vector>

namespace mrmc {

namespace parser {

/*!
 *	@brief Load state reward file and return vector of state rewards.
 */
class SparseStateRewardParser : Parser {
	public:
		SparseStateRewardParser(uint_fast64_t stateCount, std::string const &filename);

		std::shared_ptr<std::vector<double>> getStateRewards() {
			return this->stateRewards;
		}
	
	private:
		std::shared_ptr<std::vector<double>> stateRewards;
};

} // namespace parser

} // namespace mrmc

#endif /* MRMC_PARSER_SPARSESTATEREWARDPARSER_H_ */
