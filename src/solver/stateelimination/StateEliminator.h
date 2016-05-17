#ifndef STORM_SOLVER_STATEELIMINATION_STATEELIMINATOR_H_
#define STORM_SOLVER_STATEELIMINATION_STATEELIMINATOR_H_

#include "src/storage/FlexibleSparseMatrix.h"
#include "src/storage/SparseMatrix.h"
#include "src/storage/sparse/StateType.h"
#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/adapters/CarlAdapter.h"
#include "src/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "src/utility/macros.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            template<typename SparseModelType>
            class StateEliminator {
                typedef typename SparseModelType::ValueType ValueType;
                typedef typename storm::storage::FlexibleSparseMatrix<ValueType>::row_type FlexibleRowType;
                typedef typename FlexibleRowType::iterator FlexibleRowIterator;

            public:
                StateEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions);
                
                void eliminateState(storm::storage::sparse::state_type state, bool removeForwardTransitions, storm::storage::BitVector predecessorConstraint = storm::storage::BitVector());
                
                // Virtual methods for base classes to distinguish between different state elimination approaches
                virtual void updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability) = 0;
                virtual void updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) = 0;
                virtual void updatePriority(storm::storage::sparse::state_type const& state) = 0;
                virtual bool filterPredecessor(storm::storage::sparse::state_type const& state) = 0;
                virtual bool isFilterPredecessor() const = 0;
                
                
            protected:
                storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix;
                storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions;
                
            };
            
        } // namespace stateelimination
    } // namespace storage
} // namespace storm

#endif // STORM_SOLVER_STATEELIMINATION_STATEELIMINATOR_H_
