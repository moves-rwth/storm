/*
 * DeterministicModelParser.cpp
 *
 *  Created on: 19.12.2012
 *      Author: thomas
 */

#include "src/parser/DeterministicModelParser.h"

#include <string>
#include <vector>

#include "src/parser/DeterministicSparseTransitionParser.h"
#include "src/parser/AtomicPropositionLabelingParser.h"
#include "src/parser/SparseStateRewardParser.h"

namespace storm {
	namespace parser {

		DeterministicModelParser::Result DeterministicModelParser::parseDeterministicModel(std::string const & transitionsFilename, std::string const & labelingFilename, std::string const & stateRewardFilename, std::string const & transitionRewardFilename) {

			// Parse the transitions.
			storm::storage::SparseMatrix<double> transitions(std::move(storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitions<double>(transitionsFilename)));

			uint_fast64_t stateCount = transitions.getColumnCount();

			// Parse the state labeling.
			storm::models::AtomicPropositionsLabeling labeling(std::move(storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(stateCount, labelingFilename)));

			// Construct the result.
			DeterministicModelParser::Result result(std::move(transitions), std::move(labeling));

			// Only parse state rewards if a file is given.
			if (stateRewardFilename != "") {
				result.stateRewards = storm::parser::SparseStateRewardParser::parseSparseStateReward(stateCount, stateRewardFilename);
			}

			// Only parse transition rewards if a file is given.
			if (transitionRewardFilename != "") {
				result.transitionRewards = storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitionRewards(transitionRewardFilename, result.transitionSystem);
			}

			return result;
		}

		storm::models::Dtmc<double> DeterministicModelParser::parseDtmc(std::string const & transitionsFilename, std::string const & labelingFilename, std::string const & stateRewardFilename, std::string const & transitionRewardFilename) {

			DeterministicModelParser::Result parserResult(std::move(parseDeterministicModel(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename)));
			return storm::models::Dtmc<double>(std::move(parserResult.transitionSystem), std::move(parserResult.labeling), std::move(parserResult.stateRewards), std::move(parserResult.transitionRewards), boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>());
		}

		storm::models::Ctmc<double> DeterministicModelParser::parseCtmc(std::string const & transitionsFilename, std::string const & labelingFilename, std::string const & stateRewardFilename, std::string const & transitionRewardFilename) {

			DeterministicModelParser::Result parserResult(std::move(parseDeterministicModel(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename)));
			return storm::models::Ctmc<double>(std::move(parserResult.transitionSystem), std::move(parserResult.labeling), std::move(parserResult.stateRewards), std::move(parserResult.transitionRewards), boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>());
		}

	} /* namespace parser */
} /* namespace storm */
