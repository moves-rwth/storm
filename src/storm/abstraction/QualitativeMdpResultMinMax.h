#pragma once

#include "storm/storage/dd/DdType.h"

#include "storm/abstraction/QualitativeMdpResult.h"

namespace storm {
    namespace abstraction {
        
        template<storm::dd::DdType Type>
        struct QualitativeMdpResultMinMax {
        public:
            QualitativeMdpResultMinMax() = default;
            
            QualitativeMdpResult<Type> min;
            QualitativeMdpResult<Type> max;
        };
        
    }
}

