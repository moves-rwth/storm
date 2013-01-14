/*
 * MdpParser.cpp
 *
 *  Created on: 14.01.2013
 *      Author: Philipp Berger
 */

#include "src/parser/MdpParser.h"

#include <string>
#include <vector>

#include "src/parser/NonDeterministicSparseTransitionParser.h"
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
MdpParser::MdpParser(std::string const & transitionSystemFile, std::string const & labelingFile,
		std::string const & stateRewardFile, std::string const & transitionRewardFile) {
	storm::parser::NonDeterministicSparseTransitionParser tp(transitionSystemFile);
	uint_fast64_t stateCount = tp.getMatrix()->getRowCount();

	std::shared_ptr<std::vector<double>> stateRewards = nullptr;
	std::shared_ptr<storm::storage::SparseMatrix<double>> transitionRewards = nullptr;

	storm::parser::AtomicPropositionLabelingParser lp(stateCount, labelingFile);
	if (stateRewardFile != "") {
		storm::parser::SparseStateRewardParser srp(stateCount, stateRewardFile);
		stateRewards = srp.getStateRewards();
	}
	if (transitionRewardFile != "") {
		storm::parser::NonDeterministicSparseTransitionParser trp(transitionRewardFile);
		transitionRewards = trp.getMatrix();
	}

	mdp = std::shared_ptr<storm::models::Mdp<double>>(new storm::models::Mdp<double>(tp.getMatrix(), lp.getLabeling(), stateRewards, transitionRewards));
}

} /* namespace parser */

} /* namespace storm */
