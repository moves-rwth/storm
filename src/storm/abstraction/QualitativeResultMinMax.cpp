#include "storm/abstraction/QualitativeResultMinMax.h"

#include "storm/abstraction/QualitativeResult.h"

namespace storm {
    namespace abstraction {
        
        template<storm::dd::DdType Type>
        QualitativeResult<Type> const& QualitativeResultMinMax<Type>::getProb0Min() const {
            return getProb0(storm::OptimizationDirection::Minimize);
        }
        
        template<storm::dd::DdType Type>
        QualitativeResult<Type> const& QualitativeResultMinMax<Type>::getProb1Min() const {
            return getProb1(storm::OptimizationDirection::Minimize);
        }
        
        template<storm::dd::DdType Type>
        QualitativeResult<Type> const& QualitativeResultMinMax<Type>::getProb0Max() const {
            return getProb0(storm::OptimizationDirection::Maximize);
        }
        
        template<storm::dd::DdType Type>
        QualitativeResult<Type> const& QualitativeResultMinMax<Type>::getProb1Max() const {
            return getProb1(storm::OptimizationDirection::Maximize);
        }
        
        template struct QualitativeResultMinMax<storm::dd::DdType::CUDD>;
        template struct QualitativeResultMinMax<storm::dd::DdType::Sylvan>;

    }
}
