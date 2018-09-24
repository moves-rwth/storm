#pragma once

#include "storm/storage/dd/DdType.h"

#include "storm/abstraction/QualitativeResult.h"

namespace storm {
    namespace dd {
        template <storm::dd::DdType Type>
        class Bdd;
    }
    
    namespace abstraction {
        
        template <storm::dd::DdType Type>
        class SymbolicQualitativeResult : public QualitativeResult {
        public:
            virtual ~SymbolicQualitativeResult() = default;

            virtual storm::dd::Bdd<Type> const& getStates() const = 0;
        };
        
    }
}


