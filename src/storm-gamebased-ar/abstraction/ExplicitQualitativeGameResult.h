#pragma once

#include "storm-gamebased-ar/abstraction/ExplicitQualitativeResult.h"
#include "storm/utility/graph.h"

namespace storm::gbar {
namespace abstraction {

class ExplicitQualitativeGameResult : public storm::utility::graph::ExplicitGameProb01Result, public ExplicitQualitativeResult {
   public:
    ExplicitQualitativeGameResult() = default;

    ExplicitQualitativeGameResult(storm::utility::graph::ExplicitGameProb01Result const& prob01Result);

    virtual storm::storage::BitVector const& getStates() const override;
};

}  // namespace abstraction
}  // namespace storm::gbar
