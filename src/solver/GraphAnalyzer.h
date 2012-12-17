/*
 * GraphAnalyzer.h
 *
 *  Created on: 28.11.2012
 *      Author: Christian Dehnert
 */

#ifndef MRMC_SOLVER_GRAPHANALYZER_H_
#define MRMC_SOLVER_GRAPHANALYZER_H_

#include "src/models/Dtmc.h"
#include "src/exceptions/InvalidArgumentException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

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
	 * @param phiStates A bit vector of all states satisfying phi.
	 * @param psiStates A bit vector of all states satisfying psi.
	 * @param existsPhiUntilPsiStates A pointer to the result of the search for states that possess
	 * a paths satisfying phi until psi.
	 */
	template <class T>
	static void getExistsPhiUntilPsiStates(mrmc::models::Dtmc<T>& model, const mrmc::storage::BitVector& phiStates, const mrmc::storage::BitVector& psiStates, mrmc::storage::BitVector* existsPhiUntilPsiStates) {
		// Check for valid parameter.
		if (existsPhiUntilPsiStates == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'existsPhiUntilPhiStates' must not be null.");
			throw mrmc::exceptions::InvalidArgumentException("Parameter 'existsPhiUntilPhiStates' must not be null.");
		}

		// Get the backwards transition relation from the model to ease the search.
		mrmc::models::GraphTransitions<T>& backwardTransitions = model.getBackwardTransitions();

		// Add all psi states as the already satisfy the condition.
		*existsPhiUntilPsiStates |= psiStates;

		// Initialize the stack used for the DFS with the states
		std::vector<uint_fast64_t> stack;
		stack.reserve(model.getNumberOfStates());
		psiStates.getList(stack);

		// Perform the actual DFS.
		while(!stack.empty()) {
			uint_fast64_t currentState = stack.back();
			stack.pop_back();

			for(auto it = backwardTransitions.beginStateSuccessorsIterator(currentState); it != backwardTransitions.endStateSuccessorsIterator(currentState); ++it) {
				if (phiStates.get(*it) && !existsPhiUntilPsiStates->get(*it)) {
					existsPhiUntilPsiStates->set(*it, true);
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
	 * @param phiStates A bit vector of all states satisfying phi.
	 * @param psiStates A bit vector of all states satisfying psi.
	 * @param existsPhiUntilPsiStates A reference to a bit vector of states that possess a path
	 * satisfying phi until psi.
	 * @param alwaysPhiUntilPsiStates A pointer to the result of the search for states that only
	 * have paths satisfying phi until psi.
	 */
	template <class T>
	static void getAlwaysPhiUntilPsiStates(mrmc::models::Dtmc<T>& model, const mrmc::storage::BitVector& phiStates, const mrmc::storage::BitVector& psiStates, const mrmc::storage::BitVector& existsPhiUntilPsiStates, mrmc::storage::BitVector* alwaysPhiUntilPsiStates) {
		// Check for valid parameter.
		if (alwaysPhiUntilPsiStates == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'alwaysPhiUntilPhiStates' must not be null.");
			throw mrmc::exceptions::InvalidArgumentException("Parameter 'alwaysPhiUntilPhiStates' must not be null.");
		}

		GraphAnalyzer::getExistsPhiUntilPsiStates(model, ~psiStates, ~existsPhiUntilPsiStates, alwaysPhiUntilPsiStates);
		alwaysPhiUntilPsiStates->complement();
	}

	/*!
	 * Computes the set of states of the given model for which all paths lead to
	 * the given set of target states and only visit states from the filter set
	 * before.
	 * @param model The model whose graph structure to search.
	 * @param phiStates A bit vector of all states satisfying phi.
	 * @param psiStates A bit vector of all states satisfying psi.
	 * @param existsPhiUntilPsiStates A pointer to the result of the search for states that possess
	 * a path satisfying phi until psi.
	 * @param alwaysPhiUntilPsiStates A pointer to the result of the search for states that only
	 * have paths satisfying phi until psi.
	 */
	template <class T>
	static void getPhiUntilPsiStates(mrmc::models::Dtmc<T>& model, const mrmc::storage::BitVector& phiStates, const mrmc::storage::BitVector& psiStates, mrmc::storage::BitVector* existsPhiUntilPsiStates, mrmc::storage::BitVector* alwaysPhiUntilPsiStates) {
		// Check for valid parameters.
		if (existsPhiUntilPsiStates == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'existsPhiUntilPhiStates' must not be null.");
			throw mrmc::exceptions::InvalidArgumentException("Parameter 'existsPhiUntilPhiStates' must not be null.");
		}
		if (alwaysPhiUntilPsiStates == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'alwaysPhiUntilPhiStates' must not be null.");
			throw mrmc::exceptions::InvalidArgumentException("Parameter 'alwaysPhiUntilPhiStates' must not be null.");
		}

		// Perform search.
		GraphAnalyzer::getExistsPhiUntilPsiStates(model, phiStates, psiStates, existsPhiUntilPsiStates);
		GraphAnalyzer::getAlwaysPhiUntilPsiStates(model, phiStates, psiStates, *existsPhiUntilPsiStates, alwaysPhiUntilPsiStates);
	}

};

} // namespace solver

} // namespace mrmc

#endif /* MRMC_SOLVER_GRAPHANALYZER_H_ */
