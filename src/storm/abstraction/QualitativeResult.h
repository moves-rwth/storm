#pragma once

#include "storm/storage/dd/DdType.h"

namespace storm {
    namespace dd {
        template <storm::dd::DdType Type>
        class Bdd;
    }
    
    namespace abstraction {
        
        template <storm::dd::DdType Type>
        class QualitativeResult {
        public:
            virtual ~QualitativeResult() = default;

            virtual storm::dd::Bdd<Type> const& getStates() const = 0;
        };
        
    }
}


