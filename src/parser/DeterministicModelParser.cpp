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
DeterministicModelParserResultContainer<double> parseDeterministicModel(std::string const & transitionSystemFile, std::string const & labelingFile,
																		std::string const & stateRewardFile, std::string const & transitionRewardFile) {

	storm::storage::SparseMatrix<double> resultTransitionSystem(std::move(storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitions(transitionSystemFile)));

	uint_fast64_t stateCount = resultTransitionSystem.getColumnCount();
	uint_fast64_t rowCount = resultTransitionSystem.getRowCount();

	storm::models::AtomicPropositionsLabeling resultLabeling(std::move(storm::parser::AtomicPropositionLabelingParser(stateCount, labelingFile)));

	DeterministicModelParserResultContainer<double> result(std::move(resultTransitionSystem), std::move(resultLabeling));

	if (stateRewardFile != "") {
		result.stateRewards.reset(storm::parser::SparseStateRewardParser::parseSparseStateReward(stateCount, stateRewardFile));
	}
	if (transitionRewardFile != "") {
		RewardMatrixInformationStruct rewardMatrixInfo(rowCount, stateCount, nullptr);
		result.transitionRewards.reset(std::move(storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitionRewards(transitionRewardFile, rewardMatrixInfo)));
	}

	return result;
}

/*!
 * Uses the Function parseDeterministicModel internally to parse the given input files.
 * @note This is a Short-Hand for Constructing a Dtmc directly from the data returned by @parseDeterministicModel
 * @return A Dtmc Model
 */
storm::models::Dtmc<double> DeterministicModelParserAsDtmc(std::string const & transitionSystemFile, std::string const & labelingFile,
														   std::string const & stateRewardFile, std::string const & transitionRewardFile) {
	DeterministicModelParserResultContainer<double> parserResult(std::move(parseDeterministicModel(transitionSystemFile, labelingFile, stateRewardFile, transitionRewardFile)));
	return storm::models::Dtmc<double>(std::move(parserResult.transitionSystem), std::move(parserResult.labeling), std::move(parserResult.stateRewards), std::move(parserResult.transitionRewards), boost::optional<std::vector<storm::storage::VectorSet<uint_fast64_t>>>());
}

/*!
 * Uses the Function parseDeterministicModel internally to parse the given input files.
 * @note This is a Short-Hand for Constructing a Ctmc directly from the data returned by @parseDeterministicModel
 * @return A Ctmc Model
 */
storm::models::Ctmc<double> DeterministicModelParserAsCtmc(std::string const & transitionSystemFile, std::string const & labelingFile,
				std::string const & stateRewardFile, std::string const & transitionRewardFile) {
	DeterministicModelParserResultContainer<double> parserResult(std::move(parseDeterministicModel(transitionSystemFile, labelingFile, stateRewardFile, transitionRewardFile)));
	return storm::models::Ctmc<double>(std::move(parserResult.transitionSystem), std::move(parserResult.labeling), std::move(parserResult.stateRewards), std::move(parserResult.transitionRewards), boost::optional<std::vector<storm::storage::VectorSet<uint_fast64_t>>>());
}

} /* namespace parser */

} /* namespace storm */
