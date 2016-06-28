#include "src/solver/stateelimination/StateEliminator.h"

#include "src/adapters/CarlAdapter.h"

#include "src/storage/BitVector.h"

#include "src/utility/stateelimination.h"
#include "src/utility/macros.h"
#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidStateException.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            using namespace storm::utility::stateelimination;
            
            template<typename ValueType>
            StateEliminator<ValueType>::StateEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions) : EliminatorBase<ValueType, ScalingMode::DivideOneMinus>(transitionMatrix, backwardTransitions) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            void StateEliminator<ValueType>::eliminateState(storm::storage::sparse::state_type state, bool removeForwardTransitions) {
                STORM_LOG_TRACE("Eliminating state " << state << ".");
                this->eliminate(state, state, removeForwardTransitions);
            }
            
            template class StateEliminator<double>;
            template class StateEliminator<storm::RationalNumber>;
            template class StateEliminator<storm::RationalFunction>;
            
        } // namespace stateelimination
    } // namespace storage
} // namespace storm
