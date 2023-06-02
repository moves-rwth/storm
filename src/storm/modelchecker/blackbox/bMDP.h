#ifndef STORM_BMDP_H
#define STORM_BMDP_H

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/Modeltype.h"
#include "storm/models/sparse/StateLabeling.h"

#endif  // STORM_BMDP_H

namespace storm {
namespace models {
namespace sparse {

/*!
* Template class to store the bounds of the bMDP
* ValueType of bMDP
*/
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

template<class ValueType, typename RewardModelType = StandardRewardModel<ValueType>>
class bMDP : public NondeterministicModel<ValueTypePair<ValueType>, RewardModelType> {
    public:
    /*!
     * Constructs a model from the given data.
     *
     * @param transitionMatrix The matrix representing the transitions in the model.
     * @param stateLabeling The labeling of the states.
     * @param rewardModels A mapping of reward model names to reward models.
     */
    bMDP(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, StateLabeling const& stateLabeling,
        std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>(),
        ModelType type = ModelType::bMDP) {

    };

     /*!
     * Constructs a model by moving the given data.
     *
     * @param transitionMatrix The matrix representing the transitions in the model.
     * @param stateLabeling The labeling of the states.
     * @param rewardModels A mapping of reward model names to reward models.
     */
    bMDP(storm::storage::SparseMatrix<ValueType>&& transitionMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
        std::unordered_map<std::string, RewardModelType>&& rewardModels = std::unordered_map<std::string, RewardModelType>(), ModelType type = ModelType::bMDP);


    bMDP(Mdp<ValueType, RewardModelType> const& other) = default;
    bMDP& operator=(bMDP<ValueType, RewardModelType> const& other) = default;

    bMDP(Mdp<ValueType, RewardModelType>&& other) = default;
    bMDP& operator=(bMDP<ValueType, RewardModelType>&& other) = default;
    virtual ~Mdp() = default;
};

}  // namespace sparse
}  // namespace models
}  // namespace storm


