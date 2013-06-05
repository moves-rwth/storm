/*
 * graph.h
 *
 *  Created on: 28.11.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_UTILITY_GRAPH_H_
#define STORM_UTILITY_GRAPH_H_

#include "src/models/AbstractDeterministicModel.h"
#include "src/models/AbstractNondeterministicModel.h"
#include "src/exceptions/InvalidArgumentException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {

namespace utility {
    
namespace graph {

	/*!
	 * Performs a backwards breadt-first search trough the underlying graph structure
	 * of the given model to determine which states of the model have a positive probability
	 * of satisfying phi until psi. The resulting states are written to the given bit vector.
	 *
     * @param model The model whose graph structure to search.
	 * @param phiStates A bit vector of all states satisfying phi.
	 * @param psiStates A bit vector of all states satisfying psi.
     * @param useStepBound A flag that indicates whether or not to use the given number of maximal steps for the search.
     * @param maximalSteps The maximal number of steps to reach the psi states. 
	 * @return A bit vector with all indices of states that have a probability greater than 0.
	 */
	template <typename T>
	storm::storage::BitVector performProbGreater0(storm::models::AbstractDeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool useStepBound = false, uint_fast64_t maximalSteps = 0) {
        // Prepare the resulting bit vector.
        storm::storage::BitVector statesWithProbabilityGreater0(model.getNumberOfStates());
        
		// Get the backwards transition relation from the model to ease the search.
        storm::storage::SparseMatrix<bool> backwardTransitions = model.getBackwardTransitions();
        
		// Add all psi states as the already satisfy the condition.
		statesWithProbabilityGreater0 |= psiStates;

		// Initialize the stack used for the DFS with the states.
		std::vector<uint_fast64_t> stack = psiStates.getSetIndicesList();
        
        // Initialize the stack for the step bound, if the number of steps is bounded.
        std::vector<uint_fast64_t> stepStack;
        std::vector<uint_fast64_t> remainingSteps;
        if (useStepBound) {
            stepStack.reserve(model.getNumberOfStates());
            stepStack.insert(stepStack.begin(), psiStates.getNumberOfSetBits(), maximalSteps);
            remainingSteps.resize(model.getNumberOfStates());
            for (auto state : psiStates) {
                remainingSteps[state] = maximalSteps;
            }
        }

		// Perform the actual DFS.
        uint_fast64_t currentState, currentStepBound;
		while (!stack.empty()) {
            currentState = stack.back();
			stack.pop_back();
            
            if (useStepBound) {
                currentStepBound = stepStack.back();
                stepStack.pop_back();
            }

			for (auto predecessorIt = backwardTransitions.constColumnIteratorBegin(currentState), predecessorIte = backwardTransitions.constColumnIteratorEnd(currentState); predecessorIt != predecessorIte; ++predecessorIt) {
				if (phiStates.get(*predecessorIt) && (!statesWithProbabilityGreater0.get(*predecessorIt) || (useStepBound && remainingSteps[*predecessorIt] < currentStepBound - 1))) {
                    // If we don't have a bound on the number of steps to take, just add the state to the stack.
                    if (!useStepBound) {
                        statesWithProbabilityGreater0.set(*predecessorIt, true);
                        stack.push_back(*predecessorIt);
                    } else if (currentStepBound > 0) {
                        // If there is at least one more step to go, we need to push the state and the new number of steps.
                        remainingSteps[*predecessorIt] = currentStepBound - 1;
                        statesWithProbabilityGreater0.set(*predecessorIt, true);
                        stack.push_back(*predecessorIt);
                        stepStack.push_back(currentStepBound - 1);
                    }
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
	storm::storage::BitVector performProb1(storm::models::AbstractDeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::storage::BitVector const& statesWithProbabilityGreater0) {
        storm::storage::BitVector statesWithProbability1 = performProbGreater0(model, ~psiStates, ~statesWithProbabilityGreater0);
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
	storm::storage::BitVector performProb1(storm::models::AbstractDeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
        storm::storage::BitVector statesWithProbabilityGreater0 = performProbGreater0(model, phiStates, psiStates);
		storm::storage::BitVector statesWithProbability1 = performProbGreater0(model, ~psiStates, ~(statesWithProbabilityGreater0));
		statesWithProbability1.complement();
        return statesWithProbability1;
	}

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
		result.first = performProbGreater0(model, phiStates, psiStates);
		result.second = performProb1(model, phiStates, psiStates, result.first);
		result.first.complement();
        return result;
	}

    /*!
	 * Computes the sets of states that have probability greater 0 of satisfying phi until psi under at least
     * one possible resolution of non-determinism in a non-deterministic model. Stated differently,
     * this means that these states have a probability greater 0 of satisfying phi until psi if the
     * scheduler tries to minimize this probability.
     *
	 * @param model The model whose graph structure to search.
     * @param backwardTransitions The reversed transition relation of the model.
	 * @param phiStates The set of all states satisfying phi.
	 * @param psiStates The set of all states satisfying psi.
     * @param useStepBound A flag that indicates whether or not to use the given number of maximal steps for the search.
     * @param maximalSteps The maximal number of steps to reach the psi states.
	 * @return A bit vector that represents all states with probability 0.
	 */
	template <typename T>
	storm::storage::BitVector performProbGreater0E(storm::models::AbstractNondeterministicModel<T> const& model, storm::storage::SparseMatrix<bool> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool useStepBound = false, uint_fast64_t maximalSteps = 0) {
        // Prepare resulting bit vector.
        storm::storage::BitVector statesWithProbabilityGreater0(model.getNumberOfStates());
        
		// Add all psi states as the already satisfy the condition.
		statesWithProbabilityGreater0 |= psiStates;
        
		// Initialize the stack used for the DFS with the states
		std::vector<uint_fast64_t> stack = psiStates.getSetIndicesList();
        
        // Initialize the stack for the step bound, if the number of steps is bounded.
        std::vector<uint_fast64_t> stepStack;
        std::vector<uint_fast64_t> remainingSteps;
        if (useStepBound) {
            stepStack.reserve(model.getNumberOfStates());
            stepStack.insert(stepStack.begin(), psiStates.getNumberOfSetBits(), maximalSteps);
            remainingSteps.resize(model.getNumberOfStates());
            for (auto state : psiStates) {
                remainingSteps[state] = maximalSteps;
            }
        }
        
		// Perform the actual DFS.
        uint_fast64_t currentState, currentStepBound;
		while (!stack.empty()) {
			currentState = stack.back();
			stack.pop_back();
            
            if (useStepBound) {
                currentStepBound = stepStack.back();
                stepStack.pop_back();
            }
            
			for (auto predecessorIt = backwardTransitions.constColumnIteratorBegin(currentState), predecessorIte = backwardTransitions.constColumnIteratorEnd(currentState); predecessorIt != predecessorIte; ++predecessorIt) {
                if (phiStates.get(*predecessorIt) && (!statesWithProbabilityGreater0.get(*predecessorIt) || (useStepBound && remainingSteps[*predecessorIt] < currentStepBound - 1))) {
                    // If we don't have a bound on the number of steps to take, just add the state to the stack.
                    if (!useStepBound) {
                        statesWithProbabilityGreater0.set(*predecessorIt, true);
                        stack.push_back(*predecessorIt);
                    } else if (currentStepBound > 0) {
                        // If there is at least one more step to go, we need to push the state and the new number of steps.
                        remainingSteps[*predecessorIt] = currentStepBound - 1;
                        statesWithProbabilityGreater0.set(*predecessorIt, true);
                        stack.push_back(*predecessorIt);
                        stepStack.push_back(currentStepBound - 1);
                    }
                }
			}
		}
        
        return statesWithProbabilityGreater0;
	}
    
    /*!
	 * Computes the sets of states that have probability 0 of satisfying phi until psi under all
     * possible resolutions of non-determinism in a non-deterministic model. Stated differently,
     * this means that these states have probability 0 of satisfying phi until psi even if the
     * scheduler tries to maximize this probability.
     *
	 * @param model The model whose graph structure to search.
     * @param backwardTransitions The reversed transition relation of the model.
	 * @param phiStates The set of all states satisfying phi.
	 * @param psiStates The set of all states satisfying psi.
     * @param useStepBound A flag that indicates whether or not to use the given number of maximal steps for the search.
     * @param maximalSteps The maximal number of steps to reach the psi states. 
	 * @return A bit vector that represents all states with probability 0.
	 */
	template <typename T>
	storm::storage::BitVector performProb0A(storm::models::AbstractNondeterministicModel<T> const& model, storm::storage::SparseMatrix<bool> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
        storm::storage::BitVector statesWithProbability0 = performProbGreater0E(model, backwardTransitions, phiStates, psiStates);
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
     * @param backwardTransitions The reversed transition relation of the model.
	 * @param phiStates The set of all states satisfying phi.
	 * @param psiStates The set of all states satisfying psi.
	 * @return A bit vector that represents all states with probability 1.
	 */
	template <typename T>
	storm::storage::BitVector performProb1E(storm::models::AbstractNondeterministicModel<T> const& model, storm::storage::SparseMatrix<bool> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
        // Get some temporaries for convenience.
        storm::storage::SparseMatrix<T> const& transitionMatrix = model.getTransitionMatrix();
		std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = model.getNondeterministicChoiceIndices();

        // Initialize the environment for the iterative algorithm.
		storm::storage::BitVector currentStates(model.getNumberOfStates(), true);
		std::vector<uint_fast64_t> stack;
		stack.reserve(model.getNumberOfStates());

        // Perform the loop as long as the set of states gets larger.
		bool done = false;
        uint_fast64_t currentState;
		while (!done) {
			stack.clear();
			storm::storage::BitVector nextStates(psiStates);
			psiStates.addSetIndicesToVector(stack);

			while (!stack.empty()) {
				currentState = stack.back();
				stack.pop_back();

				for(auto predecessorIt = backwardTransitions.constColumnIteratorBegin(currentState), predecessorIte = backwardTransitions.constColumnIteratorEnd(currentState); predecessorIt != predecessorIte; ++predecessorIt) {
					if (phiStates.get(*predecessorIt) && !nextStates.get(*predecessorIt)) {
						// Check whether the predecessor has only successors in the current state set for one of the
						// nondeterminstic choices.
						for (auto row = nondeterministicChoiceIndices[*predecessorIt]; row < nondeterministicChoiceIndices[*predecessorIt + 1]; ++row) {
							bool allSuccessorsInCurrentStates = true;
							for (auto targetIt = transitionMatrix.constColumnIteratorBegin(row), targetIte = transitionMatrix.constColumnIteratorEnd(row); targetIt != targetIte; ++targetIt) {
								if (!currentStates.get(*targetIt)) {
									allSuccessorsInCurrentStates = false;
									break;
								}
							}

							// If all successors for a given nondeterministic choice are in the current state set, we
							// add it to the set of states for the next iteration and perform a backward search from
							// that state.
							if (allSuccessorsInCurrentStates) {
								nextStates.set(*predecessorIt, true);
								stack.push_back(*predecessorIt);
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
     * such that the probability is maximized.
     *
	 * @param model The model whose graph structure to search.
	 * @param phiStates The set of all states satisfying phi.
	 * @param psiStates The set of all states satisfying psi.
	 * @return A pair of bit vectors that represent all states with probability 0 and 1, respectively.
	 */
	template <typename T>
	std::pair<storm::storage::BitVector, storm::storage::BitVector> performProb01Max(storm::models::AbstractNondeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
		std::pair<storm::storage::BitVector, storm::storage::BitVector> result;
        
        // Get the backwards transition relation from the model to ease the search.
		storm::storage::SparseMatrix<bool> backwardTransitions = model.getBackwardTransitions();
        
        result.first = performProb0A(model, backwardTransitions, phiStates, psiStates);
		result.second = performProb1E(model, backwardTransitions, phiStates, psiStates);
        return result;
	}

    /*!
	 * Computes the sets of states that have probability greater 0 of satisfying phi until psi under any
     * possible resolution of non-determinism in a non-deterministic model. Stated differently,
     * this means that these states have a probability greater 0 of satisfying phi until psi if the
     * scheduler tries to maximize this probability.
     *
	 * @param model The model whose graph structure to search.
     * @param backwardTransitions The reversed transition relation of the model.
	 * @param phiStates The set of all states satisfying phi.
	 * @param psiStates The set of all states satisfying psi.
     * @param useStepBound A flag that indicates whether or not to use the given number of maximal steps for the search.
     * @param maximalSteps The maximal number of steps to reach the psi states.
	 * @return A bit vector that represents all states with probability 0.
	 */
	template <typename T>
	storm::storage::BitVector performProbGreater0A(storm::models::AbstractNondeterministicModel<T> const& model, storm::storage::SparseMatrix<bool> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool useStepBound = false, uint_fast64_t maximalSteps = 0) {
        // Prepare resulting bit vector.
        storm::storage::BitVector statesWithProbabilityGreater0(model.getNumberOfStates());
        
		// Get some temporaries for convenience.
        storm::storage::SparseMatrix<T> const& transitionMatrix = model.getTransitionMatrix();
		std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = model.getNondeterministicChoiceIndices();
        
		// Add all psi states as the already satisfy the condition.
		statesWithProbabilityGreater0 |= psiStates;
        
		// Initialize the stack used for the DFS with the states
		std::vector<uint_fast64_t> stack = psiStates.getSetIndicesList();
        
        // Initialize the stack for the step bound, if the number of steps is bounded.
        std::vector<uint_fast64_t> stepStack;
        std::vector<uint_fast64_t> remainingSteps;
        if (useStepBound) {
            stepStack.reserve(model.getNumberOfStates());
            stepStack.insert(stepStack.begin(), psiStates.getNumberOfSetBits(), maximalSteps);
            remainingSteps.resize(model.getNumberOfStates());
            for (auto state : psiStates) {
                remainingSteps[state] = maximalSteps;
            }
        }
        
		// Perform the actual DFS.
        uint_fast64_t currentState, currentStepBound;
		while(!stack.empty()) {
			currentState = stack.back();
			stack.pop_back();
            
            if (useStepBound) {
                currentStepBound = stepStack.back();
                stepStack.pop_back();
            }
            
			for(auto predecessorIt = backwardTransitions.constColumnIteratorBegin(currentState), predecessorIte = backwardTransitions.constColumnIteratorEnd(currentState); predecessorIt != predecessorIte; ++predecessorIt) {
                if (phiStates.get(*predecessorIt) && (!statesWithProbabilityGreater0.get(*predecessorIt) || (useStepBound && remainingSteps[*predecessorIt] < currentStepBound - 1))) {
                    // Check whether the predecessor has at least one successor in the current state set for every
                    // nondeterministic choice.
                    bool addToStatesWithProbabilityGreater0 = true;
                    for (auto row = nondeterministicChoiceIndices[*predecessorIt]; row < nondeterministicChoiceIndices[*predecessorIt + 1]; ++row) {
                        bool hasAtLeastOneSuccessorWithProbabilityGreater0 = false;
                        for (auto successorIt = transitionMatrix.constColumnIteratorBegin(row), successorIte = transitionMatrix.constColumnIteratorEnd(row); successorIt != successorIte; ++successorIt) {
                            if (statesWithProbabilityGreater0.get(*successorIt)) {
                                hasAtLeastOneSuccessorWithProbabilityGreater0 = true;
                                break;
                            }
                        }
                        
                        if (!hasAtLeastOneSuccessorWithProbabilityGreater0) {
                            addToStatesWithProbabilityGreater0 = false;
                            break;
                        }
                    }
                    
                    // If we need to add the state, then actually add it and perform further search from the state.
                    if (addToStatesWithProbabilityGreater0) {
                        // If we don't have a bound on the number of steps to take, just add the state to the stack.
                        if (!useStepBound) {
                            statesWithProbabilityGreater0.set(*predecessorIt, true);
                            stack.push_back(*predecessorIt);
                        } else if (currentStepBound > 0) {
                            // If there is at least one more step to go, we need to push the state and the new number of steps.
                            remainingSteps[*predecessorIt] = currentStepBound - 1;
                            statesWithProbabilityGreater0.set(*predecessorIt, true);
                            stack.push_back(*predecessorIt);
                            stepStack.push_back(currentStepBound - 1);
                        }
                    }
                }
			}
		}
        
        return statesWithProbabilityGreater0;
	}
    
    /*!
	 * Computes the sets of states that have probability 0 of satisfying phi until psi under at least
     * one possible resolution of non-determinism in a non-deterministic model. Stated differently,
     * this means that these states have probability 0 of satisfying phi until psi if the
     * scheduler tries to minimize this probability.
     *
	 * @param model The model whose graph structure to search.
     * @param backwardTransitions The reversed transition relation of the model.
	 * @param phiStates The set of all states satisfying phi.
	 * @param psiStates The set of all states satisfying psi.
	 * @return A bit vector that represents all states with probability 0.
	 */
	template <typename T>
	storm::storage::BitVector performProb0E(storm::models::AbstractNondeterministicModel<T> const& model, storm::storage::SparseMatrix<bool> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
        storm::storage::BitVector statesWithProbability0 = performProbGreater0A(model, backwardTransitions, phiStates, psiStates);
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
     * @param backwardTransitions The reversed transition relation of the model.
	 * @param phiStates The set of all states satisfying phi.
	 * @param psiStates The set of all states satisfying psi.
	 * @return A bit vector that represents all states with probability 0.
	 */
	template <typename T>
	storm::storage::BitVector performProb1A(storm::models::AbstractNondeterministicModel<T> const& model, storm::storage::SparseMatrix<bool> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
		// Get some temporaries for convenience.
        storm::storage::SparseMatrix<T> const& transitionMatrix = model.getTransitionMatrix();
		std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = model.getNondeterministicChoiceIndices();

        // Initialize the environment for the iterative algorithm.
		storm::storage::BitVector currentStates(model.getNumberOfStates(), true);
		std::vector<uint_fast64_t> stack;
		stack.reserve(model.getNumberOfStates());

        // Perform the loop as long as the set of states gets smaller.
		bool done = false;
        uint_fast64_t currentState;
		while (!done) {
			stack.clear();
			storm::storage::BitVector nextStates(psiStates);
			psiStates.addSetIndicesToVector(stack);

			while (!stack.empty()) {
				currentState = stack.back();
				stack.pop_back();

				for(auto predecessorIt = backwardTransitions.constColumnIteratorBegin(currentState), predecessorIte = backwardTransitions.constColumnIteratorEnd(currentState); predecessorIt != predecessorIte; ++predecessorIt) {
					if (phiStates.get(*predecessorIt) && !nextStates.get(*predecessorIt)) {
						// Check whether the predecessor has only successors in the current state set for all of the
						// nondeterminstic choices.
						bool allSuccessorsInCurrentStatesForAllChoices = true;
						for (auto row = nondeterministicChoiceIndices[*predecessorIt]; row < nondeterministicChoiceIndices[*predecessorIt + 1]; ++row) {
							for (auto successorIt = transitionMatrix.constColumnIteratorBegin(row), successorIte = transitionMatrix.constColumnIteratorEnd(row); successorIt != successorIte; ++successorIt) {
								if (!currentStates.get(*successorIt)) {
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
							nextStates.set(*predecessorIt, true);
							stack.push_back(*predecessorIt);
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
	std::pair<storm::storage::BitVector, storm::storage::BitVector> performProb01Min(storm::models::AbstractNondeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
		std::pair<storm::storage::BitVector, storm::storage::BitVector> result;
        
        // Get the backwards transition relation from the model to ease the search.
		storm::storage::SparseMatrix<bool> backwardTransitions = model.getBackwardTransitions();
        
        result.first = performProb0E(model, backwardTransitions, phiStates, psiStates);
		result.second = performProb1A(model, backwardTransitions, phiStates, psiStates);
        return result;
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
     * @param matrix The transition matrix representing the graph structure.
     * @param stronglyConnectedComponents A vector of strongly connected components to which newly found SCCs are added.
     */
	template <typename T>
	void performSccDecompositionHelper(uint_fast64_t startState, uint_fast64_t& currentIndex, std::vector<uint_fast64_t>& stateIndices, std::vector<uint_fast64_t>& lowlinks, std::vector<uint_fast64_t>& tarjanStack, storm::storage::BitVector& tarjanStackStates, storm::storage::BitVector& visitedStates, storm::storage::SparseMatrix<T> const& matrix, std::vector<std::vector<uint_fast64_t>>& stronglyConnectedComponents) {
		// Create the stacks needed for turning the recursive formulation of Tarjan's algorithm
		// into an iterative version. In particular, we keep one stack for states and one stack
		// for the iterators. The last one is not strictly needed, but reduces iteration work when
		// all successors of a particular state are considered.
		std::vector<uint_fast64_t> recursionStateStack;
		recursionStateStack.reserve(lowlinks.size());
		std::vector<typename storm::storage::SparseMatrix<T>::ConstIndexIterator> recursionIteratorStack;
		recursionIteratorStack.reserve(lowlinks.size());
		std::vector<bool> statesInStack(lowlinks.size());
        
		// Initialize the recursion stacks with the given initial state (and its successor iterator).
		recursionStateStack.push_back(startState);
		recursionIteratorStack.push_back(matrix.constColumnIteratorBegin(startState));
        
    recursionStepForward:
		while (!recursionStateStack.empty()) {
			uint_fast64_t currentState = recursionStateStack.back();
			typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = recursionIteratorStack.back();
            
			// Perform the treatment of newly discovered state as defined by Tarjan's algorithm
			visitedStates.set(currentState, true);
			stateIndices[currentState] = currentIndex;
			lowlinks[currentState] = currentIndex;
			++currentIndex;
			tarjanStack.push_back(currentState);
			tarjanStackStates.set(currentState, true);
            
			// Now, traverse all successors of the current state.
			for(; successorIt != matrix.constColumnIteratorEnd(currentState); ++successorIt) {
				// If we have not visited the successor already, we need to perform the procedure
				// recursively on the newly found state.
				if (!visitedStates.get(*successorIt)) {
					// Save current iterator position so we can continue where we left off later.
					recursionIteratorStack.pop_back();
					recursionIteratorStack.push_back(successorIt);
                    
					// Put unvisited successor on top of our recursion stack and remember that.
					recursionStateStack.push_back(*successorIt);
					statesInStack[*successorIt] = true;
                    
					// Also, put initial value for iterator on corresponding recursion stack.
					recursionIteratorStack.push_back(matrix.constColumnIteratorBegin(*successorIt));
                    
					// Perform the actual recursion step in an iterative way.
					goto recursionStepForward;
                    
                recursionStepBackward:
					lowlinks[currentState] = std::min(lowlinks[currentState], lowlinks[*successorIt]);
				} else if (tarjanStackStates.get(*successorIt)) {
					// Update the lowlink of the current state.
					lowlinks[currentState] = std::min(lowlinks[currentState], stateIndices[*successorIt]);
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
				successorIt = recursionIteratorStack.back();
                
				goto recursionStepBackward;
			}
		}
	}
    
    /*!
     * Performs a decomposition of the given model into its SCCs.
     *
     * @param model The nondeterminstic model to decompose.
     * @return A vector of SCCs of the model.
     */
	template <typename T>
	std::vector<std::vector<uint_fast64_t>> performSccDecomposition(storm::models::AbstractModel<T> const& model) {
		LOG4CPLUS_INFO(logger, "Computing SCC decomposition.");
        
        std::vector<std::vector<uint_fast64_t>> scc;
        uint_fast64_t numberOfStates = model.getNumberOfStates();
            
        // Set up the environment of Tarjan's algorithm.
        std::vector<uint_fast64_t> tarjanStack;
        tarjanStack.reserve(numberOfStates);
        storm::storage::BitVector tarjanStackStates(numberOfStates);
        std::vector<uint_fast64_t> stateIndices(numberOfStates);
        std::vector<uint_fast64_t> lowlinks(numberOfStates);
        storm::storage::BitVector visitedStates(numberOfStates);
            
        // Start the search for SCCs from every vertex in the graph structure, because there is.
        uint_fast64_t currentIndex = 0;
        for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
            if (!visitedStates.get(state)) {
                performSccDecompositionHelper(state, currentIndex, stateIndices, lowlinks, tarjanStack, tarjanStackStates, visitedStates, model.getTransitionMatrix(), scc);
            }
        }

		LOG4CPLUS_INFO(logger, "Done computing SCC decomposition.");
        return scc;
	}

    /*!
     * Performs a topological sort of the states of the system according to the given transitions.
     *
     * @param matrix A square matrix representing the transition relation of the system.
     * @return A vector of indices that is a topological sort of the states.
     */
	template <typename T>
	std::vector<uint_fast64_t> getTopologicalSort(storm::storage::SparseMatrix<T> const& matrix) {
        if (matrix.getRowCount() != matrix.getColumnCount()) {
            LOG4CPLUS_ERROR(logger, "Provided matrix is required to be square.");
            throw storm::exceptions::InvalidArgumentException() << "Provided matrix is required to be square.";
        }
        
        uint_fast64_t numberOfStates = matrix.getRowCount();
        
        // Prepare the result. This relies on the matrix being square.
        std::vector<uint_fast64_t> topologicalSort;
		topologicalSort.reserve(numberOfStates);

        // Prepare the stacks needed for recursion.
		std::vector<uint_fast64_t> recursionStack;
		recursionStack.reserve(matrix.getRowCount());
		std::vector<typename storm::storage::SparseMatrix<T>::ConstIndexIterator> iteratorRecursionStack;
		iteratorRecursionStack.reserve(numberOfStates);

        // Perform a depth-first search over the given transitions and record states in the reverse order they were visited.
		storm::storage::BitVector visitedStates(numberOfStates);
		for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
			if (!visitedStates.get(state)) {
				recursionStack.push_back(state);
				iteratorRecursionStack.push_back(matrix.constColumnIteratorBegin(state));

				recursionStepForward:
				while (!recursionStack.empty()) {
					uint_fast64_t currentState = recursionStack.back();
					typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = iteratorRecursionStack.back();

					visitedStates.set(currentState, true);

					recursionStepBackward:
					for (; successorIt != matrix.constColumnIteratorEnd(currentState); ++successorIt) {
						if (!visitedStates.get(*successorIt)) {
							// Put unvisited successor on top of our recursion stack and remember that.
							recursionStack.push_back(*successorIt);

							// Save current iterator position so we can continue where we left off later.
							iteratorRecursionStack.pop_back();
							iteratorRecursionStack.push_back(successorIt + 1);

							// Also, put initial value for iterator on corresponding recursion stack.
							iteratorRecursionStack.push_back(matrix.constColumnIteratorBegin(*successorIt));

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
						successorIt = iteratorRecursionStack.back();

						goto recursionStepBackward;
					}
				}
			}
		}
        
        return topologicalSort;
	}
    
} // namespace graph

} // namespace utility

} // namespace storm

#endif /* STORM_UTILITY_GRAPH_H_ */
