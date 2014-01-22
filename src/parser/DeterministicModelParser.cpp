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

/*!
 * Parses a transition file and a labeling file
 * Note that the labeling file may have at most as many nodes as the transition file!
 *
 * @param transitionSystemFile String containing the location of the transition file (....tra)
 * @param labelingFile String containing the location of the labeling file (....lab)
 * @param stateRewardFile String containing the location of the state reward file (...srew)
 * @param transitionRewardFile String containing the location of the transition reward file (...trew)
 */
DeterministicModelParser::Result DeterministicModelParser::parseDeterministicModel(std::string const & transitionSystemFile, std::string const & labelingFile, std::string const & stateRewardFile, std::string const & transitionRewardFile) {

	// Parse the transitions.
	storm::storage::SparseMatrix<double> transitions(std::move(storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitions(transitionSystemFile)));

	uint_fast64_t stateCount = transitions.getColumnCount();

	// Parse the state labeling.
	storm::models::AtomicPropositionsLabeling labeling(std::move(storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(stateCount, labelingFile)));

	// Construct the result.
	DeterministicModelParser::Result result(std::move(transitions), std::move(labeling));

	// Only parse state rewards if a file is given.
	if (stateRewardFile != "") {
		result.stateRewards = storm::parser::SparseStateRewardParser::parseSparseStateReward(stateCount, stateRewardFile);
	}

	// Only parse transition rewards if a file is given.
	if (transitionRewardFile != "") {
		result.transitionRewards = storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitionRewards(transitionRewardFile, result.transitionSystem);
	}

	return result;
}

/*!
 * Uses the parseDeterministicModel function internally to parse the given input files.
 * @note This is a short-hand for constructing a Dtmc directly from the data returned by @parseDeterministicModel
 * @return A Dtmc Model
 */
storm::models::Dtmc<double> DeterministicModelParser::parseDtmc(std::string const & transitionSystemFile, std::string const & labelingFile, std::string const & stateRewardFile, std::string const & transitionRewardFile) {

	DeterministicModelParser::Result parserResult(std::move(parseDeterministicModel(transitionSystemFile, labelingFile, stateRewardFile, transitionRewardFile)));
	return storm::models::Dtmc<double>(std::move(parserResult.transitionSystem), std::move(parserResult.labeling), std::move(parserResult.stateRewards), std::move(parserResult.transitionRewards), boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>());
}

/*!
 * Uses the parseDeterministicModel function internally to parse the given input files.
 * @note This is a short-hand for constructing a Ctmc directly from the data returned by @parseDeterministicModel
 * @return A Ctmc Model
 */
storm::models::Ctmc<double> DeterministicModelParser::parseCtmc(std::string const & transitionSystemFile, std::string const & labelingFile, std::string const & stateRewardFile, std::string const & transitionRewardFile) {

	DeterministicModelParser::Result parserResult(std::move(parseDeterministicModel(transitionSystemFile, labelingFile, stateRewardFile, transitionRewardFile)));
	return storm::models::Ctmc<double>(std::move(parserResult.transitionSystem), std::move(parserResult.labeling), std::move(parserResult.stateRewards), std::move(parserResult.transitionRewards), boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>());
}

} /* namespace parser */

} /* namespace storm */
