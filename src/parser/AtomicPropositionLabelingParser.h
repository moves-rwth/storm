#ifndef STORM_PARSER_LABPARSER_H_
#define STORM_PARSER_LABPARSER_H_

#include "src/models/AtomicPropositionsLabeling.h"
#include "boost/integer/integer_mask.hpp"

#include "src/parser/Parser.h"

#include <memory>

namespace storm {
namespace parser {

/*!
 *	@brief Load label file and return initialized AtomicPropositionsLabeling object.
 *
 */
storm::models::AtomicPropositionsLabeling AtomicPropositionLabelingParser(uint_fast64_t node_count, std::string const &filename);

} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_LABPARSER_H_ */
