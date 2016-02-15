#include "src/solver/stateelimination/PrioritizedEliminator.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            template<typename SparseModelType>
            PrioritizedEliminator<SparseModelType>::PrioritizedEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, PriorityQueuePointer priorityQueue, std::vector<ValueType>& stateValues) : StateEliminator<SparseModelType>(transitionMatrix, backwardTransitions), priorityQueue(priorityQueue), stateValues(stateValues) {
            }
            
            template<typename SparseModelType>
            void PrioritizedEliminator<SparseModelType>::updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability) {
                stateValues[state] = storm::utility::simplify(loopProbability * stateValues[state]);
            }
       
            template<typename SparseModelType>
            void PrioritizedEliminator<SparseModelType>::updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) {
                stateValues[predecessor] = storm::utility::simplify(stateValues[predecessor] + storm::utility::simplify(probability * stateValues[state]));
            }
            
            template<typename SparseModelType>
            void PrioritizedEliminator<SparseModelType>::updatePriority(storm::storage::sparse::state_type const& state) {
                priorityQueue->update(state, StateEliminator<SparseModelType>::transitionMatrix, StateEliminator<SparseModelType>::backwardTransitions, stateValues);
            }
            
            template<typename SparseModelType>
            bool PrioritizedEliminator<SparseModelType>::filterPredecessor(storm::storage::sparse::state_type const& state) {
                assert(false);
            }
            
            template<typename SparseModelType>
            bool PrioritizedEliminator<SparseModelType>::isFilterPredecessor() const {
                return false;
            }
            
            
            template class PrioritizedEliminator<storm::models::sparse::Dtmc<double>>;
            
#ifdef STORM_HAVE_CARL
            template class PrioritizedEliminator<storm::models::sparse::Dtmc<storm::RationalFunction>>;
#endif
            
        } // namespace stateelimination
    } // namespace storage
} // namespace storm
