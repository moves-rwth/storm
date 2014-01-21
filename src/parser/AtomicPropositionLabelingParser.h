#ifndef STORM_PARSER_ATOMICPROPOSITIONLABELINGPARSER_H_
#define STORM_PARSER_ATOMICPROPOSITIONLABELINGPARSER_H_

#include "src/models/AtomicPropositionsLabeling.h"
#include <cstdint>

namespace storm {
	namespace parser {

		class AtomicPropositionLabelingParser {

		public:

			/*
			 *	Reads a label file and puts the result in an AtomicPropositionsLabeling object.
			 *
			 *	@param node_count The number of states of the model to be labeled.
			 *	@param filename The path and name of the labeling (.lab) file.
			 *	@return The parsed labeling as an AtomicPropositionsLabeling object.
			 */
			static storm::models::AtomicPropositionsLabeling parseAtomicPropositionLabeling(uint_fast64_t node_count, std::string const &filename);

		};

	} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_ATOMICPROPOSITIONLABELINGPARSER_H_ */
