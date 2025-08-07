#pragma once
#include <memory>

#include "storm/models/sparse/Pomdp.h"
namespace storm {
namespace transformer {

template<typename ValueType>
class MakeStateSetObservationClosed {
   public:
    MakeStateSetObservationClosed(std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> pomdp);

    /*!
     * Ensures that the given set of states is observation closed, potentially, adding new observation(s)
     * A set of states S is observation closed, iff there is a set of observations Z such that `o(s) in Z iff s in S`
     *
     * @return the model where the given set of states is observation closed as well as the set of observations that uniquely describe the given state set.
     *      If the state set is already observation close, we return the original POMDP, i.e., the pomdp is not copied.
     */
    std::pair<std::shared_ptr<storm::models::sparse::Pomdp<ValueType>>, std::set<uint32_t>> transform(storm::storage::BitVector const& stateSet) const;

   protected:
    std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> pomdp;
};
}  // namespace transformer
}  // namespace storm