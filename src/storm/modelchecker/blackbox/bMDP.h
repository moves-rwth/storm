#ifndef STORM_Bmdp_H
#define STORM_Bmdp_H

#include "models/sparse/NondeterministicModel.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/StateLabeling.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"

namespace storm {
namespace models {
namespace sparse {

/*!
* Template class to store the bounds of the Bmdp
* ValueType of Bmdp
*/
template<typename ValueType>
class ValueTypePair {
   private:
    std::pair<ValueType,ValueType> valuePair;
   public:

    ValueTypePair(std::pair<ValueType,ValueType> valuePair): valuePair{valuePair}{}
    ValueTypePair(std::pair<ValueType,ValueType> &&valuePair): valuePair{std::move(valuePair)}{}


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

template<class BoundType, typename RewardModelType = StandardRewardModel<BoundType>>
class Bmdp : public NondeterministicModel<BoundType, RewardModelType> {

    using ValueType = BoundType;

    public:
     /*!
     * Constructs a model from the given data.
     *
     * @param transitionMatrix The matrix representing the transitions in the model.
     * @param stateLabeling The labeling of the states.
     * @param rewardModels A mapping of reward model names to reward models.
     */
    Bmdp(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, StateLabeling const& stateLabeling,
        std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>(),
        ModelType type = ModelType::Bmdp);

     /*!
     * Constructs a model by moving the given data.
     *
     * @param transitionMatrix The matrix representing the transitions in the model.
     * @param stateLabeling The labeling of the states.
     * @param rewardModels A mapping of reward model names to reward models.
     */
    Bmdp(storm::storage::SparseMatrix<ValueType>&& transitionMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
        std::unordered_map<std::string, RewardModelType>&& rewardModels = std::unordered_map<std::string, RewardModelType>(), ModelType type = ModelType::Bmdp);

    /*!
     * Constructs a model from the given data.
     *
     * @param components The components for this model.
     */
    Bmdp(storm::storage::sparse::ModelComponents<ValueType, RewardModelType> const& components, ModelType type = ModelType::Bmdp);
    Bmdp(storm::storage::sparse::ModelComponents<ValueType, RewardModelType>&& components, ModelType type = ModelType::Bmdp);

    Bmdp(Bmdp<BoundType, RewardModelType> const& other) = default;
    Bmdp& operator=(Bmdp<BoundType, RewardModelType> const& other) = default;

    Bmdp(Bmdp<BoundType, RewardModelType>&& other) = default;
    Bmdp& operator=(Bmdp<BoundType, RewardModelType>&& other) = default;
    virtual ~Bmdp() = default;
};

}  // namespace sparse
}  // namespace models
}  // namespace storm

#endif  // STORM_Bmdp_H

