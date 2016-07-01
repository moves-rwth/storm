#ifndef STORM_SOLVER_STATEELIMINATION_PRIORITIZEDSTATEELIMINATOR_H_
#define STORM_SOLVER_STATEELIMINATION_PRIORITIZEDSTATEELIMINATOR_H_

#include "src/solver/stateelimination/StateEliminator.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            class StatePriorityQueue;
            
            template<typename ValueType>
            class PrioritizedStateEliminator : public StateEliminator<ValueType> {
            public:
                typedef typename std::shared_ptr<StatePriorityQueue> PriorityQueuePointer;

                PrioritizedStateEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, PriorityQueuePointer priorityQueue, std::vector<ValueType>& stateValues);
                
                // Instantiaton of virtual methods.
                void updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability) override;
                void updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) override;
                void updatePriority(storm::storage::sparse::state_type const& state) override;
                
            protected:
                PriorityQueuePointer priorityQueue;
                std::vector<ValueType>& stateValues;
            };
            
        } // namespace stateelimination
    } // namespace storage
} // namespace storm

#endif // STORM_SOLVER_STATEELIMINATION_PRIORITIZEDSTATEELIMINATOR_H_
