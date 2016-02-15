#ifndef STORM_SOLVER_STATEELIMINATION_PRIORITIZEDELIMINATOR_H_
#define STORM_SOLVER_STATEELIMINATION_PRIORITIZEDELIMINATOR_H_

#include "src/solver/stateelimination/StateEliminator.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            template<typename SparseModelType>
            class PrioritizedEliminator : public StateEliminator<SparseModelType> {

                typedef typename SparseModelType::ValueType ValueType;
                typedef typename std::shared_ptr<storm::modelchecker::StatePriorityQueue<ValueType>> PriorityQueuePointer;

            public:
                PrioritizedEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, PriorityQueuePointer priorityQueue, std::vector<ValueType>& stateValues);
                
                // Instantiaton of Virtual methods
                void updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability) override;
                void updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) override;
                void updatePriority(storm::storage::sparse::state_type const& state) override;
                bool filterPredecessor(storm::storage::sparse::state_type const& state) override;
                bool isFilterPredecessor() const override;
                
            private:
                PriorityQueuePointer priorityQueue;
                std::vector<ValueType>& stateValues;
            };
            
        } // namespace stateelimination
    } // namespace storage
} // namespace storm

#endif // STORM_SOLVER_STATEELIMINATION_PRIORITIZEDELIMINATOR_H_
