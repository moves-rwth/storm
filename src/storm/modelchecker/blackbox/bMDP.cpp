#include "storm/modelchecker/blackbox/bMDP.h"
#include "models/ModelType.h"

namespace storm {
namespace models {
namespace sparse {

template<typename BoundType, typename RewardModelType>
bMDP<BoundType, RewardModelType>::bMDP(storm::storage::SparseMatrix<BoundPair> const& transitionMatrix, storm::models::sparse::StateLabeling const& stateLabeling,
                                     std::unordered_map<std::string, RewardModelType> const& rewardModels, ModelType type)
    : bMDP<BoundType, RewardModelType>(storm::storage::sparse::ModelComponents<BoundPair, RewardModelType>(transitionMatrix, stateLabeling, rewardModels),
                                      type) {
    // Intentionally left empty
}

template<typename BoundType, typename RewardModelType>
bMDP<BoundType, RewardModelType>::bMDP(storm::storage::SparseMatrix<BoundPair>&& transitionMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
                                     std::unordered_map<std::string, RewardModelType>&& rewardModels, ModelType type)
    : bMDP<BoundType, RewardModelType>(
          storm::storage::sparse::ModelComponents<BoundPair, RewardModelType>(std::move(transitionMatrix), std::move(stateLabeling), std::move(rewardModels)),
          type) {
    // Intentionally left empty
}

template<typename BoundType, typename RewardModelType>
bMDP<BoundType, RewardModelType>::bMDP(storm::storage::sparse::ModelComponents<BoundPair, RewardModelType> const& components, ModelType type)
    : NondeterministicModel<BoundPair, RewardModelType>(type, components) {
    assert(type == storm::models::ModelType::bMDP);
    // Intentionally left empty
}

template<typename BoundType, typename RewardModelType>
bMDP<BoundType, RewardModelType>::bMDP(storm::storage::sparse::ModelComponents<BoundPair, RewardModelType>&& components, ModelType type)
    : NondeterministicModel<BoundPair, RewardModelType>(type, std::move(components)) {
    assert(type == storm::models::ModelType::bMDP);
    // Intentionally left empty
}

template class bMDP<double>;
template class bMDP<storm::RationalNumber>;

template class bMDP<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
template class bMDP<storm::RationalFunction>;
}  // namespace sparse
}  // namespace models
}  // namespace storm






