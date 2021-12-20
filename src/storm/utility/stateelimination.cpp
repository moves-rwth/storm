#include "storm/utility/stateelimination.h"

#include <random>

#include "storm/solver/stateelimination/DynamicStatePriorityQueue.h"
#include "storm/solver/stateelimination/StatePriorityQueue.h"
#include "storm/solver/stateelimination/StaticStatePriorityQueue.h"

#include "storm/storage/BitVector.h"
#include "storm/storage/FlexibleSparseMatrix.h"

#include "storm/settings/SettingsManager.h"

#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/utility/constants.h"
#include "storm/utility/graph.h"
#include "storm/utility/macros.h"

namespace storm {
namespace utility {
namespace stateelimination {

bool eliminationOrderNeedsDistances(storm::settings::modules::EliminationSettings::EliminationOrder const& order) {
    return order == storm::settings::modules::EliminationSettings::EliminationOrder::Forward ||
           order == storm::settings::modules::EliminationSettings::EliminationOrder::ForwardReversed ||
           order == storm::settings::modules::EliminationSettings::EliminationOrder::Backward ||
           order == storm::settings::modules::EliminationSettings::EliminationOrder::BackwardReversed;
}

bool eliminationOrderNeedsForwardDistances(storm::settings::modules::EliminationSettings::EliminationOrder const& order) {
    return order == storm::settings::modules::EliminationSettings::EliminationOrder::Forward ||
           order == storm::settings::modules::EliminationSettings::EliminationOrder::ForwardReversed;
}

bool eliminationOrderNeedsReversedDistances(storm::settings::modules::EliminationSettings::EliminationOrder const& order) {
    return order == storm::settings::modules::EliminationSettings::EliminationOrder::ForwardReversed ||
           order == storm::settings::modules::EliminationSettings::EliminationOrder::BackwardReversed;
}

bool eliminationOrderIsPenaltyBased(storm::settings::modules::EliminationSettings::EliminationOrder const& order) {
    return order == storm::settings::modules::EliminationSettings::EliminationOrder::StaticPenalty ||
           order == storm::settings::modules::EliminationSettings::EliminationOrder::DynamicPenalty ||
           order == storm::settings::modules::EliminationSettings::EliminationOrder::RegularExpression;
}

bool eliminationOrderIsStatic(storm::settings::modules::EliminationSettings::EliminationOrder const& order) {
    return eliminationOrderNeedsDistances(order) || order == storm::settings::modules::EliminationSettings::EliminationOrder::StaticPenalty;
}

template<typename ValueType>
uint_fast64_t estimateComplexity(ValueType const&) {
    return 1;
}

#ifdef STORM_HAVE_CARL
template<>
uint_fast64_t estimateComplexity(storm::RationalFunction const& value) {
    if (storm::utility::isConstant(value)) {
        return 1;
    }
    if (value.denominator().isConstant()) {
        return value.nominator().complexity();
    } else {
        return value.denominator().complexity() * value.nominator().complexity();
    }
}
#endif

template<typename ValueType>
uint_fast64_t computeStatePenalty(storm::storage::sparse::state_type const& state, storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix,
                                  storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions,
                                  std::vector<ValueType> const& oneStepProbabilities) {
    uint_fast64_t penalty = 0;
    bool hasParametricSelfLoop = false;

    for (auto const& predecessor : backwardTransitions.getRow(state)) {
        for (auto const& successor : transitionMatrix.getRow(state)) {
            penalty += estimateComplexity(predecessor.getValue()) * estimateComplexity(successor.getValue());
        }
        if (predecessor.getColumn() == state) {
            hasParametricSelfLoop = !storm::utility::isConstant(predecessor.getValue());
        }
        penalty += estimateComplexity(oneStepProbabilities[predecessor.getColumn()]) * estimateComplexity(predecessor.getValue()) *
                   estimateComplexity(oneStepProbabilities[state]);
    }

    // If it is a self-loop that is parametric, we increase the penalty a lot.
    if (hasParametricSelfLoop) {
        penalty *= 10;
    }

    return penalty;
}

template<typename ValueType>
uint_fast64_t computeStatePenaltyRegularExpression(storm::storage::sparse::state_type const& state,
                                                   storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix,
                                                   storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const&) {
    return backwardTransitions.getRow(state).size() * transitionMatrix.getRow(state).size();
}

template<typename ValueType>
std::shared_ptr<StatePriorityQueue> createStatePriorityQueue(boost::optional<std::vector<uint_fast64_t>> const& distanceBasedStatePriorities,
                                                             storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix,
                                                             storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions,
                                                             std::vector<ValueType> const& oneStepProbabilities, storm::storage::BitVector const& states) {
    STORM_LOG_TRACE("Creating state priority queue for states " << states);

    // Get the settings to customize the priority queue.
    storm::settings::modules::EliminationSettings::EliminationOrder order =
        storm::settings::getModule<storm::settings::modules::EliminationSettings>().getEliminationOrder();

    std::vector<storm::storage::sparse::state_type> sortedStates(states.begin(), states.end());

    if (order == storm::settings::modules::EliminationSettings::EliminationOrder::Random) {
        std::random_device randomDevice;
        std::mt19937 generator(randomDevice());
        std::shuffle(sortedStates.begin(), sortedStates.end(), generator);
        return std::make_unique<StaticStatePriorityQueue>(sortedStates);
    } else {
        if (eliminationOrderNeedsDistances(order)) {
            STORM_LOG_THROW(static_cast<bool>(distanceBasedStatePriorities), storm::exceptions::InvalidStateException,
                            "Unable to build state priority queue without distance-based priorities.");
            std::sort(sortedStates.begin(), sortedStates.end(),
                      [&distanceBasedStatePriorities](storm::storage::sparse::state_type const& state1, storm::storage::sparse::state_type const& state2) {
                          return distanceBasedStatePriorities.get()[state1] < distanceBasedStatePriorities.get()[state2];
                      });
            return std::make_unique<StaticStatePriorityQueue>(sortedStates);
        } else if (eliminationOrderIsPenaltyBased(order)) {
            std::vector<std::pair<storm::storage::sparse::state_type, uint_fast64_t>> statePenalties(sortedStates.size());
            typename DynamicStatePriorityQueue<ValueType>::PenaltyFunctionType penaltyFunction =
                order == storm::settings::modules::EliminationSettings::EliminationOrder::RegularExpression ? computeStatePenaltyRegularExpression<ValueType>
                                                                                                            : computeStatePenalty<ValueType>;
            for (uint_fast64_t index = 0; index < sortedStates.size(); ++index) {
                statePenalties[index] =
                    std::make_pair(sortedStates[index], penaltyFunction(sortedStates[index], transitionMatrix, backwardTransitions, oneStepProbabilities));
            }

            std::sort(
                statePenalties.begin(), statePenalties.end(),
                [](std::pair<storm::storage::sparse::state_type, uint_fast64_t> const& statePenalty1,
                   std::pair<storm::storage::sparse::state_type, uint_fast64_t> const& statePenalty2) { return statePenalty1.second < statePenalty2.second; });

            if (eliminationOrderIsStatic(order)) {
                // For the static penalty version, we need to strip the penalties to create the queue.
                for (uint_fast64_t index = 0; index < sortedStates.size(); ++index) {
                    sortedStates[index] = statePenalties[index].first;
                }
                return std::make_unique<StaticStatePriorityQueue>(sortedStates);
            } else {
                // For the dynamic penalty version, we need to give the full state-penalty pairs.
                return std::make_unique<DynamicStatePriorityQueue<ValueType>>(statePenalties, transitionMatrix, backwardTransitions, oneStepProbabilities,
                                                                              penaltyFunction);
            }
        }
    }
    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Illegal elimination order selected.");
}

std::shared_ptr<StatePriorityQueue> createStatePriorityQueue(storm::storage::BitVector const& states) {
    std::vector<storm::storage::sparse::state_type> sortedStates(states.begin(), states.end());
    return std::make_shared<StaticStatePriorityQueue>(sortedStates);
}

std::shared_ptr<StatePriorityQueue> createStatePriorityQueue(std::vector<storm::storage::sparse::state_type> const& states) {
    return std::make_shared<StaticStatePriorityQueue>(states);
}

template<typename ValueType>
std::vector<uint_fast64_t> getDistanceBasedPriorities(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                      storm::storage::SparseMatrix<ValueType> const& transitionMatrixTransposed,
                                                      storm::storage::BitVector const& initialStates, std::vector<ValueType> const& oneStepProbabilities,
                                                      bool forward, bool reverse) {
    std::vector<uint_fast64_t> statePriorities(transitionMatrix.getRowCount());
    std::vector<storm::storage::sparse::state_type> states(transitionMatrix.getRowCount());
    for (std::size_t index = 0; index < states.size(); ++index) {
        states[index] = index;
    }

    std::vector<uint_fast64_t> distances =
        getStateDistances(transitionMatrix, transitionMatrixTransposed, initialStates, oneStepProbabilities,
                          storm::settings::getModule<storm::settings::modules::EliminationSettings>().getEliminationOrder() ==
                                  storm::settings::modules::EliminationSettings::EliminationOrder::Forward ||
                              storm::settings::getModule<storm::settings::modules::EliminationSettings>().getEliminationOrder() ==
                                  storm::settings::modules::EliminationSettings::EliminationOrder::ForwardReversed);

    // In case of the forward or backward ordering, we can sort the states according to the distances.
    if (forward ^ reverse) {
        std::sort(states.begin(), states.end(),
                  [&distances](storm::storage::sparse::state_type const& state1, storm::storage::sparse::state_type const& state2) {
                      return distances[state1] < distances[state2];
                  });
    } else {
        // Otherwise, we sort them according to descending distances.
        std::sort(states.begin(), states.end(),
                  [&distances](storm::storage::sparse::state_type const& state1, storm::storage::sparse::state_type const& state2) {
                      return distances[state1] > distances[state2];
                  });
    }

    // Now convert the ordering of the states to priorities.
    for (uint_fast64_t index = 0; index < states.size(); ++index) {
        statePriorities[states[index]] = index;
    }

    return statePriorities;
}

template<typename ValueType>
std::vector<uint_fast64_t> getStateDistances(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                             storm::storage::SparseMatrix<ValueType> const& transitionMatrixTransposed,
                                             storm::storage::BitVector const& initialStates, std::vector<ValueType> const& oneStepProbabilities, bool forward) {
    if (forward) {
        return storm::utility::graph::getDistances(transitionMatrix, initialStates);
    } else {
        // Since the target states were eliminated from the matrix already, we construct a replacement by
        // treating all states that have some non-zero probability to go to a target state in one step as target
        // states.
        storm::storage::BitVector pseudoTargetStates(transitionMatrix.getRowCount());
        for (std::size_t index = 0; index < oneStepProbabilities.size(); ++index) {
            if (oneStepProbabilities[index] != storm::utility::zero<ValueType>()) {
                pseudoTargetStates.set(index);
            }
        }

        return storm::utility::graph::getDistances(transitionMatrixTransposed, pseudoTargetStates);
    }
}

template uint_fast64_t estimateComplexity(double const& value);
template std::shared_ptr<StatePriorityQueue> createStatePriorityQueue(boost::optional<std::vector<uint_fast64_t>> const& distanceBasedStatePriorities,
                                                                      storm::storage::FlexibleSparseMatrix<double> const& transitionMatrix,
                                                                      storm::storage::FlexibleSparseMatrix<double> const& backwardTransitions,
                                                                      std::vector<double> const& oneStepProbabilities, storm::storage::BitVector const& states);
template uint_fast64_t computeStatePenalty(storm::storage::sparse::state_type const& state,
                                           storm::storage::FlexibleSparseMatrix<double> const& transitionMatrix,
                                           storm::storage::FlexibleSparseMatrix<double> const& backwardTransitions,
                                           std::vector<double> const& oneStepProbabilities);
template uint_fast64_t computeStatePenaltyRegularExpression(storm::storage::sparse::state_type const& state,
                                                            storm::storage::FlexibleSparseMatrix<double> const& transitionMatrix,
                                                            storm::storage::FlexibleSparseMatrix<double> const& backwardTransitions,
                                                            std::vector<double> const& oneStepProbabilities);
template std::vector<uint_fast64_t> getDistanceBasedPriorities(storm::storage::SparseMatrix<double> const& transitionMatrix,
                                                               storm::storage::SparseMatrix<double> const& transitionMatrixTransposed,
                                                               storm::storage::BitVector const& initialStates, std::vector<double> const& oneStepProbabilities,
                                                               bool forward, bool reverse);
template std::vector<uint_fast64_t> getStateDistances(storm::storage::SparseMatrix<double> const& transitionMatrix,
                                                      storm::storage::SparseMatrix<double> const& transitionMatrixTransposed,
                                                      storm::storage::BitVector const& initialStates, std::vector<double> const& oneStepProbabilities,
                                                      bool forward);

#ifdef STORM_HAVE_CARL
template uint_fast64_t estimateComplexity(storm::RationalNumber const& value);
template std::shared_ptr<StatePriorityQueue> createStatePriorityQueue(boost::optional<std::vector<uint_fast64_t>> const& distanceBasedStatePriorities,
                                                                      storm::storage::FlexibleSparseMatrix<storm::RationalNumber> const& transitionMatrix,
                                                                      storm::storage::FlexibleSparseMatrix<storm::RationalNumber> const& backwardTransitions,
                                                                      std::vector<storm::RationalNumber> const& oneStepProbabilities,
                                                                      storm::storage::BitVector const& states);
template uint_fast64_t computeStatePenalty(storm::storage::sparse::state_type const& state,
                                           storm::storage::FlexibleSparseMatrix<storm::RationalNumber> const& transitionMatrix,
                                           storm::storage::FlexibleSparseMatrix<storm::RationalNumber> const& backwardTransitions,
                                           std::vector<storm::RationalNumber> const& oneStepProbabilities);
template uint_fast64_t computeStatePenaltyRegularExpression(storm::storage::sparse::state_type const& state,
                                                            storm::storage::FlexibleSparseMatrix<storm::RationalNumber> const& transitionMatrix,
                                                            storm::storage::FlexibleSparseMatrix<storm::RationalNumber> const& backwardTransitions,
                                                            std::vector<storm::RationalNumber> const& oneStepProbabilities);
template std::vector<uint_fast64_t> getDistanceBasedPriorities(storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix,
                                                               storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrixTransposed,
                                                               storm::storage::BitVector const& initialStates,
                                                               std::vector<storm::RationalNumber> const& oneStepProbabilities, bool forward, bool reverse);
template std::vector<uint_fast64_t> getStateDistances(storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix,
                                                      storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrixTransposed,
                                                      storm::storage::BitVector const& initialStates,
                                                      std::vector<storm::RationalNumber> const& oneStepProbabilities, bool forward);

template std::shared_ptr<StatePriorityQueue> createStatePriorityQueue(boost::optional<std::vector<uint_fast64_t>> const& distanceBasedStatePriorities,
                                                                      storm::storage::FlexibleSparseMatrix<storm::RationalFunction> const& transitionMatrix,
                                                                      storm::storage::FlexibleSparseMatrix<storm::RationalFunction> const& backwardTransitions,
                                                                      std::vector<storm::RationalFunction> const& oneStepProbabilities,
                                                                      storm::storage::BitVector const& states);
template uint_fast64_t computeStatePenalty(storm::storage::sparse::state_type const& state,
                                           storm::storage::FlexibleSparseMatrix<storm::RationalFunction> const& transitionMatrix,
                                           storm::storage::FlexibleSparseMatrix<storm::RationalFunction> const& backwardTransitions,
                                           std::vector<storm::RationalFunction> const& oneStepProbabilities);
template uint_fast64_t computeStatePenaltyRegularExpression(storm::storage::sparse::state_type const& state,
                                                            storm::storage::FlexibleSparseMatrix<storm::RationalFunction> const& transitionMatrix,
                                                            storm::storage::FlexibleSparseMatrix<storm::RationalFunction> const& backwardTransitions,
                                                            std::vector<storm::RationalFunction> const& oneStepProbabilities);
template std::vector<uint_fast64_t> getDistanceBasedPriorities(storm::storage::SparseMatrix<storm::RationalFunction> const& transitionMatrix,
                                                               storm::storage::SparseMatrix<storm::RationalFunction> const& transitionMatrixTransposed,
                                                               storm::storage::BitVector const& initialStates,
                                                               std::vector<storm::RationalFunction> const& oneStepProbabilities, bool forward, bool reverse);
template std::vector<uint_fast64_t> getStateDistances(storm::storage::SparseMatrix<storm::RationalFunction> const& transitionMatrix,
                                                      storm::storage::SparseMatrix<storm::RationalFunction> const& transitionMatrixTransposed,
                                                      storm::storage::BitVector const& initialStates,
                                                      std::vector<storm::RationalFunction> const& oneStepProbabilities, bool forward);
#endif
}  // namespace stateelimination
}  // namespace utility
}  // namespace storm
