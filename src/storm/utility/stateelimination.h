#pragma once

#include <memory>
#include <vector>

#include <boost/optional.hpp>

#include "storm/storage/sparse/StateType.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/settings/modules/EliminationSettings.h"

namespace storm {
namespace solver {
namespace stateelimination {
class StatePriorityQueue;
}
}  // namespace solver

namespace storage {
class BitVector;

template<typename ValueType>
class FlexibleSparseMatrix;

template<typename ValueType>
class SparseMatrix;
}  // namespace storage

namespace utility {
namespace stateelimination {

using namespace storm::solver::stateelimination;

bool eliminationOrderNeedsDistances(storm::settings::modules::EliminationSettings::EliminationOrder const& order);
bool eliminationOrderNeedsForwardDistances(storm::settings::modules::EliminationSettings::EliminationOrder const& order);
bool eliminationOrderNeedsReversedDistances(storm::settings::modules::EliminationSettings::EliminationOrder const& order);
bool eliminationOrderIsPenaltyBased(storm::settings::modules::EliminationSettings::EliminationOrder const& order);
bool eliminationOrderIsStatic(storm::settings::modules::EliminationSettings::EliminationOrder const& order);

template<typename ValueType>
uint_fast64_t estimateComplexity(ValueType const& value);

#ifdef STORM_HAVE_CARL
template<>
uint_fast64_t estimateComplexity(storm::RationalFunction const& value);
#endif

template<typename ValueType>
uint_fast64_t computeStatePenalty(storm::storage::sparse::state_type const& state, storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix,
                                  storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions,
                                  std::vector<ValueType> const& oneStepProbabilities);

template<typename ValueType>
uint_fast64_t computeStatePenaltyRegularExpression(storm::storage::sparse::state_type const& state,
                                                   storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix,
                                                   storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions,
                                                   std::vector<ValueType> const& oneStepProbabilities);

template<typename ValueType>
std::shared_ptr<StatePriorityQueue> createStatePriorityQueue(boost::optional<std::vector<uint_fast64_t>> const& stateDistances,
                                                             storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix,
                                                             storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions,
                                                             std::vector<ValueType> const& oneStepProbabilities, storm::storage::BitVector const& states);

std::shared_ptr<StatePriorityQueue> createStatePriorityQueue(storm::storage::BitVector const& states);
std::shared_ptr<StatePriorityQueue> createStatePriorityQueue(std::vector<storm::storage::sparse::state_type> const& states);

template<typename ValueType>
std::vector<uint_fast64_t> getDistanceBasedPriorities(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                      storm::storage::SparseMatrix<ValueType> const& transitionMatrixTransposed,
                                                      storm::storage::BitVector const& initialStates, std::vector<ValueType> const& oneStepProbabilities,
                                                      bool forward, bool reverse);

template<typename ValueType>
std::vector<uint_fast64_t> getStateDistances(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                             storm::storage::SparseMatrix<ValueType> const& transitionMatrixTransposed,
                                             storm::storage::BitVector const& initialStates, std::vector<ValueType> const& oneStepProbabilities, bool forward);

}  // namespace stateelimination
}  // namespace utility
}  // namespace storm
