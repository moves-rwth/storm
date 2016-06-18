#include "src/solver/stateelimination/DynamicStatePriorityQueue.h"

#include "src/adapters/CarlAdapter.h"

#include "src/utility/macros.h"
#include "src/utility/constants.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            template<typename ValueType>
            DynamicStatePriorityQueue<ValueType>::DynamicStatePriorityQueue(std::vector<std::pair<storm::storage::sparse::state_type, uint_fast64_t>> const& sortedStatePenaltyPairs, storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities, PenaltyFunctionType const& penaltyFunction) : StatePriorityQueue(), transitionMatrix(transitionMatrix), backwardTransitions(backwardTransitions), oneStepProbabilities(oneStepProbabilities), priorityQueue(), stateToPriorityMapping(), penaltyFunction(penaltyFunction) {
                // Insert all state-penalty pairs into our priority queue.
                for (auto const& statePenalty : sortedStatePenaltyPairs) {
                    priorityQueue.insert(priorityQueue.end(), statePenalty);
                }
                
                // Insert all state-penalty pairs into auxiliary mapping.
                for (auto const& statePenalty : sortedStatePenaltyPairs) {
                    stateToPriorityMapping.emplace(statePenalty);
                }
            }
            
            template<typename ValueType>
            bool DynamicStatePriorityQueue<ValueType>::hasNext() const {
                return !priorityQueue.empty();
            }
            
            template<typename ValueType>
            storm::storage::sparse::state_type DynamicStatePriorityQueue<ValueType>::pop() {
                auto it = priorityQueue.begin();
                STORM_LOG_TRACE("Popping state " << it->first << " with priority " << it->second << ".");
                storm::storage::sparse::state_type result = it->first;
                priorityQueue.erase(priorityQueue.begin());
                return result;
            }
            
            template<typename ValueType>
            void DynamicStatePriorityQueue<ValueType>::update(storm::storage::sparse::state_type state) {
                // First, we need to find the priority until now.
                auto priorityIt = stateToPriorityMapping.find(state);
                
                // If the priority queue does not store the priority of the given state, we must not update it.
                if (priorityIt == stateToPriorityMapping.end()) {
                    return;
                }
                uint_fast64_t lastPriority = priorityIt->second;
                
                uint_fast64_t newPriority = penaltyFunction(state, transitionMatrix, backwardTransitions, oneStepProbabilities);
                
                if (lastPriority != newPriority) {
                    // Erase and re-insert into the priority queue with the new priority.
                    auto queueIt = priorityQueue.find(std::make_pair(state, lastPriority));
                    priorityQueue.erase(queueIt);
                    priorityQueue.emplace(state, newPriority);
                    
                    // Finally, update the probability in the mapping.
                    priorityIt->second = newPriority;
                }
            }
            
            template<typename ValueType>
            std::size_t DynamicStatePriorityQueue<ValueType>::size() const {
                return priorityQueue.size();
            }

            template class DynamicStatePriorityQueue<double>;
            
#ifdef STORM_HAVE_CARL
            template class DynamicStatePriorityQueue<storm::RationalFunction>;
#endif
        }
    }
}
