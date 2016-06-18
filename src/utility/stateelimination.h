#pragma once

#include <memory>
#include <vector>

#include <boost/optional.hpp>

#include "src/storage/sparse/StateType.h"

#include "src/adapters/CarlAdapter.h"

#include "src/settings/modules/SparseDtmcEliminationModelCheckerSettings.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            class StatePriorityQueue;
        }
    }
    
    namespace storage {
        class BitVector;
        
        template<typename ValueType>
        class FlexibleSparseMatrix;
    }
    
    namespace utility {
        namespace stateelimination {
            
            using namespace storm::solver::stateelimination;
            
            bool eliminationOrderNeedsDistances(storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder const& order);
            bool eliminationOrderNeedsForwardDistances(storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder const& order);
            bool eliminationOrderNeedsReversedDistances(storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder const& order);
            bool eliminationOrderIsPenaltyBased(storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder const& order);
            bool eliminationOrderIsStatic(storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder const& order);
            
            template<typename ValueType>
            uint_fast64_t estimateComplexity(ValueType const& value);

#ifdef STORM_HAVE_CARL
            template<>
            uint_fast64_t estimateComplexity(storm::RationalFunction const& value);
#endif
            
            template<typename ValueType>
            uint_fast64_t computeStatePenalty(storm::storage::sparse::state_type const& state, storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities);
            
            template<typename ValueType>
            uint_fast64_t computeStatePenaltyRegularExpression(storm::storage::sparse::state_type const& state, storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities);
            
            template<typename ValueType>
            std::shared_ptr<StatePriorityQueue> createStatePriorityQueue(boost::optional<std::vector<uint_fast64_t>> const& stateDistances, storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType>& oneStepProbabilities, storm::storage::BitVector const& states);
            
            std::shared_ptr<StatePriorityQueue> createNaivePriorityQueue(storm::storage::BitVector const& states);
            
        }
    }
}