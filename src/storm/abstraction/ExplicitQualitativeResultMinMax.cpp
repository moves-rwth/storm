#include "storm/abstraction/ExplicitQualitativeResultMinMax.h"

#include "storm/abstraction/ExplicitQualitativeResult.h"

namespace storm {
    namespace abstraction {
        
        bool ExplicitQualitativeResultMinMax::isExplicit() const {
            return true;
        }
        
        ExplicitQualitativeResult const& ExplicitQualitativeResultMinMax::getProb0Min() const {
            return getProb0(storm::OptimizationDirection::Minimize);
        }
        
        ExplicitQualitativeResult const& ExplicitQualitativeResultMinMax::getProb1Min() const {
            return getProb1(storm::OptimizationDirection::Minimize);
        }
        
        ExplicitQualitativeResult const& ExplicitQualitativeResultMinMax::getProb0Max() const {
            return getProb0(storm::OptimizationDirection::Maximize);
        }
        
        ExplicitQualitativeResult const& ExplicitQualitativeResultMinMax::getProb1Max() const {
            return getProb1(storm::OptimizationDirection::Maximize);
        }
        
    }
}
