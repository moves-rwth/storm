#pragma once

#include "storm/storage/dd/DdType.h"

#include "storm/abstraction/SymbolicQualitativeGameResult.h"
#include "storm/abstraction/SymbolicQualitativeResultMinMax.h"

namespace storm {
namespace abstraction {

template<storm::dd::DdType Type>
class SymbolicQualitativeGameResultMinMax : public SymbolicQualitativeResultMinMax<Type> {
   public:
    SymbolicQualitativeGameResultMinMax() = default;

    virtual SymbolicQualitativeResult<Type> const& getProb0(storm::OptimizationDirection const& dir) const override;
    virtual SymbolicQualitativeResult<Type> const& getProb1(storm::OptimizationDirection const& dir) const override;

    SymbolicQualitativeGameResult<Type> prob0Min;
    SymbolicQualitativeGameResult<Type> prob1Min;
    SymbolicQualitativeGameResult<Type> prob0Max;
    SymbolicQualitativeGameResult<Type> prob1Max;
};

}  // namespace abstraction
}  // namespace storm
