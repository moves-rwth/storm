#ifndef STORM_SOLVER_STATEELIMINATION_LONGRUNAVERAGEELIMINATOR_H_
#define STORM_SOLVER_STATEELIMINATION_LONGRUNAVERAGEELIMINATOR_H_

#include "src/solver/stateelimination/PrioritizedStateEliminator.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            class StatePriorityQueue;

            template<typename ValueType>
            class LongRunAverageEliminator : public PrioritizedStateEliminator<ValueType> {
            public:
                typedef typename std::shared_ptr<StatePriorityQueue> PriorityQueuePointer;

                LongRunAverageEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, PriorityQueuePointer priorityQueue, std::vector<ValueType>& stateValues, std::vector<ValueType>& averageTimeInStates);
                
                // Instantiaton of virtual methods.
                void updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability) override;
                void updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) override;
                
            private:
                std::vector<ValueType>& averageTimeInStates;
            };
            
        } // namespace stateelimination
    } // namespace storage
} // namespace storm

#endif // STORM_SOLVER_STATEELIMINATION_LONGRUNAVERAGEELIMINATOR_H_
