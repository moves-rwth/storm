#pragma once

#include "storm/storage/dd/DdType.h"

#include "storm/abstraction/SymbolicQualitativeResultMinMax.h"

namespace storm {
    namespace abstraction {

        template<storm::dd::DdType Type>
        class SymbolicQualitativeGameResultMinMax : public SymbolicQualitativeResultMinMax<Type> {
        public:
            SymbolicQualitativeGameResultMinMax() = default;
            
            virtual SymbolicQualitativeResult<Type> const& getProb0(storm::OptimizationDirection const& dir) const override;
            virtual SymbolicQualitativeResult<Type> const& getProb1(storm::OptimizationDirection const& dir) const override;
            
            SymbolicQualitativeResult<Type> prob0Min;
            SymbolicQualitativeResult<Type> prob1Min;
            SymbolicQualitativeResult<Type> prob0Max;
            SymbolicQualitativeResult<Type> prob1Max;
        };

    }
}
