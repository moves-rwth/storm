#include "src/solver/stateelimination/LongRunAverageEliminator.h"

#include "src/utility/constants.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            template<typename ValueType>
            LongRunAverageEliminator<ValueType>::LongRunAverageEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, PriorityQueuePointer priorityQueue, std::vector<ValueType>& stateValues, std::vector<ValueType>& averageTimeInStates) : PrioritizedStateEliminator<ValueType>(transitionMatrix, backwardTransitions, priorityQueue, stateValues), averageTimeInStates(averageTimeInStates) {
            }
            
            template<typename ValueType>
            void LongRunAverageEliminator<ValueType>::updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability) {
                this->stateValues[state] = storm::utility::simplify(loopProbability * this->stateValues[state]);
                averageTimeInStates[state] = storm::utility::simplify(loopProbability * averageTimeInStates[state]);
            }
       
            template<typename ValueType>
            void LongRunAverageEliminator<ValueType>::updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) {
                this->stateValues[predecessor] = storm::utility::simplify(this->stateValues[predecessor] + storm::utility::simplify(probability * this->stateValues[state]));
                averageTimeInStates[predecessor] = storm::utility::simplify(averageTimeInStates[predecessor] + storm::utility::simplify(probability * averageTimeInStates[state]));
            }
            
            template class LongRunAverageEliminator<double>;

#ifdef STORM_HAVE_CARL
            template class LongRunAverageEliminator<storm::RationalNumber>;
            template class LongRunAverageEliminator<storm::RationalFunction>;
#endif
        } // namespace stateelimination
    } // namespace storage
} // namespace storm
