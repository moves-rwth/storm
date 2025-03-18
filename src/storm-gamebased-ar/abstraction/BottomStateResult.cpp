#include "storm-gamebased-ar/abstraction/BottomStateResult.h"
#include "storm/storage/dd/sylvan/InternalSylvanBdd.h"

namespace storm::gbar {
namespace abstraction {

template<storm::dd::DdType DdType>
BottomStateResult<DdType>::BottomStateResult(storm::dd::Bdd<DdType> const& states, storm::dd::Bdd<DdType> const& transitions)
    : states(states), transitions(transitions) {
    // Intentionally left empty.
}

#ifdef STORM_HAVE_CUDD
template struct BottomStateResult<storm::dd::DdType::CUDD>;
#endif

#ifdef STORM_HAVE_SYLVAN
template struct BottomStateResult<storm::dd::DdType::Sylvan>;
#endif
}  // namespace abstraction
}  // namespace storm::gbar
