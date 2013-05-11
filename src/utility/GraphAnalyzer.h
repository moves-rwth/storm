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
     *
	 * @param model The model whose graph structure to search.
	 * @param phiStates The set of all states satisfying phi.
	 * @param psiStates The set of all states satisfying psi.
	 * @return A pair of bit vectors such that the first bit vector stores the indices of all states
     * with probability 0 and the second stores all indices of states with probability 1.
	 */
	template <typename T>
	static std::pair<storm::storage::BitVector, storm::storage::BitVector> performProb01(storm::models::AbstractDeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
        std::pair<storm::storage::BitVector, storm::storage::BitVector> result;
		result.first = GraphAnalyzer::performProbGreater0(model, phiStates, psiStates);
		result.second = GraphAnalyzer::performProb1(model, phiStates, psiStates, result.first);
		result.first.complement();
        return result;
	}

	/*!
	 * Performs a backwards breadt-first search trough the underlying graph structure
	 * of the given model to determine which states of the model have a positive probability
	 * of satisfying phi until psi. The resulting states are written to the given bit vector.
	 *
     * @param model The model whose graph structure to search.
	 * @param phiStates A bit vector of all states satisfying phi.
	 * @param psiStates A bit vector of all states satisfying psi.
	 * @return A bit vector with all indices of states that have a probability greater than 0.
	 */
	template <typename T>
	static storm::storage::BitVector performProbGreater0(storm::models::AbstractDeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
        // Prepare the resulting bit vector.
        storm::storage::BitVector statesWithProbabilityGreater0(model.getNumberOfStates());
        
		// Get the backwards transition relation from the model to ease the search.
		storm::models::GraphTransitions<T> backwardTransitions(*model.getTransitionMatrix(), false);
        
		// Add all psi states as the already satisfy the condition.
		statesWithProbabilityGreater0 |= psiStates;

		// Initialize the stack used for the BFS with the states.
		std::vector<uint_fast64_t> stack;
		stack.reserve(model.getNumberOfStates());
		psiStates.addSetIndicesToVector(stack);

		// Perform the actual BFS.
		while(!stack.empty()) {
			uint_fast64_t currentState = stack.back();
			stack.pop_back();

			for(auto it = backwardTransitions.beginStateSuccessorsIterator(currentState); it != backwardTransitions.endStateSuccessorsIterator(currentState); ++it) {
				if (phiStates.get(*it) && !statesWithProbabilityGreater0.get(*it)) {
					statesWithProbabilityGreater0.set(*it, true);
					stack.push_back(*it);
				}
			}
		}
        
        // Return result.
        return statesWithProbabilityGreater0;
	}

	/*!
	 * Computes the set of states of the given model for which all paths lead to
	 * the given set of target states and only visit states from the filter set
	 * before. In order to do this, it uses the given set of states that
	 * characterizes the states that possess at least one path to a target state.
	 * The results are written to the given bit vector.
     *
	 * @param model The model whose graph structure to search.
	 * @param phiStates A bit vector of all states satisfying phi.
	 * @param psiStates A bit vector of all states satisfying psi.
	 * @param statesWithProbabilityGreater0 A reference to a bit vector of states that possess a positive
	 * probability mass of satisfying phi until psi.
	 * @return A bit vector with all indices of states that have a probability greater than 1.
	 */
	template <typename T>
	static storm::storage::BitVector performProb1(storm::models::AbstractDeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::storage::BitVector const& statesWithProbabilityGreater0) {
        storm::storage::BitVector statesWithProbability1 = GraphAnalyzer::performProbGreater0(model, ~psiStates, ~statesWithProbabilityGreater0);
		statesWithProbability1.complement();
        return statesWithProbability1;
	}

	/*!
	 * Computes the set of states of the given model for which all paths lead to
	 * the given set of target states and only visit states from the filter set
	 * before. In order to do this, it uses the given set of states that
	 * characterizes the states that possess at least one path to a target state.
	 * The results are written to the given bit vector.
     *
	 * @param model The model whose graph structure to search.
	 * @param phiStates A bit vector of all states satisfying phi.
	 * @param psiStates A bit vector of all states satisfying psi.
	 * @return A bit vector with all indices of states that have a probability greater than 1.
	 */
	template <typename T>
	static storm::storage::BitVector performProb1(storm::models::AbstractDeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
        storm::storage::BitVector statesWithProbabilityGreater0 = GraphAnalyzer::performProbGreater0(model, phiStates, psiStates);
		storm::storage::BitVector statesWithProbability1 = GraphAnalyzer::performProbGreater0(model, ~psiStates, ~(statesWithProbabilityGreater0));
		statesWithProbability1.complement();
        return statesWithProbability1;
	}

	/*!
	 * Computes the sets of states that have probability 0 or 1, respectively, of satisfying phi
     * until psi in a non-deterministic model in which all non-deterministic choices are resolved
     * such that the probability is maximized.
     *
	 * @param model The model whose graph structure to search.
	 * @param phiStates The set of all states satisfying phi.
	 * @param psiStates The set of all states satisfying psi.
	 * @return A pair of bit vectors that represent all states with probability 0 and 1, respectively.
	 */
	template <typename T>
	static std::pair<storm::storage::BitVector, storm::storage::BitVector> performProb01Max(storm::models::AbstractNondeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
		std::pair<storm::storage::BitVector, storm::storage::BitVector> result;
        result.first = GraphAnalyzer::performProb0A(model, phiStates, psiStates);
		result.second = GraphAnalyzer::performProb1E(model, phiStates, psiStates);
        return result;
	}

    /*!
	 * Computes the sets of states that have probability 0 of satisfying phi until psi under all
     * possible resolutions of non-determinism in a non-deterministic model. Stated differently,
     * this means that these states have probability 0 of satisfying phi until psi even if the
     * scheduler tries to maximize this probability.
     *
	 * @param model The model whose graph structure to search.
	 * @param phiStates The set of all states satisfying phi.
	 * @param psiStates The set of all states satisfying psi.
	 * @return A bit vector that represents all states with probability 0.
	 */
	template <typename T>
	static storm::storage::BitVector performProb0A(storm::models::AbstractNondeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
        // Prepare the resulting bit vector.
        storm::storage::BitVector statesWithProbability0(model.getNumberOfStates());
        
		// Get the backwards transition relation from the model to ease the search.
		storm::models::GraphTransitions<T> backwardTransitions(*model.getTransitionMatrix(), *model.getNondeterministicChoiceIndices(), false);

		// Add all psi states as the already satisfy the condition.
		statesWithProbability0 |= psiStates;

		// Initialize the stack used for the BFS with the states
		std::vector<uint_fast64_t> stack;
		stack.reserve(model.getNumberOfStates());
		psiStates.addSetIndicesToVector(stack);

		// Perform the actual BFS.
		while(!stack.empty()) {
			uint_fast64_t currentState = stack.back();
			stack.pop_back();

			for(auto it = backwardTransitions.beginStateSuccessorsIterator(currentState); it != backwardTransitions.endStateSuccessorsIterator(currentState); ++it) {
				if (phiStates.get(*it) && !statesWithProbability0.get(*it)) {
					statesWithProbability0.set(*it, true);
					stack.push_back(*it);
				}
			}
		}

        // Finally, invert the computed set of states and return result.
		statesWithProbability0.complement();
        return statesWithProbability0;
	}

    /*!
	 * Computes the sets of states that have probability 1 of satisfying phi until psi under at least
     * one possible resolution of non-determinism in a non-deterministic model. Stated differently,
     * this means that these states have probability 1 of satisfying phi until psi if the
     * scheduler tries to maximize this probability.
     *
	 * @param model The model whose graph structure to search.
	 * @param phiStates The set of all states satisfying phi.
	 * @param psiStates The set of all states satisfying psi.
	 * @return A bit vector that represents all states with probability 1.
	 */
	template <typename T>
	static storm::storage::BitVector performProb1E(storm::models::AbstractNondeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
        // Get some temporaries for convenience.
		std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix = model.getTransitionMatrix();
		std::shared_ptr<std::vector<uint_fast64_t>> nondeterministicChoiceIndices = model.getNondeterministicChoiceIndices();

		// Get the backwards transition relation from the model to ease the search.
		storm::models::GraphTransitions<T> backwardTransitions(*model.getTransitionMatrix(), *model.getNondeterministicChoiceIndices(), false);

		storm::storage::BitVector currentStates(model.getNumberOfStates(), true);

		std::vector<uint_fast64_t> stack;
		stack.reserve(model.getNumberOfStates());

        // Perform the loop as long as the set of states gets larger.
		bool done = false;
		while (!done) {
			stack.clear();
			storm::storage::BitVector nextStates(psiStates);
			psiStates.addSetIndicesToVector(stack);

			while (!stack.empty()) {
				uint_fast64_t currentState = stack.back();
				stack.pop_back();

				for(auto it = backwardTransitions.beginStateSuccessorsIterator(currentState); it != backwardTransitions.endStateSuccessorsIterator(currentState); ++it) {
					if (phiStates.get(*it) && !nextStates.get(*it)) {
						// Check whether the predecessor has only successors in the current state set for one of the
						// nondeterminstic choices.
						for (uint_fast64_t row = (*nondeterministicChoiceIndices)[*it]; row < (*nondeterministicChoiceIndices)[*it + 1]; ++row) {
							bool allSuccessorsInCurrentStates = true;
							for (auto colIt = transitionMatrix->constColumnIteratorBegin(row); colIt != transitionMatrix->constColumnIteratorEnd(row); ++colIt) {
								if (!currentStates.get(*colIt)) {
									allSuccessorsInCurrentStates = false;
									break;
								}
							}

							// If all successors for a given nondeterministic choice are in the current state set, we
							// add it to the set of states for the next iteration and perform a backward search from
							// that state.
							if (allSuccessorsInCurrentStates) {
								nextStates.set(*it, true);
								stack.push_back(*it);
								break;
							}
						}
					}
				}
			}

			// Check whether we need to perform an additional iteration.
			if (currentStates == nextStates) {
				done = true;
			} else {
				currentStates = std::move(nextStates);
			}
		}

        return currentStates;
	}
	/*!
	 * Computes the sets of states that have probability 0 or 1, respectively, of satisfying phi
     * until psi in a non-deterministic model in which all non-deterministic choices are resolved
     * such that the probability is minimized.
     *
	 * @param model The model whose graph structure to search.
	 * @param phiStates The set of all states satisfying phi.
	 * @param psiStates The set of all states satisfying psi.
	 * @return A pair of bit vectors that represent all states with probability 0 and 1, respectively.
	 */
    template <typename T>
	static std::pair<storm::storage::BitVector, storm::storage::BitVector> performProb01Min(storm::models::AbstractNondeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
		std::pair<storm::storage::BitVector, storm::storage::BitVector> result;
        result.first = GraphAnalyzer::performProb0E(model, phiStates, psiStates);
		result.second = GraphAnalyzer::performProb1A(model, phiStates, psiStates);
        return result;
	}

    /*!
	 * Computes the sets of states that have probability 0 of satisfying phi until psi under at least
     * one possible resolution of non-determinism in a non-deterministic model. Stated differently,
     * this means that these states have probability 0 of satisfying phi until psi if the
     * scheduler tries to minimize this probability.
     *
	 * @param model The model whose graph structure to search.
	 * @param phiStates The set of all states satisfying phi.
	 * @param psiStates The set of all states satisfying psi.
	 * @return A bit vector that represents all states with probability 0.
	 */
	template <typename T>
	static storm::storage::BitVector performProb0E(storm::models::AbstractNondeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
        // Prepare resulting bit vector.
        storm::storage::BitVector statesWithProbability0(model.getNumberOfStates());
        
		// Get some temporaries for convenience.
		std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix = model.getTransitionMatrix();
		std::shared_ptr<std::vector<uint_fast64_t>> nondeterministicChoiceIndices = model.getNondeterministicChoiceIndices();

		// Get the backwards transition relation from the model to ease the search.
		storm::models::GraphTransitions<T> backwardTransitions(*transitionMatrix, *nondeterministicChoiceIndices, false);

		// Add all psi states as the already satisfy the condition.
		statesWithProbability0 |= psiStates;

		// Initialize the stack used for the DFS with the states
		std::vector<uint_fast64_t> stack;
		stack.reserve(model.getNumberOfStates());
		psiStates.addSetIndicesToVector(stack);

		// Perform the actual DFS.
		while(!stack.empty()) {
			uint_fast64_t currentState = stack.back();
			stack.pop_back();

			for(auto it = backwardTransitions.beginStateSuccessorsIterator(currentState); it != backwardTransitions.endStateSuccessorsIterator(currentState); ++it) {
				if (phiStates.get(*it) && !statesWithProbability0.get(*it)) {
					// Check whether the predecessor has at least one successor in the current state
					// set for every nondeterministic choice.
					bool addToStatesWithProbability0 = true;
					for (auto rowIt = nondeterministicChoiceIndices->begin() + *it; rowIt != nondeterministicChoiceIndices->begin() + *it + 1; ++rowIt) {
						bool hasAtLeastOneSuccessorWithProbabilityGreater0 = false;
						for (auto colIt = transitionMatrix->constColumnIteratorBegin(*rowIt); colIt != transitionMatrix->constColumnIteratorEnd(*rowIt); ++colIt) {
							if (statesWithProbability0.get(*colIt)) {
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
						statesWithProbability0.set(*it, true);
						stack.push_back(*it);
					}
				}
			}
		}

		statesWithProbability0.complement();
        return statesWithProbability0;
	}

    /*!
	 * Computes the sets of states that have probability 1 of satisfying phi until psi under all
     * possible resolutions of non-determinism in a non-deterministic model. Stated differently,
     * this means that these states have probability 1 of satisfying phi until psi even if the
     * scheduler tries to minimize this probability.
     *
	 * @param model The model whose graph structure to search.
	 * @param phiStates The set of all states satisfying phi.
	 * @param psiStates The set of all states satisfying psi.
	 * @return A bit vector that represents all states with probability 0.
	 */
	template <typename T>
	static storm::storage::BitVector performProb1A(storm::models::AbstractNondeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
		// Get some temporaries for convenience.
		std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix = model.getTransitionMatrix();
		std::shared_ptr<std::vector<uint_fast64_t>> nondeterministicChoiceIndices = model.getNondeterministicChoiceIndices();

		// Get the backwards transition relation from the model to ease the search.
		storm::models::GraphTransitions<T> backwardTransitions(*model.getTransitionMatrix(), *model.getNondeterministicChoiceIndices(), false);

		storm::storage::BitVector currentStates(model.getNumberOfStates(), true);

		std::vector<uint_fast64_t> stack;
		stack.reserve(model.getNumberOfStates());

        // Perform the loop as long as the set of states gets smaller.
		bool done = false;
		while (!done) {
			stack.clear();
			storm::storage::BitVector nextStates(psiStates);
			psiStates.addSetIndicesToVector(stack);

			while (!stack.empty()) {
				uint_fast64_t currentState = stack.back();
				stack.pop_back();

				for(auto it = backwardTransitions.beginStateSuccessorsIterator(currentState); it != backwardTransitions.endStateSuccessorsIterator(currentState); ++it) {
					if (phiStates.get(*it) && !nextStates.get(*it)) {
						// Check whether the predecessor has only successors in the current state set for all of the
						// nondeterminstic choices.
						bool allSuccessorsInCurrentStatesForAllChoices = true;
						for (uint_fast64_t row = (*nondeterministicChoiceIndices)[*it]; row < (*nondeterministicChoiceIndices)[*it + 1]; ++row) {
							for (auto colIt = transitionMatrix->constColumnIteratorBegin(row); colIt != transitionMatrix->constColumnIteratorEnd(row); ++colIt) {
								if (!currentStates.get(*colIt)) {
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
							nextStates.set(*it, true);
							stack.push_back(*it);
						}
					}
				}
			}

			// Check whether we need to perform an additional iteration.
			if (currentStates == nextStates) {
				done = true;
			} else {
				currentStates = std::move(nextStates);
			}
		}
        return currentStates;
	}

    /*!
     * Performs a decomposition of the given nondetermistic model into its SCCs.
     *
     * @param model The nondeterminstic model to decompose.
     * @return A pair whose first component represents the SCCs and whose second component represents the dependency
     * graph of the SCCs.
     */
	template <typename T>
	static std::pair<std::vector<std::vector<uint_fast64_t>>, storm::models::GraphTransitions<T>> performSccDecomposition(storm::models::AbstractNondeterministicModel<T> const& model) {
		LOG4CPLUS_INFO(logger, "Computing SCC decomposition.");

        // Get the forward transition relation from the model to ease the search.
        std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = model.getNondeterministicChoiceIndices();
		storm::models::GraphTransitions<T> forwardTransitions(model.getTransitionMatrix(), nondeterministicChoiceIndices, true);

		// Perform the actual SCC decomposition based on the graph-transitions of the system.
		std::pair<std::vector<std::vector<uint_fast64_t>>, storm::models::GraphTransitions<T>> result = performSccDecomposition(nondeterministicChoiceIndices.size() - 1, forwardTransitions);
		LOG4CPLUS_INFO(logger, "Done computing SCC decomposition.");
        
        return result;
	}

    /*!
     * Performs a topological sort of the states of the system according to the given transitions.
     *
     * @param transitions The transitions of the graph structure.
     * @return A vector of indices that is a topological sort of the states.
     */
	template <typename T>
	static std::vector<uint_fast64_t> getTopologicalSort(storm::models::GraphTransitions<T> const& transitions) {
        std::vector<uint_fast64_t> topologicalSort;
		topologicalSort.reserve(transitions.getNumberOfStates());

        // Prepare the stacks needed for recursion.
		std::vector<uint_fast64_t> recursionStack;
		recursionStack.reserve(transitions.getNumberOfStates());
		std::vector<typename storm::models::GraphTransitions<T>::stateSuccessorIterator> iteratorRecursionStack;
		iteratorRecursionStack.reserve(transitions.getNumberOfStates());

        // Perform a depth-first search over the given transitions and record states in the reverse order they were visited.
		storm::storage::BitVector visitedStates(transitions.getNumberOfStates());
		for (uint_fast64_t state = 0; state < transitions.getNumberOfStates(); ++state) {
			if (!visitedStates.get(state)) {
				recursionStack.push_back(state);
				iteratorRecursionStack.push_back(transitions.beginStateSuccessorsIterator(state));

				recursionStepForward:
				while (!recursionStack.empty()) {
					uint_fast64_t currentState = recursionStack.back();
					typename storm::models::GraphTransitions<T>::stateSuccessorIterator currentIt = iteratorRecursionStack.back();

					visitedStates.set(currentState, true);

					recursionStepBackward:
					for (; currentIt != transitions.endStateSuccessorsIterator(currentState); ++currentIt) {
						if (!visitedStates.get(*currentIt)) {
							// Put unvisited successor on top of our recursion stack and remember that.
							recursionStack.push_back(*currentIt);

							// Save current iterator position so we can continue where we left off later.
							iteratorRecursionStack.pop_back();
							iteratorRecursionStack.push_back(currentIt + 1);

							// Also, put initial value for iterator on corresponding recursion stack.
							iteratorRecursionStack.push_back(transitions.beginStateSuccessorsIterator(*currentIt));

							goto recursionStepForward;
						}
					}

					topologicalSort.push_back(currentState);

					// If we reach this point, we have completed the recursive descent for the current state.
					// That is, we need to pop it from the recursion stacks.
					recursionStack.pop_back();
					iteratorRecursionStack.pop_back();

					// If there is at least one state under the current one in our recursion stack, we need
					// to restore the topmost state as the current state and jump to the part after the
					// original recursive call.
					if (recursionStack.size() > 0) {
						currentState = recursionStack.back();
						currentIt = iteratorRecursionStack.back();

						goto recursionStepBackward;
					}
				}
			}
		}
        
        return topologicalSort;
	}

private:
    /*!
     * Performs an SCC decomposition of the system given by its forward transitions.
     *
     * @param forwardTransitions The (forward) transition relation of the model to decompose.
     * @return A pair whose first component represents the SCCs and whose second component represents the dependency
     * graph of the SCCs.
     */
	template <typename T>
	static std::pair<std::vector<std::vector<uint_fast64_t>>, storm::models::GraphTransitions<T>> performSccDecomposition(storm::models::GraphTransitions<T> const& forwardTransitions) {
        std::pair<std::vector<std::vector<uint_fast64_t>>, storm::models::GraphTransitions<T>> sccDecomposition;
        uint_fast64_t numberOfStates = forwardTransitions.getNumberOfStates();
        
        // Set up the environment of Tarjan's algorithm.
		std::vector<uint_fast64_t> tarjanStack;
		tarjanStack.reserve(numberOfStates);
		storm::storage::BitVector tarjanStackStates(numberOfStates);
		std::vector<uint_fast64_t> stateIndices(numberOfStates);
		std::vector<uint_fast64_t> lowlinks(numberOfStates);
		storm::storage::BitVector visitedStates(numberOfStates);
		std::map<uint_fast64_t, uint_fast64_t> stateToSccMap;

        // Start the search for SCCs from every vertex in the graph structure, because there is.
		uint_fast64_t currentIndex = 0;
		for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
			if (!visitedStates.get(state)) {
				performSccDecompositionHelper(state, currentIndex, stateIndices, lowlinks, tarjanStack, tarjanStackStates, visitedStates, forwardTransitions, sccDecomposition.first, stateToSccMap);
			}
		}

        // Finally, determine the dependency graph over the SCCs and return result.
		sccDecomposition.second = storm::models::GraphTransitions<T>(forwardTransitions, sccDecomposition.first, stateToSccMap);
        return sccDecomposition;
	}

    /*!
     * Performs an SCC decomposition using Tarjan's algorithm.
     *
     * @param startState The state at which the search is started.
     * @param currentIndex The index that is to be used as the next free index.
     * @param stateIndices The vector that stores the index for each state.
     * @param lowlinks A vector that stores the lowlink of each state, i.e. the lowest index of a state reachable from
     * a particular state.
     * @param tarjanStack A stack used for Tarjan's algorithm.
     * @param tarjanStackStates A bit vector that represents all states that are currently contained in tarjanStack.
     * @param visitedStates A bit vector that stores all states that have already been visited.
     * @param forwardTransitions The (forward) transition relation of the graph structure.
     * @param stronglyConnectedComponents A vector of strongly connected components to which newly found SCCs are added.
     * @param stateToSccMap A mapping from state indices to SCC indices that maps each state to its SCC.
     */
	template <typename T>
	static void performSccDecompositionHelper(uint_fast64_t startState, uint_fast64_t& currentIndex, std::vector<uint_fast64_t>& stateIndices, std::vector<uint_fast64_t>& lowlinks, std::vector<uint_fast64_t>& tarjanStack, storm::storage::BitVector& tarjanStackStates, storm::storage::BitVector& visitedStates, storm::models::GraphTransitions<T> const& forwardTransitions, std::vector<std::vector<uint_fast64_t>>& stronglyConnectedComponents, std::map<uint_fast64_t, uint_fast64_t>& stateToSccMap) {
		// Create the stacks needed for turning the recursive formulation of Tarjan's algorithm
		// into an iterative version. In particular, we keep one stack for states and one stack
		// for the iterators. The last one is not strictly needed, but reduces iteration work when
		// all successors of a particular state are considered.
		std::vector<uint_fast64_t> recursionStateStack;
		recursionStateStack.reserve(lowlinks.size());
		std::vector<typename storm::models::GraphTransitions<T>::stateSuccessorIterator> recursionIteratorStack;
		recursionIteratorStack.reserve(lowlinks.size());
		std::vector<bool> statesInStack(lowlinks.size());

		// Initialize the recursion stacks with the given initial state (and its successor iterator).
		recursionStateStack.push_back(startState);
		recursionIteratorStack.push_back(forwardTransitions.beginStateSuccessorsIterator(startState));

		recursionStepForward:
		while (!recursionStateStack.empty()) {
			uint_fast64_t currentState = recursionStateStack.back();
			typename storm::models::GraphTransitions<T>::stateSuccessorIterator currentIt = recursionIteratorStack.back();

			// Perform the treatment of newly discovered state as defined by Tarjan's algorithm
			visitedStates.set(currentState, true);
			stateIndices[currentState] = currentIndex;
			lowlinks[currentState] = currentIndex;
			++currentIndex;
			tarjanStack.push_back(currentState);
			tarjanStackStates.set(currentState, true);

			// Now, traverse all successors of the current state.
			for(; currentIt != forwardTransitions.endStateSuccessorsIterator(currentState); ++currentIt) {
				// If we have not visited the successor already, we need to perform the procedure
				// recursively on the newly found state.
				if (!visitedStates.get(*currentIt)) {
					// Save current iterator position so we can continue where we left off later.
					recursionIteratorStack.pop_back();
					recursionIteratorStack.push_back(currentIt);

					// Put unvisited successor on top of our recursion stack and remember that.
					recursionStateStack.push_back(*currentIt);
					statesInStack[*currentIt] = true;

					// Also, put initial value for iterator on corresponding recursion stack.
					recursionIteratorStack.push_back(forwardTransitions.beginStateSuccessorsIterator(*currentIt));

					// Perform the actual recursion step in an iterative way.
					goto recursionStepForward;

					recursionStepBackward:
					lowlinks[currentState] = std::min(lowlinks[currentState], lowlinks[*currentIt]);
				} else if (tarjanStackStates.get(*currentIt)) {
					// Update the lowlink of the current state.
					lowlinks[currentState] = std::min(lowlinks[currentState], stateIndices[*currentIt]);
				}
			}

			// If the current state is the root of a SCC, we need to pop all states of the SCC from
			// the algorithm's stack.
			if (lowlinks[currentState] == stateIndices[currentState]) {
				stronglyConnectedComponents.push_back(std::vector<uint_fast64_t>());

				uint_fast64_t lastState = 0;
				do {
					// Pop topmost state from the algorithm's stack.
					lastState = tarjanStack.back();
					tarjanStack.pop_back();
					tarjanStackStates.set(lastState, false);

					// Add the state to the current SCC.
					stronglyConnectedComponents.back().push_back(lastState);
					stateToSccMap[lastState] = stronglyConnectedComponents.size() - 1;
				} while (lastState != currentState);
			}

			// If we reach this point, we have completed the recursive descent for the current state.
			// That is, we need to pop it from the recursion stacks.
			recursionStateStack.pop_back();
			recursionIteratorStack.pop_back();

			// If there is at least one state under the current one in our recursion stack, we need
			// to restore the topmost state as the current state and jump to the part after the
			// original recursive call.
			if (recursionStateStack.size() > 0) {
				currentState = recursionStateStack.back();
				currentIt = recursionIteratorStack.back();

				goto recursionStepBackward;
			}
		}
	}
};

} // namespace utility

} // namespace storm

#endif /* STORM_UTILITY_GRAPHANALYZER_H_ */
