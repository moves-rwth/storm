#include "src/solver/stateelimination/PrioritizedStateEliminator.h"

#include "src/solver/stateelimination/StatePriorityQueue.h"

#include "src/utility/macros.h"
#include "src/utility/constants.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            template<typename ValueType>
            PrioritizedStateEliminator<ValueType>::PrioritizedStateEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, PriorityQueuePointer priorityQueue, std::vector<ValueType>& stateValues) : StateEliminator<ValueType>(transitionMatrix, backwardTransitions), priorityQueue(priorityQueue), stateValues(stateValues) {
            }
            
            template<typename ValueType>
            void PrioritizedStateEliminator<ValueType>::updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability) {
                stateValues[state] = storm::utility::simplify(loopProbability * stateValues[state]);
            }
       
            template<typename ValueType>
            void PrioritizedStateEliminator<ValueType>::updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) {
                stateValues[predecessor] = storm::utility::simplify(stateValues[predecessor] + storm::utility::simplify(probability * stateValues[state]));
            }
            
            template<typename ValueType>
            void PrioritizedStateEliminator<ValueType>::updatePriority(storm::storage::sparse::state_type const& state) {
                priorityQueue->update(state);
            }
            
            template class PrioritizedStateEliminator<double>;

#ifdef STORM_HAVE_CARL
            template class PrioritizedStateEliminator<storm::RationalNumber>;
            template class PrioritizedStateEliminator<storm::RationalFunction>;
#endif
        } // namespace stateelimination
    } // namespace storage
} // namespace storm
