#include "src/solver/stateelimination/ConditionalEliminator.h"

#include "src/utility/macros.h"
#include "src/utility/constants.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            template<typename ValueType>
            ConditionalEliminator<ValueType>::ConditionalEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, std::vector<ValueType>& oneStepProbabilities, storm::storage::BitVector& phiStates, storm::storage::BitVector& psiStates) : StateEliminator<ValueType>(transitionMatrix, backwardTransitions), oneStepProbabilities(oneStepProbabilities), phiStates(phiStates), psiStates(psiStates), filterLabel(StateLabel::NONE) {
            }
            
            template<typename ValueType>
            void ConditionalEliminator<ValueType>::updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability) {
                oneStepProbabilities[state] = storm::utility::simplify(loopProbability * oneStepProbabilities[state]);
            }
            
            template<typename ValueType>
            void ConditionalEliminator<ValueType>::updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) {
                oneStepProbabilities[predecessor] = storm::utility::simplify(oneStepProbabilities[predecessor] * storm::utility::simplify(probability * oneStepProbabilities[state]));
            }
            
            template<typename ValueType>
            void ConditionalEliminator<ValueType>::updatePriority(storm::storage::sparse::state_type const& state) {
                // Do nothing
            }
            
            template<typename ValueType>
            bool ConditionalEliminator<ValueType>::filterPredecessor(storm::storage::sparse::state_type const& state) {
                // TODO find better solution than flag
                switch (filterLabel) {
                    case StateLabel::PHI:
                        return phiStates.get(state);
                    case StateLabel::PSI:
                        return psiStates.get(state);
                    default:
                        STORM_LOG_ASSERT(false, "Specific state not set.");
                        return false;
                }
            }
            
            template<typename ValueType>
            bool ConditionalEliminator<ValueType>::isFilterPredecessor() const {
                return true;
            }
            
            template<typename ValueType>
            void ConditionalEliminator<ValueType>::setFilterPhi() {
                filterLabel = StateLabel::PHI;
            }
            
            template<typename ValueType>
            void ConditionalEliminator<ValueType>::setFilterPsi() {
                filterLabel = StateLabel::PSI;
            }
            
            template<typename ValueType>
            void ConditionalEliminator<ValueType>::setFilter(StateLabel const& stateLabel) {
                filterLabel = stateLabel;
            }
            
            template<typename ValueType>
            void ConditionalEliminator<ValueType>::unsetFilter() {
                filterLabel = StateLabel::NONE;
            }
            
            template class ConditionalEliminator<double>;
            
#ifdef STORM_HAVE_CARL
            template class ConditionalEliminator<storm::RationalFunction>;
#endif
            
        } // namespace stateelimination
    } // namespace storage
} // namespace storm
