#include "src/solver/stateelimination/ConditionalEliminator.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            template<typename SparseModelType>
            ConditionalEliminator<SparseModelType>::ConditionalEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, std::vector<ValueType>& oneStepProbabilities, storm::storage::BitVector& phiStates, storm::storage::BitVector& psiStates) : StateEliminator<SparseModelType>(transitionMatrix, backwardTransitions), oneStepProbabilities(oneStepProbabilities), phiStates(phiStates), psiStates(psiStates), specificState(NONE) {
            }
            
            template<typename SparseModelType>
            void ConditionalEliminator<SparseModelType>::updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability) {
                oneStepProbabilities[state] = storm::utility::simplify(loopProbability * oneStepProbabilities[state]);
            }
            
            template<typename SparseModelType>
            void ConditionalEliminator<SparseModelType>::updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) {
                oneStepProbabilities[predecessor] = storm::utility::simplify(oneStepProbabilities[predecessor] * storm::utility::simplify(probability * oneStepProbabilities[state]));
            }
            
            template<typename SparseModelType>
            void ConditionalEliminator<SparseModelType>::updatePriority(storm::storage::sparse::state_type const& state) {
                // Do nothing
            }
            
            template<typename SparseModelType>
            bool ConditionalEliminator<SparseModelType>::filterPredecessor(storm::storage::sparse::state_type const& state) {
                // TODO find better solution than flag
                switch (specificState) {
                    case PHI:
                        return phiStates.get(state);
                    case PSI:
                        return psiStates.get(state);
                    default:
                        STORM_LOG_ASSERT(false, "Specific state not set.");
                        return false;
                }
            }
            
            template<typename SparseModelType>
            bool ConditionalEliminator<SparseModelType>::isFilterPredecessor() const {
                return true;
            }
            
            
            template class ConditionalEliminator<storm::models::sparse::Dtmc<double>>;
            
#ifdef STORM_HAVE_CARL
            template class ConditionalEliminator<storm::models::sparse::Dtmc<storm::RationalFunction>>;
#endif
            
        } // namespace stateelimination
    } // namespace storage
} // namespace storm
