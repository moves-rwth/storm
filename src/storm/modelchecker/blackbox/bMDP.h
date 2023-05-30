#ifndef STORM_BMDP_H
#define STORM_BMDP_H

#include "storm/models/sparse/Mdp.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
namespace models {
namespace sparse {

template<typename ValueType>
class ValueTypePair {
   private:
    std::pair<ValueType,ValueType> valuePair;
   public:
    ValueType getLBound() {
        valuePair.first();
    }
    ValueType getUBound() {
        valuePair.second();
    }
    std::pair<ValueType,ValueType> operator+(ValueTypePair other) {
        return std::make_pair(getLBound() + other.getLBound(), getUBound() + other.getUBound());
    }
};

template<typename ValueType = ValueTypePair<double>, typename RewardModelType = StandardRewardModel<ValueTypePair<double>>>
class bMDP : public NondeterministicModel<ValueType, RewardModelType> {
    public:
    /*!
     * Constructs a model from the given data.
     *
     * @param transitionMatrix The matrix representing the transitions in the model.
     * @param stateLabeling The labeling of the states.
     * @param rewardModels A mapping of reward model names to reward models.
     */
    bMDP(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::models::sparse::StateLabeling const& stateLabeling,
        std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>(),
        ModelType type = ModelType::bMDP);
};
}  // namespace sparse
}  // namespace models
}  // namespace storm
#endif  // STORM_BMDP_H

