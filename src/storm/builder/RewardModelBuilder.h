#pragma once

#include <cstdint>
#include <vector>

#include "storm/builder/RewardModelInformation.h"

namespace storm {
namespace models {
namespace sparse {
template<typename ValueType>
class StandardRewardModel;
}
}  // namespace models
namespace builder {

/*!
 * A structure that is used to keep track of a reward model currently being built.
 */
template<typename ValueType>
class RewardModelBuilder {
   public:
    RewardModelBuilder(RewardModelInformation const& rewardModelInformation);

    storm::models::sparse::StandardRewardModel<ValueType> build(uint_fast64_t rowCount, uint_fast64_t columnCount, uint_fast64_t rowGroupCount);

    std::string const& getName() const;

    void addStateReward(ValueType const& value);

    void addStateActionReward(ValueType const& value);

    bool hasStateRewards() const;

    bool hasStateActionRewards() const;

   private:
    std::string rewardModelName;

    bool stateRewards;
    bool stateActionRewards;

    // The state reward vector.
    std::vector<ValueType> stateRewardVector;

    // The state-action reward vector.
    std::vector<ValueType> stateActionRewardVector;
};

}  // namespace builder
}  // namespace storm
