#include <list>
#include <numeric>
#include <queue>

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"

namespace storm {
namespace storage {

template<typename ValueType>
MaximalEndComponentDecomposition<ValueType>::MaximalEndComponentDecomposition() : Decomposition() {
    // Intentionally left empty.
}

template<typename ValueType>
template<typename RewardModelType>
MaximalEndComponentDecomposition<ValueType>::MaximalEndComponentDecomposition(
    storm::models::sparse::NondeterministicModel<ValueType, RewardModelType> const& model) {
    performMaximalEndComponentDecomposition(model.getTransitionMatrix(), model.getBackwardTransitions());
}

template<typename ValueType>
MaximalEndComponentDecomposition<ValueType>::MaximalEndComponentDecomposition(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                              storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {
    performMaximalEndComponentDecomposition(transitionMatrix, backwardTransitions);
}

template<typename ValueType>
MaximalEndComponentDecomposition<ValueType>::MaximalEndComponentDecomposition(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                              storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                                              storm::storage::BitVector const& states) {
    performMaximalEndComponentDecomposition(transitionMatrix, backwardTransitions, &states);
}

template<typename ValueType>
MaximalEndComponentDecomposition<ValueType>::MaximalEndComponentDecomposition(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                              storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                                              storm::storage::BitVector const& states,
                                                                              storm::storage::BitVector const& choices) {
    performMaximalEndComponentDecomposition(transitionMatrix, backwardTransitions, &states, &choices);
}

template<typename ValueType>
MaximalEndComponentDecomposition<ValueType>::MaximalEndComponentDecomposition(storm::models::sparse::NondeterministicModel<ValueType> const& model,
                                                                              storm::storage::BitVector const& states) {
    performMaximalEndComponentDecomposition(model.getTransitionMatrix(), model.getBackwardTransitions(), &states);
}

template<typename ValueType>
MaximalEndComponentDecomposition<ValueType>::MaximalEndComponentDecomposition(MaximalEndComponentDecomposition const& other) : Decomposition(other) {
    // Intentionally left empty.
}

template<typename ValueType>
MaximalEndComponentDecomposition<ValueType>& MaximalEndComponentDecomposition<ValueType>::operator=(MaximalEndComponentDecomposition const& other) {
    Decomposition::operator=(other);
    return *this;
}

template<typename ValueType>
MaximalEndComponentDecomposition<ValueType>::MaximalEndComponentDecomposition(MaximalEndComponentDecomposition&& other) : Decomposition(std::move(other)) {
    // Intentionally left empty.
}

template<typename ValueType>
MaximalEndComponentDecomposition<ValueType>& MaximalEndComponentDecomposition<ValueType>::operator=(MaximalEndComponentDecomposition&& other) {
    Decomposition::operator=(std::move(other));
    return *this;
}

template<typename ValueType>
void MaximalEndComponentDecomposition<ValueType>::performMaximalEndComponentDecomposition(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                                          storm::storage::SparseMatrix<ValueType> backwardTransitions,
                                                                                          storm::storage::BitVector const* states,
                                                                                          storm::storage::BitVector const* choices) {
    // Get some data for convenient access.
    uint_fast64_t numberOfStates = transitionMatrix.getRowGroupCount();
    std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = transitionMatrix.getRowGroupIndices();

    // Initialize the maximal end component list to be the full state space.
    std::list<StateBlock> endComponentStateSets;
    if (states) {
        endComponentStateSets.emplace_back(states->begin(), states->end(), true);
    } else {
        std::vector<storm::storage::sparse::state_type> allStates;
        allStates.resize(transitionMatrix.getRowGroupCount());
        std::iota(allStates.begin(), allStates.end(), 0);
        endComponentStateSets.emplace_back(allStates.begin(), allStates.end(), true);
    }
    storm::storage::BitVector statesToCheck(numberOfStates);
    storm::storage::BitVector includedChoices;
    if (choices) {
        includedChoices = *choices;
        if (states) {
            // Exclude choices that originate from or lead to states that are not considered.
            includedChoices &= transitionMatrix.getRowFilter(*states, *states);
        }
    } else if (states) {
        // Exclude choices that originate from or lead to states that are not considered.
        includedChoices = transitionMatrix.getRowFilter(*states, *states);
    } else {
        includedChoices = storm::storage::BitVector(transitionMatrix.getRowCount(), true);
    }
    storm::storage::BitVector currMecAsBitVector(transitionMatrix.getRowGroupCount());

    for (std::list<StateBlock>::const_iterator mecIterator = endComponentStateSets.begin(); mecIterator != endComponentStateSets.end();) {
        StateBlock const& mec = *mecIterator;
        currMecAsBitVector.clear();
        currMecAsBitVector.set(mec.begin(), mec.end(), true);
        // Keep track of whether the MEC changed during this iteration.
        bool mecChanged = false;

        // Get an SCC decomposition of the current MEC candidate.

        StronglyConnectedComponentDecomposition<ValueType> sccs(
            transitionMatrix, StronglyConnectedComponentDecompositionOptions().subsystem(&currMecAsBitVector).choices(&includedChoices).dropNaiveSccs());

        // We need to do another iteration in case we have either more than once SCC or the SCC is smaller than
        // the MEC canditate itself.
        mecChanged |= sccs.size() != 1 || (sccs.size() > 0 && sccs[0].size() < mec.size());

        // Check for each of the SCCs whether there is at least one action for each state that does not leave the SCC.
        for (auto& scc : sccs) {
            statesToCheck.set(scc.begin(), scc.end());

            while (!statesToCheck.empty()) {
                storm::storage::BitVector statesToRemove(numberOfStates);

                for (auto state : statesToCheck) {
                    bool keepStateInMEC = false;

                    for (uint_fast64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice) {
                        // If the choice is not part of our subsystem, skip it.
                        if (choices && !choices->get(choice)) {
                            continue;
                        }

                        // If the choice is not included any more, skip it.
                        if (!includedChoices.get(choice)) {
                            continue;
                        }

                        bool choiceContainedInMEC = true;
                        for (auto const& entry : transitionMatrix.getRow(choice)) {
                            if (storm::utility::isZero(entry.getValue())) {
                                continue;
                            }

                            if (!scc.containsState(entry.getColumn())) {
                                includedChoices.set(choice, false);
                                choiceContainedInMEC = false;
                                break;
                            }
                        }

                        // If there is at least one choice whose successor states are fully contained in the MEC, we can leave the state in the MEC.
                        if (choiceContainedInMEC) {
                            keepStateInMEC = true;
                        }
                    }

                    if (!keepStateInMEC) {
                        statesToRemove.set(state, true);
                    }
                }

                // Now erase the states that have no option to stay inside the MEC with all successors.
                mecChanged |= !statesToRemove.empty();
                for (uint_fast64_t state : statesToRemove) {
                    scc.erase(state);
                }

                // Now check which states should be reconsidered, because successors of them were removed.
                statesToCheck.clear();
                for (auto state : statesToRemove) {
                    for (auto const& entry : backwardTransitions.getRow(state)) {
                        if (scc.containsState(entry.getColumn())) {
                            statesToCheck.set(entry.getColumn());
                        }
                    }
                }
            }
        }

        // If the MEC changed, we delete it from the list of MECs and append the possible new MEC candidates to
        // the list instead.
        if (mecChanged) {
            for (StronglyConnectedComponent& scc : sccs) {
                if (!scc.empty()) {
                    endComponentStateSets.push_back(std::move(scc));
                }
            }

            std::list<StateBlock>::const_iterator eraseIterator(mecIterator);
            ++mecIterator;
            endComponentStateSets.erase(eraseIterator);
        } else {
            // Otherwise, we proceed with the next MEC candidate.
            ++mecIterator;
        }

    }  // End of loop over all MEC candidates.

    // Now that we computed the underlying state sets of the MECs, we need to properly identify the choices
    // contained in the MEC and store them as actual MECs.
    this->blocks.reserve(endComponentStateSets.size());
    for (auto const& mecStateSet : endComponentStateSets) {
        MaximalEndComponent newMec;

        for (auto state : mecStateSet) {
            MaximalEndComponent::set_type containedChoices;
            for (uint_fast64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice) {
                // Skip the choice if it is not part of our subsystem.
                if (choices && !choices->get(choice)) {
                    continue;
                }

                if (includedChoices.get(choice)) {
                    containedChoices.insert(choice);
                }
            }

            STORM_LOG_ASSERT(!containedChoices.empty(), "The contained choices of any state in an MEC must be non-empty.");
            newMec.addState(state, std::move(containedChoices));
        }

        this->blocks.emplace_back(std::move(newMec));
    }

    STORM_LOG_DEBUG("MEC decomposition found " << this->size() << " MEC(s).");
}

// Explicitly instantiate the MEC decomposition.
template class MaximalEndComponentDecomposition<double>;
template MaximalEndComponentDecomposition<double>::MaximalEndComponentDecomposition(storm::models::sparse::NondeterministicModel<double> const& model);

#ifdef STORM_HAVE_CARL
template class MaximalEndComponentDecomposition<storm::RationalNumber>;
template MaximalEndComponentDecomposition<storm::RationalNumber>::MaximalEndComponentDecomposition(
    storm::models::sparse::NondeterministicModel<storm::RationalNumber> const& model);

template class MaximalEndComponentDecomposition<storm::RationalFunction>;
template MaximalEndComponentDecomposition<storm::RationalFunction>::MaximalEndComponentDecomposition(
    storm::models::sparse::NondeterministicModel<storm::RationalFunction> const& model);
#endif
}  // namespace storage
}  // namespace storm
