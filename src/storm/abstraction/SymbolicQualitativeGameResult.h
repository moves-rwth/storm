#pragma once

#include "storm/abstraction/SymbolicQualitativeResult.h"
#include "storm/utility/graph.h"

namespace storm {
namespace abstraction {

template<storm::dd::DdType Type>
class SymbolicQualitativeGameResult : public storm::utility::graph::SymbolicGameProb01Result<Type>, public SymbolicQualitativeResult<Type> {
   public:
    SymbolicQualitativeGameResult() = default;

    SymbolicQualitativeGameResult(storm::utility::graph::SymbolicGameProb01Result<Type> const& prob01Result);

    virtual storm::dd::Bdd<Type> const& getStates() const override;
};

}  // namespace abstraction
}  // namespace storm
