#pragma once

#include "storm/abstraction/QuantitativeResult.h"

namespace storm {
    namespace abstraction {
     
        template<storm::dd::DdType Type, typename ValueType>
        struct QuantitativeResultMinMax {
            QuantitativeResultMinMax() = default;
            
            QuantitativeResultMinMax(QuantitativeResult<Type, ValueType> const& min, QuantitativeResult<Type, ValueType> const& max) : min(min), max(max) {
                // Intentionally left empty.
            }
            
            QuantitativeResult<Type, ValueType> min;
            QuantitativeResult<Type, ValueType> max;
        };
        
    }
}
