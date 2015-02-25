#ifndef CUDA_KERNELS_GRAPH_H
#define	CUDA_KERNELS_GRAPH_H

#include <set>
#include <limits>

#include "src/storage/sparse/StateType.h"
#include "src/models/AbstractDeterministicModel.h"
#include "src/models/AbstractNondeterministicModel.h"
#include "src/utility/constants.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace stormcuda {
    namespace graph {
        void helloWorld();
        template <typename T>
        storm::storage::BitVector performProbGreater0(storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool useStepBound = false, uint_fast64_t maximalSteps = 0) {
            helloWorld();
            // Prepare the resulting bit vector.
            uint_fast64_t numberOfStates = phiStates.size();
            storm::storage::BitVector statesWithProbabilityGreater0(numberOfStates);

            // Add all psi states as the already satisfy the condition.
            statesWithProbabilityGreater0 |= psiStates;

            // Initialize the stack used for the DFS with the states.
            std::vector<uint_fast64_t> stack(psiStates.begin(), psiStates.end());

            // Initialize the stack for the step bound, if the number of steps is bounded.
            std::vector<uint_fast64_t> stepStack;
            std::vector<uint_fast64_t> remainingSteps;
            if (useStepBound) {
                stepStack.reserve(numberOfStates);
                stepStack.insert(stepStack.begin(), psiStates.getNumberOfSetBits(), maximalSteps);
                remainingSteps.resize(numberOfStates);
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

                for (typename storm::storage::SparseMatrix<T>::const_iterator entryIt = backwardTransitions.begin(currentState), entryIte = backwardTransitions.end(currentState); entryIt != entryIte; ++entryIt) {
                    if (phiStates[entryIt->getColumn()] && (!statesWithProbabilityGreater0.get(entryIt->getColumn()) || (useStepBound && remainingSteps[entryIt->getColumn()] < currentStepBound - 1))) {
                        // If we don't have a bound on the number of steps to take, just add the state to the stack.
                        if (!useStepBound) {
                            statesWithProbabilityGreater0.set(entryIt->getColumn(), true);
                            stack.push_back(entryIt->getColumn());
                        } else if (currentStepBound > 0) {
                            // If there is at least one more step to go, we need to push the state and the new number of steps.
                            remainingSteps[entryIt->getColumn()] = currentStepBound - 1;
                            statesWithProbabilityGreater0.set(entryIt->getColumn(), true);
                            stack.push_back(entryIt->getColumn());
                            stepStack.push_back(currentStepBound - 1);
                        }
                    }
                }
            }

            // Return result.
            return statesWithProbabilityGreater0;
        }
    }
}

#endif /* CUDA_KERNELS_GRAPH_H */