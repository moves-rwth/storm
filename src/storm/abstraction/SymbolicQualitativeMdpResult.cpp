#include "storm/abstraction/SymbolicQualitativeMdpResult.h"

namespace storm {
    namespace abstraction {
        
        template <storm::dd::DdType Type>
        SymbolicQualitativeMdpResult<Type>::SymbolicQualitativeMdpResult(storm::dd::Bdd<Type> const& states) : states(states) {
            // Intentionally left empty.
        }
            
        template <storm::dd::DdType Type>
        storm::dd::Bdd<Type> const& SymbolicQualitativeMdpResult<Type>::getStates() const {
            return states;
        }
        
        template class SymbolicQualitativeMdpResult<storm::dd::DdType::CUDD>;
        template class SymbolicQualitativeMdpResult<storm::dd::DdType::Sylvan>;

    }
}
