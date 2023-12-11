#pragma once

#include "storm-gamebased-ar/abstraction/SymbolicQuantitativeGameResult.h"

namespace storm::gbar {
namespace abstraction {

template<storm::dd::DdType Type, typename ValueType>
class SymbolicQuantitativeGameResultMinMax {
   public:
    SymbolicQuantitativeGameResultMinMax() = default;

    SymbolicQuantitativeGameResultMinMax(SymbolicQuantitativeGameResult<Type, ValueType> const& min,
                                         SymbolicQuantitativeGameResult<Type, ValueType> const& max);

    SymbolicQuantitativeGameResult<Type, ValueType> min;
    SymbolicQuantitativeGameResult<Type, ValueType> max;
};

}  // namespace abstraction
}  // namespace storm::gbar
