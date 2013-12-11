/*
 * NonDeterministicModelParser.cpp
 *
 *  Created on: 14.01.2013
 *      Author: Philipp Berger
 */

#include "src/parser/NondeterministicModelParser.h"

#include <string>
#include <vector>

#include "src/parser/NondeterministicSparseTransitionParser.h"
#include "src/parser/AtomicPropositionLabelingParser.h"
#include "src/parser/SparseStateRewardParser.h"

namespace storm {
namespace parser {

/*!
 * Parses a transition file and a labeling file and produces an intermediate Result Container
 * Note that the labeling file may have at most as many nodes as the transition file!
 *
 * @param transitionSystemFile String containing the location of the transition file (....tra)
 * @param labelingFile String containing the location of the labeling file (....lab)
 * @param stateRewardFile String containing the location of the state reward file (...srew)
 * @param transitionRewardFile String containing the location of the transition reward file (...trew)
 */
NondeterministicModelParserResultContainer<double> parseNondeterministicModel(std::string const & transitionSystemFile, std::string const & labelingFile,
		std::string const & stateRewardFile, std::string const & transitionRewardFile) {

	NondeterministicSparseTransitionParserResult_t nondeterministicSparseTransitionParserResult(std::move(storm::parser::NondeterministicSparseTransitionParser(transitionSystemFile)));
	storm::storage::SparseMatrix<double> resultTransitionSystem(std::move(nondeterministicSparseTransitionParserResult.first));

	uint_fast64_t stateCount = resultTransitionSystem.getColumnCount();
	uint_fast64_t rowCount = resultTransitionSystem.getRowCount();

	storm::models::AtomicPropositionsLabeling resultLabeling(std::move(storm::parser::AtomicPropositionLabelingParser(stateCount, labelingFile)));

	NondeterministicModelParserResultContainer<double> result(std::move(resultTransitionSystem), std::move(nondeterministicSparseTransitionParserResult.second), std::move(resultLabeling));
	
	if (stateRewardFile != "") {
		result.stateRewards.reset(storm::parser::SparseStateRewardParser(stateCount, stateRewardFile));
	}
	if (transitionRewardFile != "") {
		RewardMatrixInformationStruct* rewardMatrixInfo = new RewardMatrixInformationStruct(rowCount, stateCount, &result.rowMapping);
		result.transitionRewards.reset(storm::parser::NondeterministicSparseTransitionParser(transitionRewardFile, rewardMatrixInfo).first);
		delete rewardMatrixInfo;
	}
	return result;
}

/*!
 * Uses the Function parseNondeterministicModel internally to parse the given input files.
 * @note This is a Short-Hand for Constructing a Mdp directly from the data returned by @parseNondeterministicModel
 * @return A Mdp Model
 */
storm::models::Mdp<double> NondeterministicModelParserAsMdp(std::string const & transitionSystemFile, std::string const & labelingFile,
														 std::string const & stateRewardFile, std::string const & transitionRewardFile) {
	NondeterministicModelParserResultContainer<double> parserResult = parseNondeterministicModel(transitionSystemFile, labelingFile, stateRewardFile, transitionRewardFile);
	return storm::models::Mdp<double>(std::move(parserResult.transitionSystem), std::move(parserResult.labeling), std::move(parserResult.rowMapping), std::move(parserResult.stateRewards), std::move(parserResult.transitionRewards), boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>());
}

/*!
 * Uses the Function parseNondeterministicModel internally to parse the given input files.
 * @note This is a Short-Hand for Constructing a Ctmdp directly from the data returned by @parseNondeterministicModel
 * @return A Ctmdp Model
 */
storm::models::Ctmdp<double> NondeterministicModelParserAsCtmdp(std::string const & transitionSystemFile, std::string const & labelingFile,
															 std::string const & stateRewardFile, std::string const & transitionRewardFile) {
	NondeterministicModelParserResultContainer<double> parserResult = parseNondeterministicModel(transitionSystemFile, labelingFile, stateRewardFile, transitionRewardFile);
	return storm::models::Ctmdp<double>(std::move(parserResult.transitionSystem), std::move(parserResult.labeling), std::move(parserResult.rowMapping), std::move(parserResult.stateRewards), std::move(parserResult.transitionRewards), boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>());
}

} /* namespace parser */

} /* namespace storm */
