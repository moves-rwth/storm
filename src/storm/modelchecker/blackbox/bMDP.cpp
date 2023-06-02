#include "storm/modelchecker/blackbox/bMDP.h"

namespace storm {
namespace models {
namespace sparse {

template<typename ValueType, typename RewardModelType>
bMDP<ValueType, RewardModelType>::bMDP(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::models::sparse::StateLabeling const& stateLabeling,
        std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>(),
        storm::models::ModelType type = ModelType::bMDP) {
    // Intentionally left empty
};

template<typename ValueType, typename RewardModelType>
bMDP<ValueType, RewardModelType>::bMDP(storm::storage::SparseMatrix<ValueType>&& transitionMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
                                     std::unordered_map<std::string, RewardModelType>&& rewardModels, ModelType type)
    : bMDP<ValueType, RewardModelType>(
          storm::storage::sparse::ModelComponents<ValueType, RewardModelType>(std::move(transitionMatrix), std::move(stateLabeling), std::move(rewardModels)),
          type) {
    // Intentionally left empty
};

template class bMDP<double>;
template class bMDP<storm::RationalNumber>;

template class bMDP<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
template class bMDP<storm::RationalFunction>;
}  // namespace sparse
}  // namespace models
}  // namespace storm






