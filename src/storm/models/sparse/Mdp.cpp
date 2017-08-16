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
   
            template <typename ValueType, typename RewardModelType>
            Mdp<ValueType, RewardModelType> Mdp<ValueType, RewardModelType>::restrictChoices(storm::storage::BitVector const& enabledChoices) const {
                storm::storage::sparse::ModelComponents<ValueType, RewardModelType> newComponents(this->getTransitionMatrix().restrictRows(enabledChoices));
                newComponents.stateLabeling = this->getStateLabeling();
                for (auto const& rewardModel : this->getRewardModels()) {
                    newComponents.rewardModels.emplace(rewardModel.first, rewardModel.second.restrictActions(enabledChoices));
                }
                if (this->hasChoiceLabeling()) {
                    newComponents.choiceLabeling = this->getChoiceLabeling().getSubLabeling(enabledChoices);
                }
                newComponents.stateValuations = this->getOptionalStateValuations();
                if (this->hasChoiceOrigins()) {
                    newComponents.choiceOrigins = this->getChoiceOrigins()->selectChoices(enabledChoices);
                }
                return Mdp<ValueType, RewardModelType>(std::move(newComponents));
            }

            template<typename ValueType, typename RewardModelType>
            uint_least64_t Mdp<ValueType, RewardModelType>::getChoiceIndex(storm::storage::StateActionPair const& stateactPair) const {
                return this->getNondeterministicChoiceIndices()[stateactPair.getState()]+stateactPair.getAction();
            }

            template class Mdp<double>;
            template class Mdp<storm::RationalNumber>;

            template class Mdp<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
            template class Mdp<storm::RationalFunction>;
        } // namespace sparse
    } // namespace models
} // namespace storm
