#pragma once

#include "storm/solver/OptimizationDirection.h"

#include "storm/storage/dd/DdType.h"

namespace storm {
    namespace dd {
        template<storm::dd::DdType Type>
        class Bdd;
    }
    
    namespace abstraction {
        template<storm::dd::DdType Type>
        struct QualitativeResult;
        
        template<storm::dd::DdType Type>
        struct QualitativeResultMinMax {
        public:
            QualitativeResultMinMax() = default;

            QualitativeResult<Type> const& getProb0Min() const;
            QualitativeResult<Type> const& getProb1Min() const;
            QualitativeResult<Type> const& getProb0Max() const;
            QualitativeResult<Type> const& getProb1Max() const;
            
            virtual QualitativeResult<Type> const& getProb0(storm::OptimizationDirection const& dir) const = 0;
            virtual QualitativeResult<Type> const& getProb1(storm::OptimizationDirection const& dir) const = 0;
        };
    }
}
