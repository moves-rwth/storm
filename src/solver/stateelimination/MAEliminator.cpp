#include "src/solver/stateelimination/MAEliminator.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            template<typename SparseModelType>
            MAEliminator<SparseModelType>::MAEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions) : StateEliminator<SparseModelType>(transitionMatrix, backwardTransitions){
            }
            
            template<typename SparseModelType>
            void MAEliminator<SparseModelType>::updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability) {
                // Do nothing
            }
       
            template<typename SparseModelType>
            void MAEliminator<SparseModelType>::updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) {
                // Do nothing
            }
            
            template<typename SparseModelType>
            void MAEliminator<SparseModelType>::updatePriority(storm::storage::sparse::state_type const& state) {
                // Do nothing
            }
            
            template<typename SparseModelType>
            bool MAEliminator<SparseModelType>::filterPredecessor(storm::storage::sparse::state_type const& state) {
                assert(false);
            }
            
            template<typename SparseModelType>
            bool MAEliminator<SparseModelType>::isFilterPredecessor() const {
                return false;
            }
            
            
            template class MAEliminator<storm::models::sparse::Dtmc<double>>;
            
#ifdef STORM_HAVE_CARL
            template class MAEliminator<storm::models::sparse::Dtmc<storm::RationalFunction>>;
#endif
            
        } // namespace stateelimination
    } // namespace storage
} // namespace storm
