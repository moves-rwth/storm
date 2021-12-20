#pragma once

#include "storm/storage/dd/DdType.h"

namespace storm {
namespace abstraction {

template<storm::dd::DdType Type>
class SymbolicQualitativeResult;

class ExplicitQualitativeResult;

class QualitativeResult {
   public:
    virtual ~QualitativeResult() = default;

    virtual bool isSymbolic() const;
    virtual bool isExplicit() const;

    template<storm::dd::DdType Type>
    SymbolicQualitativeResult<Type>& asSymbolicQualitativeResult();
    template<storm::dd::DdType Type>
    SymbolicQualitativeResult<Type> const& asSymbolicQualitativeResult() const;

    ExplicitQualitativeResult& asExplicitQualitativeResult();
    ExplicitQualitativeResult const& asExplicitQualitativeResult() const;
};

}  // namespace abstraction
}  // namespace storm
