#pragma once

#include "storm/models/sparse/NondeterministicModel.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
namespace transformer {

template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
class ChoiceSelector {
   public:
    ChoiceSelector(storm::models::sparse::NondeterministicModel<ValueType, RewardModelType> const& inputModel) : inputModel(inputModel) {}

    /*!
     * Constructs an MDP by copying the current MDP and restricting the choices of each state to the ones given by the bitvector.
     *
     * @param enabledActions A BitVector of lenght numberOfChoices(), which is one iff the action should be kept.
     * @return A subMDP.
     */
    std::shared_ptr<storm::models::sparse::NondeterministicModel<ValueType, RewardModelType>> transform(storm::storage::BitVector const& enabledActions) const;

   private:
    storm::models::sparse::NondeterministicModel<ValueType, RewardModelType> const& inputModel;
};

}  // namespace transformer
}  // namespace storm