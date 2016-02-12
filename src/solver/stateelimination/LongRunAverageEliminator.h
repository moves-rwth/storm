#ifndef STORM_SOLVER_STATEELIMINATION_LONGRUNAVERAGEELIMINATOR_H_
#define STORM_SOLVER_STATEELIMINATION_LONGRUNAVERAGEELIMINATOR_H_

#include "src/solver/stateelimination/StateEliminator.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            template<typename SparseModelType>
            class LongRunAverageEliminator : public StateEliminator<SparseModelType> {

                typedef typename SparseModelType::ValueType ValueType;

            public:
                LongRunAverageEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, storm::modelchecker::StatePriorityQueue<ValueType>& priorityQueue, std::vector<ValueType>& stateValues, std::vector<ValueType>& averageTimeInStates);
                
                // Instantiaton of Virtual methods
                void updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability) override;
                void updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) override;
                void updatePriority(storm::storage::sparse::state_type const& state) override;
                bool filterPredecessor(storm::storage::sparse::state_type const& state) override;
                bool isFilterPredecessor() const override;
                
            private:
                
                storm::modelchecker::StatePriorityQueue<ValueType>& priorityQueue;
                std::vector<ValueType>& stateValues;
                std::vector<ValueType>& averageTimeInStates;
            };
            
        } // namespace stateelimination
    } // namespace storage
} // namespace storm

#endif // STORM_SOLVER_STATEELIMINATION_LONGRUNAVERAGEELIMINATOR_H_
