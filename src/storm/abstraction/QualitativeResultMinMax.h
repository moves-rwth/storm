#pragma once

#include "storm/storage/dd/DdType.h"

namespace storm {
namespace abstraction {

template<storm::dd::DdType Type>
class SymbolicQualitativeResultMinMax;

class QualitativeResultMinMax {
   public:
    virtual ~QualitativeResultMinMax() = default;

    virtual bool isSymbolic() const;
    virtual bool isExplicit() const;

    template<storm::dd::DdType Type>
    SymbolicQualitativeResultMinMax<Type> const& asSymbolicQualitativeResultMinMax() const;

    template<storm::dd::DdType Type>
    SymbolicQualitativeResultMinMax<Type>& asSymbolicQualitativeResultMinMax();
};

}  // namespace abstraction
}  // namespace storm
