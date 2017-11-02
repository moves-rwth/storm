#pragma once

#include "storm/storage/dd/DdType.h"

#include "storm/abstraction/SymbolicQualitativeResultMinMax.h"
#include "storm/abstraction/QualitativeMdpResult.h"

namespace storm {
    namespace abstraction {
        
        template<storm::dd::DdType Type>
        class QualitativeMdpResultMinMax : public SymbolicQualitativeResultMinMax<Type> {
        public:
            QualitativeMdpResultMinMax() = default;
            
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
            
            QualitativeMdpResult<Type> prob0Min;
            QualitativeMdpResult<Type> prob1Min;
            QualitativeMdpResult<Type> prob0Max;
            QualitativeMdpResult<Type> prob1Max;
        };
        
    }
}

