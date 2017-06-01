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
            Mdp<ValueType, RewardModelType>::Mdp(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                storm::models::sparse::StateLabeling const& stateLabeling,
                                std::unordered_map<std::string, RewardModelType> const& rewardModels,
                                boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling)
            : NondeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Mdp, transitionMatrix, stateLabeling, rewardModels, optionalChoiceLabeling) {
                STORM_LOG_THROW(transitionMatrix.isProbabilistic(), storm::exceptions::InvalidArgumentException, "The probability matrix is invalid.");
            }


            template <typename ValueType, typename RewardModelType>
            Mdp<ValueType, RewardModelType>::Mdp(storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                storm::models::sparse::StateLabeling&& stateLabeling,
                                std::unordered_map<std::string, RewardModelType>&& rewardModels,
                                boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling)
            : NondeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Mdp, std::move(transitionMatrix), std::move(stateLabeling), std::move(rewardModels), std::move(optionalChoiceLabeling)) {
                STORM_LOG_THROW(transitionMatrix.isProbabilistic(), storm::exceptions::InvalidArgumentException, "The probability matrix is invalid.");
            }

            template <typename ValueType, typename RewardModelType>
            Mdp<ValueType, RewardModelType> Mdp<ValueType, RewardModelType>::restrictChoiceLabels(LabelSet const& enabledChoiceLabels) const {
                STORM_LOG_THROW(this->hasChoiceLabeling(), storm::exceptions::InvalidArgumentException, "Restriction to label set is impossible for unlabeled model.");

                std::vector<LabelSet> const& choiceLabeling = this->getChoiceLabeling();

                storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0, this->getTransitionMatrix().getColumnCount(), 0, true, true);
                std::vector<LabelSet> newChoiceLabeling;

                // Check for each choice of each state, whether the choice labels are fully contained in the given label set.
                uint_fast64_t currentRow = 0;
                for(uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
                    bool stateHasValidChoice = false;
                    for (uint_fast64_t choice = this->getTransitionMatrix().getRowGroupIndices()[state]; choice < this->getTransitionMatrix().getRowGroupIndices()[state + 1]; ++choice) {
                        bool choiceValid = std::includes(enabledChoiceLabels.begin(), enabledChoiceLabels.end(), choiceLabeling[choice].begin(), choiceLabeling[choice].end());

                        // If the choice is valid, copy over all its elements.
                        if (choiceValid) {
                            if (!stateHasValidChoice) {
                                transitionMatrixBuilder.newRowGroup(currentRow);
                            }
                            stateHasValidChoice = true;
                            for (auto const& entry : this->getTransitionMatrix().getRow(choice)) {
                                transitionMatrixBuilder.addNextValue(currentRow, entry.getColumn(), entry.getValue());
                            }
                            newChoiceLabeling.emplace_back(choiceLabeling[choice]);
                            ++currentRow;
                        }
                    }

                    // If no choice of the current state may be taken, we insert a self-loop to the state instead.
                    if (!stateHasValidChoice) {
                        transitionMatrixBuilder.newRowGroup(currentRow);
                        transitionMatrixBuilder.addNextValue(currentRow, state, storm::utility::one<ValueType>());
                        newChoiceLabeling.emplace_back();
                        ++currentRow;
                    }
                }

                Mdp<ValueType, RewardModelType> restrictedMdp(transitionMatrixBuilder.build(), storm::models::sparse::StateLabeling(this->getStateLabeling()),
                                                              std::unordered_map<std::string, RewardModelType>(this->getRewardModels()), boost::optional<std::vector<LabelSet>>(newChoiceLabeling));

                return restrictedMdp;
            }

            template <typename ValueType, typename RewardModelType>
            Mdp<ValueType, RewardModelType> Mdp<ValueType, RewardModelType>::restrictChoices(storm::storage::BitVector const& enabledChoices) const {
                storm::storage::SparseMatrix<ValueType> restrictedTransitions = this->getTransitionMatrix().restrictRows(enabledChoices);
                std::unordered_map<std::string, RewardModelType> newRewardModels;
                for (auto const& rewardModel : this->getRewardModels()) {
                    newRewardModels.emplace(rewardModel.first, rewardModel.second.restrictActions(enabledChoices));
                }
                if(this->hasChoiceLabeling()) {
                    return Mdp<ValueType, RewardModelType>(restrictedTransitions, this->getStateLabeling(), newRewardModels, boost::optional<std::vector<LabelSet>>(storm::utility::vector::filterVector(this->getChoiceLabeling(), enabledChoices)));
                } else {
                    return Mdp<ValueType, RewardModelType>(restrictedTransitions, this->getStateLabeling(), newRewardModels, boost::optional<std::vector<LabelSet>>());
                }


            }

            template<typename ValueType, typename RewardModelType>
            uint_least64_t Mdp<ValueType, RewardModelType>::getChoiceIndex(storm::storage::StateActionPair const& stateactPair) const {
                return this->getNondeterministicChoiceIndices()[stateactPair.getState()]+stateactPair.getAction();
            }

            template class Mdp<double>;
            template class Mdp<float>;

#ifdef STORM_HAVE_CARL
            template class Mdp<storm::RationalNumber>;

            template class Mdp<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
            template class Mdp<storm::RationalFunction>;
#endif
        } // namespace sparse
    } // namespace models
} // namespace storm
