#include "storm/models/sparse/Mdp.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"
#include "storm/adapters/CarlAdapter.h"

#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
    namespace models {
        namespace sparse {

            template <typename ValueType, typename RewardModelType>
            Mdp<ValueType, RewardModelType>::Mdp(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                storm::models::sparse::StateLabeling const& stateLabeling,
                                std::unordered_map<std::string, RewardModelType> const& rewardModels,
                                boost::optional<storm::models::sparse::ChoiceLabeling> const& optionalChoiceLabeling)
            : NondeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Mdp, transitionMatrix, stateLabeling, rewardModels, optionalChoiceLabeling) {
                STORM_LOG_THROW(transitionMatrix.isProbabilistic(), storm::exceptions::InvalidArgumentException, "The probability matrix is invalid.");
            }


            template <typename ValueType, typename RewardModelType>
            Mdp<ValueType, RewardModelType>::Mdp(storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                storm::models::sparse::StateLabeling&& stateLabeling,
                                std::unordered_map<std::string, RewardModelType>&& rewardModels,
                                boost::optional<storm::models::sparse::ChoiceLabeling>&& optionalChoiceLabeling)
            : NondeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Mdp, std::move(transitionMatrix), std::move(stateLabeling), std::move(rewardModels), std::move(optionalChoiceLabeling)) {
                STORM_LOG_THROW(transitionMatrix.isProbabilistic(), storm::exceptions::InvalidArgumentException, "The probability matrix is invalid.");
            }

            template <typename ValueType, typename RewardModelType>
            Mdp<ValueType, RewardModelType> Mdp<ValueType, RewardModelType>::restrictChoices(storm::storage::BitVector const& enabledChoices) const {
                storm::storage::SparseMatrix<ValueType> restrictedTransitions = this->getTransitionMatrix().restrictRows(enabledChoices);
                std::unordered_map<std::string, RewardModelType> newRewardModels;
                for (auto const& rewardModel : this->getRewardModels()) {
                    newRewardModels.emplace(rewardModel.first, rewardModel.second.restrictActions(enabledChoices));
                }
                if(this->hasChoiceLabeling()) {
                    return Mdp<ValueType, RewardModelType>(restrictedTransitions, this->getStateLabeling(), newRewardModels, this->getChoiceLabeling().getSubLabeling(enabledChoices));
                } else {
                    return Mdp<ValueType, RewardModelType>(restrictedTransitions, this->getStateLabeling(), newRewardModels);
                }
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
