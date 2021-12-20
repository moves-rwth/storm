#pragma once

#include "storm/abstraction/ExplicitQualitativeGameResult.h"
#include "storm/abstraction/ExplicitQualitativeResultMinMax.h"

namespace storm {
namespace abstraction {

class ExplicitQualitativeGameResultMinMax : public ExplicitQualitativeResultMinMax {
   public:
    ExplicitQualitativeGameResultMinMax() = default;

    virtual ExplicitQualitativeGameResult const& getProb0(storm::OptimizationDirection const& dir) const override;
    virtual ExplicitQualitativeGameResult const& getProb1(storm::OptimizationDirection const& dir) const override;
    virtual ExplicitQualitativeGameResult& getProb0(storm::OptimizationDirection const& dir) override;
    virtual ExplicitQualitativeGameResult& getProb1(storm::OptimizationDirection const& dir) override;

    ExplicitQualitativeGameResult prob0Min;
    ExplicitQualitativeGameResult prob1Min;
    ExplicitQualitativeGameResult prob0Max;
    ExplicitQualitativeGameResult prob1Max;
};

}  // namespace abstraction
}  // namespace storm
