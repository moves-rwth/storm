#pragma once

#include "storm/storage/dd/DdType.h"

#include "storm/abstraction/SymbolicQualitativeResultMinMax.h"
#include "storm/abstraction/QualitativeGameResult.h"

namespace storm {
    namespace abstraction {
        
        class ExplicitQualitativeGameResultMinMax : public QualitativeResultMinMax {
        public:
            ExplicitQualitativeGameResultMinMax() = default;
            
            virtual QualitativeResult<Type> const& getProb0(storm::OptimizationDirection const& dir) const override {
                if (dir == storm::OptimizationDirection::Minimize) {
                    return prob0Min;
                } else {
                    return prob0Max;
                }
            }
            
            virtual QualitativeResult<Type> const& getProb1(storm::OptimizationDirection const& dir) const override {
                if (dir == storm::OptimizationDirection::Minimize) {
                    return prob1Min;
                } else {
                    return prob1Max;
                }
            }
            
            QualitativeGameResult<Type> prob0Min;
            QualitativeGameResult<Type> prob1Min;
            QualitativeGameResult<Type> prob0Max;
            QualitativeGameResult<Type> prob1Max;
        };
        
    }
}

