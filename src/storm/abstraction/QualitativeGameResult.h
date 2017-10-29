#pragma once

#include "storm/utility/graph.h"
#include "storm/abstraction/QualitativeResult.h"

namespace storm {
    namespace abstraction {
        
        template <storm::dd::DdType Type>
        struct QualitativeGameResult : public storm::utility::graph::GameProb01Result<Type>, public QualitativeResult<Type> {
            QualitativeGameResult() = default;
            
            QualitativeGameResult(storm::utility::graph::GameProb01Result<Type> const& prob01Result) : storm::utility::graph::GameProb01Result<Type>(prob01Result) {
                // Intentionally left empty.
            }
            
            virtual storm::dd::Bdd<Type> const& getStates() const override {
                return this->getPlayer1States();
            }
            
        };
        
    }
}
