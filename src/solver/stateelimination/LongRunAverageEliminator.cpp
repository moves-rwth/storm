#include "src/solver/stateelimination/LongRunAverageEliminator.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            template<typename SparseModelType>
            LongRunAverageEliminator<SparseModelType>::LongRunAverageEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, PriorityQueuePointer priorityQueue, std::vector<ValueType>& stateValues, std::vector<ValueType>& averageTimeInStates) : StateEliminator<SparseModelType>(transitionMatrix, backwardTransitions), priorityQueue(priorityQueue), stateValues(stateValues), averageTimeInStates(averageTimeInStates) {
            }
            
            template<typename SparseModelType>
            void LongRunAverageEliminator<SparseModelType>::updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability) {
                stateValues[state] = storm::utility::simplify(loopProbability * stateValues[state]);
                averageTimeInStates[state] = storm::utility::simplify(loopProbability * averageTimeInStates[state]);
            }
       
            template<typename SparseModelType>
            void LongRunAverageEliminator<SparseModelType>::updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) {
                stateValues[predecessor] = storm::utility::simplify(stateValues[predecessor] + storm::utility::simplify(probability * stateValues[state]));
                averageTimeInStates[predecessor] = storm::utility::simplify(averageTimeInStates[predecessor] + storm::utility::simplify(probability * averageTimeInStates[state]));
            }
            
            template<typename SparseModelType>
            void LongRunAverageEliminator<SparseModelType>::updatePriority(storm::storage::sparse::state_type const& state) {
                priorityQueue->update(state, StateEliminator<SparseModelType>::transitionMatrix, StateEliminator<SparseModelType>::backwardTransitions, stateValues);
            }
            
            template<typename SparseModelType>
            bool LongRunAverageEliminator<SparseModelType>::filterPredecessor(storm::storage::sparse::state_type const& state) {
                STORM_LOG_ASSERT(false, "Filter should not be applied.");
                return false;
            }
            
            template<typename SparseModelType>
            bool LongRunAverageEliminator<SparseModelType>::isFilterPredecessor() const {
                return false;
            }
            
            
            template class LongRunAverageEliminator<storm::models::sparse::Dtmc<double>>;
            
#ifdef STORM_HAVE_CARL
            template class LongRunAverageEliminator<storm::models::sparse::Dtmc<storm::RationalFunction>>;
#endif
            
        } // namespace stateelimination
    } // namespace storage
} // namespace storm
