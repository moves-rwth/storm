#include "storm/models/sparse/Smg.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"
#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
    namespace models {
        namespace sparse {

            template <typename ValueType, typename RewardModelType>
            Smg<ValueType, RewardModelType>::Smg(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::models::sparse::StateLabeling const& stateLabeling,
                                  std::unordered_map<std::string, RewardModelType> const& rewardModels, ModelType type)
                    : Smg<ValueType, RewardModelType>(storm::storage::sparse::ModelComponents<ValueType, RewardModelType>(transitionMatrix, stateLabeling, rewardModels), type) {
                // Intentionally left empty
            }

            template <typename ValueType, typename RewardModelType>
            Smg<ValueType, RewardModelType>::Smg(storm::storage::SparseMatrix<ValueType>&& transitionMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
                                  std::unordered_map<std::string, RewardModelType>&& rewardModels, ModelType type)
                    : Smg<ValueType, RewardModelType>(storm::storage::sparse::ModelComponents<ValueType, RewardModelType>(std::move(transitionMatrix), std::move(stateLabeling), std::move(rewardModels)), type) {
                // Intentionally left empty
            }

            template <typename ValueType, typename RewardModelType>
            Smg<ValueType, RewardModelType>::Smg(storm::storage::sparse::ModelComponents<ValueType, RewardModelType> const& components, ModelType type)
                    : NondeterministicModel<ValueType, RewardModelType>(type, components) {
                assert(type == storm::models::ModelType::Smg);
                // Intentionally left empty
            }

           template <typename ValueType, typename RewardModelType>
            Smg<ValueType, RewardModelType>::Smg(storm::storage::sparse::ModelComponents<ValueType, RewardModelType>&& components, ModelType type)
                    : NondeterministicModel<ValueType, RewardModelType>(type, std::move(components)) {
               assert(type == storm::models::ModelType::Smg);
                // Intentionally left empty
            }

            template class Smg<double>;
            template class Smg<storm::RationalNumber>;

            template class Smg<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
            template class Smg<storm::RationalFunction>;
        } // namespace sparse
    } // namespace models
} // namespace storm
