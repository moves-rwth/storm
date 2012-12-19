#ifndef MRMC_PARSER_LABPARSER_H_
#define MRMC_PARSER_LABPARSER_H_

#include "src/models/AtomicPropositionsLabeling.h"
#include "boost/integer/integer_mask.hpp"

#include "src/parser/Parser.h"

#include <memory>

namespace mrmc {
namespace parser {

/*!
 *	@brief Load label file and return initialized AtomicPropositionsLabeling object.
 *
 *	Note that this class creates a new AtomicPropositionsLabeling object that can
 *	be accessed via getLabeling(). However, it will not delete this object!
 */
class AtomicPropositionLabelingParser : Parser {
	public:
		AtomicPropositionLabelingParser(uint_fast64_t node_count, std::string const &filename);

		std::shared_ptr<mrmc::models::AtomicPropositionsLabeling> getLabeling() {
			return this->labeling;
		}
	
	private:
		std::shared_ptr<mrmc::models::AtomicPropositionsLabeling> labeling;
};

} // namespace parser
} // namespace mrmc

#endif /* MRMC_PARSER_LABPARSER_H_ */
