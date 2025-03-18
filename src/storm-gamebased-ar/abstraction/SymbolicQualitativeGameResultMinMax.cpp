#include "storm-gamebased-ar/abstraction/SymbolicQualitativeGameResultMinMax.h"

namespace storm::gbar {
namespace abstraction {

template<storm::dd::DdType Type>
SymbolicQualitativeResult<Type> const& SymbolicQualitativeGameResultMinMax<Type>::getProb0(storm::OptimizationDirection const& dir) const {
    if (dir == storm::OptimizationDirection::Minimize) {
        return prob0Min;
    } else {
        return prob0Max;
    }
}

template<storm::dd::DdType Type>
SymbolicQualitativeResult<Type> const& SymbolicQualitativeGameResultMinMax<Type>::getProb1(storm::OptimizationDirection const& dir) const {
    if (dir == storm::OptimizationDirection::Minimize) {
        return prob1Min;
    } else {
        return prob1Max;
    }
}

#ifdef STORM_HAVE_CUDD
template class SymbolicQualitativeGameResultMinMax<storm::dd::DdType::CUDD>;
#endif

#ifdef STORM_HAVE_SYLVAN
template class SymbolicQualitativeGameResultMinMax<storm::dd::DdType::Sylvan>;
#endif

}  // namespace abstraction
}  // namespace storm::gbar
