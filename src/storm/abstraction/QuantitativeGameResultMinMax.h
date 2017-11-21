#pragma once

#include "storm/abstraction/QuantitativeGameResult.h"

namespace storm {
    namespace abstraction {
     
        template<storm::dd::DdType Type, typename ValueType>
        class QuantitativeGameResultMinMax {
        public:
            QuantitativeGameResultMinMax() = default;
            
            QuantitativeGameResultMinMax(QuantitativeGameResult<Type, ValueType> const& min, QuantitativeGameResult<Type, ValueType> const& max) : min(min), max(max) {
                // Intentionally left empty.
            }
            
            QuantitativeGameResult<Type, ValueType> min;
            QuantitativeGameResult<Type, ValueType> max;
        };
        
    }
}
