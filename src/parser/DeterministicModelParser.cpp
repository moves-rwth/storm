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
 * Parses a transition file and a labeling file and produces a DTMC out of them; a pointer to the dtmc
 * is saved in the field "dtmc"
 * Note that the labeling file may have at most as many nodes as the transition file!
 *
 * @param transitionSystemFile String containing the location of the transition file (....tra)
 * @param labelingFile String containing the location of the labeling file (....lab)
 * @param stateRewardFile String containing the location of the state reward file (...srew)
 * @param transitionRewardFile String containing the location of the transition reward file (...trew)
 */
DeterministicModelParser::DeterministicModelParser(std::string const & transitionSystemFile, std::string const & labelingFile,
		std::string const & stateRewardFile, std::string const & transitionRewardFile) {
	storm::parser::DeterministicSparseTransitionParser tp(transitionSystemFile);
	this->transitionSystem = tp.getMatrix();

	uint_fast64_t stateCount = this->transitionSystem->getRowCount();

	storm::parser::AtomicPropositionLabelingParser lp(stateCount, labelingFile);
	this->labeling = lp.getLabeling();

	this->stateRewards = nullptr;
	this->transitionRewards = nullptr;

	if (stateRewardFile != "") {
		storm::parser::SparseStateRewardParser srp(stateCount, stateRewardFile);
		this->stateRewards = srp.getStateRewards();
	}
	if (transitionRewardFile != "") {
		RewardMatrixInformationStruct* rewardMatrixInfo = new RewardMatrixInformationStruct(stateCount, stateCount, nullptr);
		storm::parser::DeterministicSparseTransitionParser trp(transitionRewardFile, false, rewardMatrixInfo);
		delete rewardMatrixInfo;
		this->transitionRewards = trp.getMatrix();
	}
	this->dtmc = nullptr;
	this->ctmc = nullptr;
}

} /* namespace parser */

} /* namespace storm */
