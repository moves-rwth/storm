#include "storm-gamebased-ar/abstraction/SymbolicQualitativeGameResult.h"
#include "storm/storage/dd/sylvan/InternalSylvanBdd.h"

namespace storm::gbar {
namespace abstraction {

template<storm::dd::DdType Type>
SymbolicQualitativeGameResult<Type>::SymbolicQualitativeGameResult(storm::utility::graph::SymbolicGameProb01Result<Type> const& prob01Result)
    : storm::utility::graph::SymbolicGameProb01Result<Type>(prob01Result) {
    // Intentionally left empty.
}

template<storm::dd::DdType Type>
storm::dd::Bdd<Type> const& SymbolicQualitativeGameResult<Type>::getStates() const {
    return this->getPlayer1States();
}

#ifdef STORM_HAVE_CUDD
template class SymbolicQualitativeGameResult<storm::dd::DdType::CUDD>;
#endif

#ifdef STORM_HAVE_SYLVAN
template class SymbolicQualitativeGameResult<storm::dd::DdType::Sylvan>;
#endif

}  // namespace abstraction
}  // namespace storm::gbar
