#pragma once

#include "storm/solver/OptimizationDirection.h"

#include "storm/abstraction/QualitativeResultMinMax.h"

namespace storm {
    namespace abstraction {
        class ExplicitQualitativeResult;

        class ExplicitQualitativeResultMinMax : public QualitativeResultMinMax {
        public:
            ExplicitQualitativeResultMinMax() = default;
            
            virtual bool isExplicit() const override;
            
            ExplicitQualitativeResult const& getProb0Min() const;
            ExplicitQualitativeResult const& getProb1Min() const;
            ExplicitQualitativeResult const& getProb0Max() const;
            ExplicitQualitativeResult const& getProb1Max() const;
            
            virtual ExplicitQualitativeResult const& getProb0(storm::OptimizationDirection const& dir) const = 0;
            virtual ExplicitQualitativeResult const& getProb1(storm::OptimizationDirection const& dir) const = 0;
        };
        
    }
}

