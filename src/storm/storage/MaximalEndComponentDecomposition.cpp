#include <list>
#include <numeric>
#include <queue>

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/utility/graph.h"

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
    performMaximalEndComponentDecomposition(transitionMatrix, backwardTransitions, states);
}

template<typename ValueType>
MaximalEndComponentDecomposition<ValueType>::MaximalEndComponentDecomposition(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                              storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                                              storm::storage::BitVector const& states,
                                                                              storm::storage::BitVector const& choices) {
    performMaximalEndComponentDecomposition(transitionMatrix, backwardTransitions, states, choices);
}

template<typename ValueType>
MaximalEndComponentDecomposition<ValueType>::MaximalEndComponentDecomposition(storm::models::sparse::NondeterministicModel<ValueType> const& model,
                                                                              storm::storage::BitVector const& states) {
    performMaximalEndComponentDecomposition(model.getTransitionMatrix(), model.getBackwardTransitions(), states);
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
                                                                                          storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                                                          storm::OptionalRef<storm::storage::BitVector const> states,
                                                                                          storm::OptionalRef<storm::storage::BitVector const> choices) {
    // Get some data for convenient access.
    auto const& nondeterministicChoiceIndices = transitionMatrix.getRowGroupIndices();

    storm::storage::BitVector remainingEcCandidates, ecChoices;
    SccDecompositionResult sccDecRes;
    SccDecompositionMemoryCache sccDecCache;
    StronglyConnectedComponentDecompositionOptions sccDecOptions;
    sccDecOptions.dropNaiveSccs();
    if (states) {
        sccDecOptions.subsystem(*states);
    }
    if (choices) {
        ecChoices = *choices;
        sccDecOptions.choices(ecChoices);
    } else {
        ecChoices.resize(transitionMatrix.getRowCount(), true);
    }

    while (true) {
        performSccDecomposition(transitionMatrix, sccDecOptions, sccDecRes, sccDecCache);

        remainingEcCandidates = sccDecRes.nonTrivialStates;
        storm::storage::BitVector ecSccIndices(sccDecRes.sccCount, true);
        storm::storage::BitVector nonTrivSccIndices(sccDecRes.sccCount, false);
        // find the choices that do not stay in their SCC
        for (auto state : remainingEcCandidates) {
            auto const sccIndex = sccDecRes.stateToSccMapping[state];
            nonTrivSccIndices.set(sccIndex, true);
            bool stateCanStayInScc = false;
            for (auto const choice : transitionMatrix.getRowGroupIndices(state)) {
                if (!ecChoices.get(choice)) {
                    continue;
                }
                auto row = transitionMatrix.getRow(choice);
                if (std::any_of(row.begin(), row.end(), [&sccIndex, &sccDecRes](auto const& entry) {
                        return sccIndex != sccDecRes.stateToSccMapping[entry.getColumn()] && !storm::utility::isZero(entry.getValue());
                    })) {
                    ecChoices.set(choice, false);       // The choice leaves the SCC
                    ecSccIndices.set(sccIndex, false);  // This SCC is not 'stable' yet
                } else {
                    stateCanStayInScc = true;  // The choice stays in the SCC
                }
            }
            if (!stateCanStayInScc) {
                remainingEcCandidates.set(state, false);  // This state is not in an EC
            }
        }

        // process the MECs that we've found, i.e. SCCs where every state can stay inside the SCC
        ecSccIndices &= nonTrivSccIndices;
        for (auto sccIndex : ecSccIndices) {
            MaximalEndComponent newMec;
            for (auto state : remainingEcCandidates) {
                // skip states from different SCCs
                if (sccDecRes.stateToSccMapping[state] != sccIndex) {
                    continue;
                }
                // This is no longer a candidate
                remainingEcCandidates.set(state, false);
                // Add choices to the MEC
                MaximalEndComponent::set_type containedChoices;
                for (auto ecChoiceIt = ecChoices.begin(nondeterministicChoiceIndices[state]); *ecChoiceIt < nondeterministicChoiceIndices[state + 1];
                     ++ecChoiceIt) {
                    containedChoices.insert(*ecChoiceIt);
                }
                STORM_LOG_ASSERT(!containedChoices.empty(), "The contained choices of any state in an MEC must be non-empty.");
                newMec.addState(state, std::move(containedChoices));
            }
            this->blocks.emplace_back(std::move(newMec));
        }

        if (nonTrivSccIndices == ecSccIndices) {
            // All non trivial SCCs are MECs, nothing left to do!
            break;
        }

        // prepare next iteration.
        // It suffices to keep the candidates that have the possibility to always stay in the candidate set
        remainingEcCandidates = storm::utility::graph::performProbGreater0A(transitionMatrix, nondeterministicChoiceIndices, backwardTransitions,
                                                                            remainingEcCandidates, ~remainingEcCandidates, false, 0, ecChoices);
        remainingEcCandidates.complement();
        sccDecOptions.subsystem(remainingEcCandidates);
        sccDecOptions.choices(ecChoices);
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
