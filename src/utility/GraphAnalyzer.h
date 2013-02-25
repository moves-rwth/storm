/*
 * GraphAnalyzer.h
 *
 *  Created on: 28.11.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_UTILITY_GRAPHANALYZER_H_
#define STORM_UTILITY_GRAPHANALYZER_H_

#include "src/models/AbstractDeterministicModel.h"
#include "src/models/AbstractNondeterministicModel.h"
#include "src/exceptions/InvalidArgumentException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {

namespace utility {

class GraphAnalyzer {
public:
	/*!
	 * Computes the sets of states that have probability 0 or 1, respectively, of satisfying phi until psi in a
	 * deterministic model.
	 * @param model The model whose graph structure to search.
	 * @param phiStates The set of all states satisfying phi.
	 * @param psiStates The set of all states satisfying psi.
	 * @param statesWithProbability0 A pointer to a bit vector that is initially empty and will contain all states with
	 * probability 0 after the invocation of the function.
	 * @param statesWithProbability1 A pointer to a bit vector that is initially empty and will contain all states with
	 * probability 1 after the invocation of the function.
	 */
	template <class T>
	static void performProb01(storm::models::AbstractDeterministicModel<T>& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::storage::BitVector* statesWithProbability0, storm::storage::BitVector* statesWithProbability1) {
		// Check for valid parameters.
		if (statesWithProbability0 == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'statesWithProbability0' must not be null.");
			throw storm::exceptions::InvalidArgumentException("Parameter 'statesWithProbability0' must not be null.");
		}
		if (statesWithProbability1 == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'statesWithProbability1' must not be null.");
			throw storm::exceptions::InvalidArgumentException("Parameter 'statesWithProbability1' must not be null.");
		}

		// Perform the actual search.
		GraphAnalyzer::performProbGreater0(model, phiStates, psiStates, statesWithProbability0);
		GraphAnalyzer::performProb1(model, phiStates, psiStates, *statesWithProbability0, statesWithProbability1);
		statesWithProbability0->complement();
	}

	/*!
	 * Performs a backwards depth-first search trough the underlying graph structure
	 * of the given model to determine which states of the model have a positive probability
	 * of satisfying phi until psi. The resulting states are written to the given bit vector.
	 * @param model The model whose graph structure to search.
	 * @param phiStates A bit vector of all states satisfying phi.
	 * @param psiStates A bit vector of all states satisfying psi.
	 * @param statesWithProbabilityGreater0 A pointer to the result of the search for states that possess
	 * a positive probability of satisfying phi until psi.
	 */
	template <class T>
	static void performProbGreater0(storm::models::AbstractDeterministicModel<T>& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::storage::BitVector* statesWithProbabilityGreater0) {
		// Check for valid parameter.
		if (statesWithProbabilityGreater0 == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'statesWithProbabilityGreater0' must not be null.");
			throw storm::exceptions::InvalidArgumentException("Parameter 'statesWithProbabilityGreater0' must not be null.");
		}

		// Get the backwards transition relation from the model to ease the search.
		storm::models::GraphTransitions<T> backwardTransitions(model.getTransitionMatrix(), false);

		// Add all psi states as the already satisfy the condition.
		*statesWithProbabilityGreater0 |= psiStates;

		// Initialize the stack used for the DFS with the states
		std::vector<uint_fast64_t> stack;
		stack.reserve(model.getNumberOfStates());
		psiStates.addSetIndicesToList(stack);

		// Perform the actual DFS.
		while(!stack.empty()) {
			uint_fast64_t currentState = stack.back();
			stack.pop_back();

			for(auto it = backwardTransitions.beginStateSuccessorsIterator(currentState); it != backwardTransitions.endStateSuccessorsIterator(currentState); ++it) {
				if (phiStates.get(*it) && !statesWithProbabilityGreater0->get(*it)) {
					statesWithProbabilityGreater0->set(*it, true);
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
	 * @param statesWithProbabilityGreater0 A reference to a bit vector of states that possess a positive
	 * probability mass of satisfying phi until psi.
	 * @param alwaysPhiUntilPsiStates A pointer to the result of the search for states that only
	 * have paths satisfying phi until psi.
	 */
	template <class T>
	static void performProb1(storm::models::AbstractDeterministicModel<T>& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::storage::BitVector const& statesWithProbabilityGreater0, storm::storage::BitVector* statesWithProbability1) {
		// Check for valid parameter.
		if (statesWithProbability1 == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'statesWithProbability1' must not be null.");
			throw storm::exceptions::InvalidArgumentException("Parameter 'statesWithProbability1' must not be null.");
		}

		GraphAnalyzer::performProbGreater0(model, ~psiStates, ~statesWithProbabilityGreater0, statesWithProbability1);
		statesWithProbability1->complement();
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
	 * @param alwaysPhiUntilPsiStates A pointer to the result of the search for states that only
	 * have paths satisfying phi until psi.
	 */
	template <class T>
	static void performProb1(storm::models::AbstractDeterministicModel<T>& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::storage::BitVector* statesWithProbability1) {
		// Check for valid parameter.
		if (statesWithProbability1 == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'statesWithProbability1' must not be null.");
			throw storm::exceptions::InvalidArgumentException("Parameter 'statesWithProbability1' must not be null.");
		}

		storm::storage::BitVector* statesWithProbabilityGreater0 = new storm::storage::BitVector(model.getNumberOfStates());
		GraphAnalyzer::performProbGreater0(model, phiStates, psiStates, statesWithProbabilityGreater0);
		GraphAnalyzer::performProbGreater0(model, ~psiStates, ~(*statesWithProbabilityGreater0), statesWithProbability1);
		delete statesWithProbabilityGreater0;
		statesWithProbability1->complement();
	}

	template <class T>
	static void performProb01Max(storm::models::AbstractNondeterministicModel<T>& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::storage::BitVector* statesWithProbability0, storm::storage::BitVector* statesWithProbability1) {
		// Check for valid parameters.
		if (statesWithProbability0 == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'statesWithProbability0' must not be null.");
			throw storm::exceptions::InvalidArgumentException("Parameter 'statesWithProbability0' must not be null.");
		}
		if (statesWithProbability1 == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'statesWithProbability1' must not be null.");
			throw storm::exceptions::InvalidArgumentException("Parameter 'statesWithProbability1' must not be null.");
		}

		// Perform the actual search.
		GraphAnalyzer::performProb0A(model, phiStates, psiStates, statesWithProbability0);
		GraphAnalyzer::performProb1E(model, phiStates, psiStates, statesWithProbability1);
	}

	template <class T>
	static void performProb0A(storm::models::AbstractNondeterministicModel<T>& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::storage::BitVector* statesWithProbability0) {
		// Check for valid parameter.
		if (statesWithProbability0 == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'statesWithProbability0' must not be null.");
			throw storm::exceptions::InvalidArgumentException("Parameter 'statesWithProbability0' must not be null.");
		}

		// Get the backwards transition relation from the model to ease the search.
		storm::models::GraphTransitions<T> backwardTransitions(model.getTransitionMatrix(), model.getNondeterministicChoiceIndices(), false);

		// Add all psi states as the already satisfy the condition.
		*statesWithProbability0 |= psiStates;

		// Initialize the stack used for the DFS with the states
		std::vector<uint_fast64_t> stack;
		stack.reserve(model.getNumberOfStates());
		psiStates.addSetIndicesToList(stack);

		// Perform the actual DFS.
		while(!stack.empty()) {
			uint_fast64_t currentState = stack.back();
			stack.pop_back();

			for(auto it = backwardTransitions.beginStateSuccessorsIterator(currentState); it != backwardTransitions.endStateSuccessorsIterator(currentState); ++it) {
				if (phiStates.get(*it) && !statesWithProbability0->get(*it)) {
					statesWithProbability0->set(*it, true);
					stack.push_back(*it);
				}
			}
		}

		statesWithProbability0->complement();
	}

	template <class T>
	static void performProb1E(storm::models::AbstractNondeterministicModel<T>& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::storage::BitVector* statesWithProbability1) {
		// Check for valid parameters.
		if (statesWithProbability1 == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'statesWithProbability1' must not be null.");
			throw storm::exceptions::InvalidArgumentException("Parameter 'statesWithProbability1' must not be null.");
		}

		// Get some temporaries for convenience.
		std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix = model.getTransitionMatrix();
		std::shared_ptr<std::vector<uint_fast64_t>> nondeterministicChoiceIndices = model.getNondeterministicChoiceIndices();

		// Get the backwards transition relation from the model to ease the search.
		storm::models::GraphTransitions<T> backwardTransitions(model.getTransitionMatrix(), model.getNondeterministicChoiceIndices(), false);

		storm::storage::BitVector* currentStates = new storm::storage::BitVector(model.getNumberOfStates(), true);

		std::vector<uint_fast64_t> stack;
		stack.reserve(model.getNumberOfStates());

		bool done = false;
		while (!done) {
			stack.clear();
			storm::storage::BitVector* nextStates = new storm::storage::BitVector(psiStates);
			psiStates.addSetIndicesToList(stack);

			while (!stack.empty()) {
				uint_fast64_t currentState = stack.back();
				stack.pop_back();

				for(auto it = backwardTransitions.beginStateSuccessorsIterator(currentState); it != backwardTransitions.endStateSuccessorsIterator(currentState); ++it) {
					if (phiStates.get(*it) && !nextStates->get(*it)) {
						// Check whether the predecessor has only successors in the current state set for one of the
						// nondeterminstic choices.
						for (uint_fast64_t row = (*nondeterministicChoiceIndices)[*it]; row < (*nondeterministicChoiceIndices)[*it + 1]; ++row) {
							bool allSuccessorsInCurrentStates = true;
							for (auto colIt = transitionMatrix->beginConstColumnIterator(row); colIt != transitionMatrix->endConstColumnIterator(row); ++colIt) {
								if (!currentStates->get(*colIt)) {
									allSuccessorsInCurrentStates = false;
									break;
								}
							}

							// If all successors for a given nondeterministic choice are in the current state set, we
							// add it to the set of states for the next iteration and perform a backward search from
							// that state.
							if (allSuccessorsInCurrentStates) {
								nextStates->set(*it, true);
								stack.push_back(*it);
								break;
							}
						}
					}
				}
			}

			// Check whether we need to perform an additional iteration.
			if (*currentStates == *nextStates) {
				done = true;
			} else {
				*currentStates = *nextStates;
			}
		}

		*statesWithProbability1 = *currentStates;
		delete currentStates;
	}

	template <class T>
	static void performProb01Min(storm::models::AbstractNondeterministicModel<T>& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::storage::BitVector* statesWithProbability0, storm::storage::BitVector* statesWithProbability1) {
		// Check for valid parameters.
		if (statesWithProbability0 == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'statesWithProbability0' must not be null.");
			throw storm::exceptions::InvalidArgumentException("Parameter 'statesWithProbability0' must not be null.");
		}
		if (statesWithProbability1 == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'statesWithProbability1' must not be null.");
			throw storm::exceptions::InvalidArgumentException("Parameter 'statesWithProbability1' must not be null.");
		}

		// Perform the actual search.
		GraphAnalyzer::performProb0E(model, phiStates, psiStates, statesWithProbability0);
		GraphAnalyzer::performProb1A(model, phiStates, psiStates, statesWithProbability1);
	}

	template <class T>
	static void performProb0E(storm::models::AbstractNondeterministicModel<T>& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::storage::BitVector* statesWithProbability0) {
		// Check for valid parameter.
		if (statesWithProbability0 == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'statesWithProbability0' must not be null.");
			throw storm::exceptions::InvalidArgumentException("Parameter 'statesWithProbability0' must not be null.");
		}

		// Get some temporaries for convenience.
		std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix = model.getTransitionMatrix();
		std::shared_ptr<std::vector<uint_fast64_t>> nondeterministicChoiceIndices = model.getNondeterministicChoiceIndices();

		// Get the backwards transition relation from the model to ease the search.
		storm::models::GraphTransitions<T> backwardTransitions(transitionMatrix, nondeterministicChoiceIndices, false);

		// Add all psi states as the already satisfy the condition.
		*statesWithProbability0 |= psiStates;

		// Initialize the stack used for the DFS with the states
		std::vector<uint_fast64_t> stack;
		stack.reserve(model.getNumberOfStates());
		psiStates.addSetIndicesToList(stack);

		// Perform the actual DFS.
		while(!stack.empty()) {
			uint_fast64_t currentState = stack.back();
			stack.pop_back();

			for(auto it = backwardTransitions.beginStateSuccessorsIterator(currentState); it != backwardTransitions.endStateSuccessorsIterator(currentState); ++it) {
				if (phiStates.get(*it) && !statesWithProbability0->get(*it)) {
					// Check whether the predecessor has at least one successor in the current state
					// set for every nondeterministic choice.
					bool addToStatesWithProbability0 = true;
					for (auto rowIt = nondeterministicChoiceIndices->begin() + *it; rowIt != nondeterministicChoiceIndices->begin() + *it + 1; ++rowIt) {
						bool hasAtLeastOneSuccessorWithProbabilityGreater0 = false;
						for (auto colIt = transitionMatrix->beginConstColumnIterator(*rowIt); colIt != transitionMatrix->endConstColumnIterator(*rowIt); ++colIt) {
							if (statesWithProbability0->get(*colIt)) {
								hasAtLeastOneSuccessorWithProbabilityGreater0 = true;
								break;
							}
						}
						if (!hasAtLeastOneSuccessorWithProbabilityGreater0) {
							addToStatesWithProbability0 = false;
							break;
						}
					}

					// If we need to add the state, then actually add it and perform further search
					// from the state.
					if (addToStatesWithProbability0) {
						statesWithProbability0->set(*it, true);
						stack.push_back(*it);
					}
				}
			}
		}

		statesWithProbability0->complement();
	}

	template <class T>
	static void performProb1A(storm::models::AbstractNondeterministicModel<T>& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::storage::BitVector* statesWithProbability1) {
		// Check for valid parameters.
		if (statesWithProbability1 == nullptr) {
			LOG4CPLUS_ERROR(logger, "Parameter 'statesWithProbability1' must not be null.");
			throw storm::exceptions::InvalidArgumentException("Parameter 'statesWithProbability1' must not be null.");
		}

		// Get some temporaries for convenience.
		std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix = model.getTransitionMatrix();
		std::shared_ptr<std::vector<uint_fast64_t>> nondeterministicChoiceIndices = model.getNondeterministicChoiceIndices();

		// Get the backwards transition relation from the model to ease the search.
		storm::models::GraphTransitions<T> backwardTransitions(model.getTransitionMatrix(), model.getNondeterministicChoiceIndices(), false);

		storm::storage::BitVector* currentStates = new storm::storage::BitVector(model.getNumberOfStates(), true);

		std::vector<uint_fast64_t> stack;
		stack.reserve(model.getNumberOfStates());

		bool done = false;
		while (!done) {
			stack.clear();
			storm::storage::BitVector* nextStates = new storm::storage::BitVector(psiStates);
			psiStates.addSetIndicesToList(stack);

			while (!stack.empty()) {
				uint_fast64_t currentState = stack.back();
				stack.pop_back();

				for(auto it = backwardTransitions.beginStateSuccessorsIterator(currentState); it != backwardTransitions.endStateSuccessorsIterator(currentState); ++it) {
					if (phiStates.get(*it) && !nextStates->get(*it)) {
						// Check whether the predecessor has only successors in the current state set for all of the
						// nondeterminstic choices.
						bool allSuccessorsInCurrentStatesForAllChoices = true;
						for (uint_fast64_t row = (*nondeterministicChoiceIndices)[*it]; row < (*nondeterministicChoiceIndices)[*it + 1]; ++row) {
							for (auto colIt = transitionMatrix->beginConstColumnIterator(row); colIt != transitionMatrix->endConstColumnIterator(row); ++colIt) {
								if (!currentStates->get(*colIt)) {
									allSuccessorsInCurrentStatesForAllChoices = false;
									goto afterCheckLoop;
								}
							}
						}

				afterCheckLoop:
						// If all successors for all nondeterministic choices are in the current state set, we
						// add it to the set of states for the next iteration and perform a backward search from
						// that state.
						if (allSuccessorsInCurrentStatesForAllChoices) {
							nextStates->set(*it, true);
							stack.push_back(*it);
						}
					}
				}
			}

			// Check whether we need to perform an additional iteration.
			if (*currentStates == *nextStates) {
				done = true;
			} else {
				*currentStates = *nextStates;
			}
		}

		*statesWithProbability1 = *currentStates;
		delete currentStates;
	}
};

} // namespace utility

} // namespace storm

#endif /* STORM_UTILITY_GRAPHANALYZER_H_ */
