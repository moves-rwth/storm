#pragma once

#include "storm/abstraction/QualitativeResult.h"

namespace storm {
    namespace abstraction {
        
        template <storm::dd::DdType Type>
        struct QualitativeMdpResult : public QualitativeResult<Type> {
            QualitativeMdpResult() = default;
            
            QualitativeMdpResult(storm::dd::Bdd<Type> const& states) : states(states) {
                // Intentionally left empty.
            }
            
            virtual storm::dd::Bdd<Type> const& getStates() const override {
                return states;
            }
            
            storm::dd::Bdd<Type> states;
        };
        
    }
}

