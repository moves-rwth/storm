#include "storm/models/sparse/Pomdp.h"

namespace storm {
    namespace models {
        namespace sparse {

            template <typename ValueType, typename RewardModelType>
            Pomdp<ValueType, RewardModelType>::Pomdp(storm::storage::SparseMatrix<ValueType> const &transitionMatrix, storm::models::sparse::StateLabeling const &stateLabeling, std::unordered_map <std::string, RewardModelType> const &rewardModels) : Mdp<ValueType, RewardModelType>(transitionMatrix, stateLabeling, rewardModels) {
                // Intentionally left blank.
            }

            template <typename ValueType, typename RewardModelType>
            Pomdp<ValueType, RewardModelType>::Pomdp(storm::storage::SparseMatrix<ValueType> &&transitionMatrix, storm::models::sparse::StateLabeling &&stateLabeling, std::unordered_map <std::string, RewardModelType> &&rewardModels) : Mdp<ValueType, RewardModelType>(transitionMatrix, stateLabeling, rewardModels) {
                // Intentionally left empty.
            }

            template <typename ValueType, typename RewardModelType>
            Pomdp<ValueType, RewardModelType>::Pomdp(storm::storage::sparse::ModelComponents<ValueType, RewardModelType> const &components) : Mdp<ValueType, RewardModelType>(components) {

            }

            template <typename ValueType, typename RewardModelType>
            Pomdp<ValueType, RewardModelType>::Pomdp(storm::storage::sparse::ModelComponents<ValueType, RewardModelType> &&components): Mdp<ValueType, RewardModelType>(components) {

            }


            template class Pomdp<double>;
            template class Pomdp<storm::RationalNumber>;
            template class Pomdp<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
            template class Pomdp<storm::RationalFunction>;

        }
    }
}