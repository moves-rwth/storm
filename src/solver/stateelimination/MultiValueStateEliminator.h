#pragma once
#include "src/solver/stateelimination/PrioritizedStateEliminator.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            class StatePriorityQueue;

            template<typename ValueType>
            class MultiValueStateEliminator : public PrioritizedStateEliminator<ValueType> {
            public:
                typedef typename std::shared_ptr<StatePriorityQueue> PriorityQueuePointer;
                typedef typename std::vector<ValueType> ValueTypeVector;

                MultiValueStateEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, PriorityQueuePointer priorityQueue, std::vector<ValueType>& stateValues, std::vector<ValueType>& additionalStateValues);
                
                // Instantiaton of virtual methods.
                void updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability) override;
                void updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) override;
                
            private:
                std::vector<std::reference_wrapper<ValueTypeVector>>additionalStateValues;
            };
            
        } // namespace stateelimination
    } // namespace storage
} // namespace storm

