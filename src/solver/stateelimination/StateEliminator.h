#ifndef STORM_SOLVER_STATEELIMINATION_STATEELIMINATOR_H_
#define STORM_SOLVER_STATEELIMINATION_STATEELIMINATOR_H_

#include "src/storage/sparse/StateType.h"

#include "src/storage/FlexibleSparseMatrix.h"
#include "src/storage/BitVector.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            template<typename ValueType>
            class StateEliminator {
            public:
                typedef typename storm::storage::FlexibleSparseMatrix<ValueType>::row_type FlexibleRowType;
                typedef typename FlexibleRowType::iterator FlexibleRowIterator;

                StateEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions);
                
                void eliminateState(storm::storage::sparse::state_type state, bool removeForwardTransitions, storm::storage::BitVector predecessorConstraint = storm::storage::BitVector());
                
                // Provide virtual methods that can be customized by subclasses to govern side-effect of the elimination.
                virtual void updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability);
                virtual void updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state);
                virtual void updatePriority(storm::storage::sparse::state_type const& state);
                virtual bool filterPredecessor(storm::storage::sparse::state_type const& state);
                virtual bool isFilterPredecessor() const;
                
            protected:
                storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix;
                storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions;
            };
            
        } // namespace stateelimination
    } // namespace storage
} // namespace storm

#endif // STORM_SOLVER_STATEELIMINATION_STATEELIMINATOR_H_
