#include "storm/models/sparse/Mdp.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"
#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
    namespace models {
        namespace sparse {

            template <typename ValueType, typename RewardModelType>
            Mdp<ValueType, RewardModelType>::Mdp(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::models::sparse::StateLabeling const& stateLabeling,
                                  std::unordered_map<std::string, RewardModelType> const& rewardModels)
                    : Mdp<ValueType, RewardModelType>(storm::storage::sparse::ModelComponents<ValueType, RewardModelType>(transitionMatrix, stateLabeling, rewardModels)) {
                // Intentionally left empty
            }
            
            template <typename ValueType, typename RewardModelType>
            Mdp<ValueType, RewardModelType>::Mdp(storm::storage::SparseMatrix<ValueType>&& transitionMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
                                  std::unordered_map<std::string, RewardModelType>&& rewardModels)
                    : Mdp<ValueType, RewardModelType>(storm::storage::sparse::ModelComponents<ValueType, RewardModelType>(std::move(transitionMatrix), std::move(stateLabeling), std::move(rewardModels))) {
                // Intentionally left empty
            }
            
            template <typename ValueType, typename RewardModelType>
            Mdp<ValueType, RewardModelType>::Mdp(storm::storage::sparse::ModelComponents<ValueType, RewardModelType> const& components)
                    : NondeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Mdp, components) {
                // Intentionally left empty
            }
            
           template <typename ValueType, typename RewardModelType>
            Mdp<ValueType, RewardModelType>::Mdp(storm::storage::sparse::ModelComponents<ValueType, RewardModelType>&& components)
                    : NondeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Mdp, std::move(components)) {
                // Intentionally left empty
            }
   

            template<typename ValueType, typename RewardModelType>
            uint_least64_t Mdp<ValueType, RewardModelType>::getChoiceIndex(storm::storage::StateActionPair const& stateactPair) const {
                return this->getNondeterministicChoiceIndices()[stateactPair.getState()]+stateactPair.getAction();
            }

            template class Mdp<double>;

#ifdef STORM_HAVE_CARL
            template class Mdp<storm::RationalNumber>;

            template class Mdp<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
            template class Mdp<storm::RationalFunction>;
#endif
        } // namespace sparse
    } // namespace models
} // namespace storm
