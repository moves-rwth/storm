#pragma once

#include "storm/storage/dd/DdType.h"

namespace storm {
    namespace dd {
        template <storm::dd::DdType Type>
        class Bdd;
    }
    
    namespace abstraction {
        
        template <storm::dd::DdType Type>
        struct QualitativeResult {
            virtual storm::dd::Bdd<Type> const& getStates() const = 0;
        };
        
    }
}


