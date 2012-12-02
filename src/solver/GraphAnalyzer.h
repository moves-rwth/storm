/*
 * GraphAnalyzer.h
 *
 *  Created on: 28.11.2012
 *      Author: Christian Dehnert
 */

#ifndef GRAPHANALYZER_H_
#define GRAPHANALYZER_H_

#include "src/models/Dtmc.h"

#include <iostream>

namespace mrmc {

namespace solver {

class GraphAnalyzer {
public:
	/*!
	 * Performs a backwards depth-first search trough the underlying graph structure
	 * of the given model to determine which states of the model can reach one
	 * of the given target states whilst always staying in the set of filter states
	 * before. The resulting states are written to the given bit vector.
	 * @param model The model whose graph structure to search.
	 * @param targetStates The target states of the search.
	 * @param filterStates A set of states constraining the search.
	 * @param existentialReachabilityStates The result of the search.
	 */
	template <class T>
	static void getExistsPhiUntilPsiStates(mrmc::models::Dtmc<T>& model, const mrmc::storage::BitVector& phiStates, const mrmc::storage::BitVector& psiStates, mrmc::storage::BitVector& existsPhiUntilPsiStates) {
		// Get the backwards transition relation from the model to ease the search.
		mrmc::models::GraphTransitions<T>& backwardTransitions = model.getBackwardTransitions();

		// Add all psi states as the already satisfy the condition.
		existsPhiUntilPsiStates |= psiStates;

		// Initialize the stack used for the DFS with the states
		std::vector<uint_fast64_t> stack;
		stack.reserve(model.getNumberOfStates());
		psiStates.getList(stack);

		// Perform the actual DFS.
		while(!stack.empty()) {
			uint_fast64_t currentState = stack.back();
			stack.pop_back();

			for(auto it = backwardTransitions.beginStateSuccessorsIterator(currentState); it != backwardTransitions.endStateSuccessorsIterator(currentState); ++it) {
				if (phiStates.get(*it) && !existsPhiUntilPsiStates.get(*it)) {
					existsPhiUntilPsiStates.set(*it, true);
					stack.push_back(*it);
				}
			}
		}
	}

	/*!
	 * Computes the set of states of the given model for which all paths lead to
	 * the given set of target states and only visit states from the filter set
	 * before. In order to do this, it uses the given set of states that
	 * characterizes the states that possess at least one path to a target state.
	 * The results are written to the given bit vector.
	 * @param model The model whose graph structure to search.
	 * @param targetStates The target states of the search.
	 * @param filterStates A set of states constraining the search.
	 * @param existentialReachabilityStates The set of states that possess at
	 * least one path to a target state.
	 * @param universalReachabilityStates The result of the search.
	 */
	template <class T>
	static void getUniversalReachabilityStates(mrmc::models::Dtmc<T>& model, const mrmc::storage::BitVector& phiStates, const mrmc::storage::BitVector& psiStates, const mrmc::storage::BitVector& existsPhiUntilPsiStates, mrmc::storage::BitVector& alwaysPhiUntilPsiStates) {
		GraphAnalyzer::getExistsPhiUntilPsiStates(model, ~psiStates, ~existsPhiUntilPsiStates, alwaysPhiUntilPsiStates);
		alwaysPhiUntilPsiStates.complement();
	}

	/*!
	 * Computes the set of states of the given model for which all paths lead to
	 * the given set of target states and only visit states from the filter set
	 * before.
	 * @param model The model whose graph structure to search.
	 * @param targetStates The target states of the search.
	 * @param filterStates A set of states constraining the search.
	 * @param universalReachabilityStates The result of the search.
	 */
	template <class T>
	static void getUniversalReachabilityStates(mrmc::models::Dtmc<T>& model, const mrmc::storage::BitVector& phiStates, const mrmc::storage::BitVector& psiStates, mrmc::storage::BitVector& alwaysPhiUntilPsiStates) {
		mrmc::storage::BitVector existsPhiUntilPsiStates(model.getNumberOfStates());
		GraphAnalyzer::getExistsPhiUntilPsiStates(model, phiStates, psiStates, existsPhiUntilPsiStates);
		GraphAnalyzer::getUniversalReachabilityStates(model, phiStates, psiStates, existsPhiUntilPsiStates, alwaysPhiUntilPsiStates);
	}

};

} // namespace solver

} // namespace mrmc

#endif /* GRAPHANALYZER_H_ */
