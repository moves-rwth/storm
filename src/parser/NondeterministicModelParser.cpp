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
 * Parses a transition file and a labeling file and produces a MDP out of them; a pointer to the mdp
 * is saved in the field "mdp"
 * Note that the labeling file may have at most as many nodes as the transition file!
 *
 * @param transitionSystemFile String containing the location of the transition file (....tra)
 * @param labelingFile String containing the location of the labeling file (....lab)
 * @param stateRewardFile String containing the location of the state reward file (...srew)
 * @param transitionRewardFile String containing the location of the transition reward file (...trew)
 */
NondeterministicModelParser::NondeterministicModelParser(std::string const & transitionSystemFile, std::string const & labelingFile,
		std::string const & stateRewardFile, std::string const & transitionRewardFile) {
	NondeterministicSparseTransitionParserResult_t nondeterministicSparseTransitionParserResult = storm::parser::NondeterministicSparseTransitionParser(transitionSystemFile);
	uint_fast64_t stateCount = nondeterministicSparseTransitionParserResult.first.getColumnCount();

	if (stateRewardFile != "") {
		std::vector<double> stateRewards = storm::parser::SparseStateRewardParser(stateCount, stateRewardFile);
	}
	if (transitionRewardFile != "") {
		RewardMatrixInformationStruct* rewardMatrixInfo = new RewardMatrixInformationStruct(nondeterministicSparseTransitionParserResult.first.getRowCount(), nondeterministicSparseTransitionParserResult.first.getColumnCount(), nondeterministicSparseTransitionParserResult.second);
		storm::parser::NondeterministicSparseTransitionParser trp(transitionRewardFile, rewardMatrixInfo);
		delete rewardMatrixInfo;
		this->transitionRewardMatrix = trp.getMatrix();
	}

	this->probabilityMatrix = tp.getMatrix();
	this->stateLabeling = std::shared_ptr<storm::models::AtomicPropositionsLabeling>(storm::parser::AtomicPropositionLabelingParser(stateCount, labelingFile));
	this->rowMapping = tp.getRowMapping();

	this->mdp = nullptr;
	this->ctmdp = nullptr;
}

} /* namespace parser */

} /* namespace storm */
