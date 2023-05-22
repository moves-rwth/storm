#pragma once

#include "storm/models/sparse/Pomdp.h"

namespace storm {
namespace logic {
class Formula;
}

namespace transformer {

template<typename ValueType>
class GlobalPOMDPSelfLoopEliminator {
   public:
    GlobalPOMDPSelfLoopEliminator(storm::models::sparse::Pomdp<ValueType> const& pomdp) : pomdp(pomdp) {}

    std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> transform() const;
    bool preservesFormula(storm::logic::Formula const& formula) const;
    storm::models::sparse::Pomdp<ValueType> const& pomdp;
};
}  // namespace transformer
}  // namespace storm