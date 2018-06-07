#include "storm/abstraction/SymbolicQuantitativeGameResultMinMax.h"

namespace storm {
    namespace abstraction {
        
        template<storm::dd::DdType Type, typename ValueType>
        SymbolicQuantitativeGameResultMinMax<Type, ValueType>::SymbolicQuantitativeGameResultMinMax(SymbolicQuantitativeGameResult<Type, ValueType> const& min, SymbolicQuantitativeGameResult<Type, ValueType> const& max) : min(min), max(max) {
            // Intentionally left empty.
        }
        
    }
}
