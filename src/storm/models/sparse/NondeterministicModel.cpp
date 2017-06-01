#include "storm/models/sparse/NondeterministicModel.h"

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            template<typename ValueType, typename RewardModelType>
            NondeterministicModel<ValueType, RewardModelType>::NondeterministicModel(storm::models::ModelType const& modelType,
                                                                    storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                    storm::models::sparse::StateLabeling const& stateLabeling,
                                                                    std::unordered_map<std::string, RewardModelType> const& rewardModels,
                                                                    boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling)
            : Model<ValueType, RewardModelType>(modelType, transitionMatrix, stateLabeling, rewardModels, optionalChoiceLabeling) {
                // Intentionally left empty.
            }
            
            template<typename ValueType, typename RewardModelType>
            NondeterministicModel<ValueType, RewardModelType>::NondeterministicModel(storm::models::ModelType const& modelType,
                                                                    storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                                                    storm::models::sparse::StateLabeling&& stateLabeling,
                                                                    std::unordered_map<std::string, RewardModelType>&& rewardModels,
                                                                    boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling)
            : Model<ValueType, RewardModelType>(modelType, std::move(transitionMatrix), std::move(stateLabeling), std::move(rewardModels),
                               std::move(optionalChoiceLabeling)) {
                // Intentionally left empty.
            }
            
            template<typename ValueType, typename RewardModelType>
            uint_fast64_t NondeterministicModel<ValueType, RewardModelType>::getNumberOfChoices() const {
                return this->getTransitionMatrix().getRowCount();
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<uint_fast64_t> const& NondeterministicModel<ValueType, RewardModelType>::getNondeterministicChoiceIndices() const {
                return this->getTransitionMatrix().getRowGroupIndices();
            }
            
            template<typename ValueType, typename RewardModelType>
            uint_fast64_t NondeterministicModel<ValueType, RewardModelType>::getNumberOfChoices(uint_fast64_t state) const {
                auto indices = this->getNondeterministicChoiceIndices();
                return indices[state+1] - indices[state];
            }
            
            template<typename ValueType, typename RewardModelType>
            void NondeterministicModel<ValueType, RewardModelType>::modifyStateActionRewards(RewardModelType& rewardModel, std::map<std::pair<uint_fast64_t, LabelSet>, typename RewardModelType::ValueType> const& modifications) const {
                STORM_LOG_THROW(rewardModel.hasStateActionRewards(), storm::exceptions::InvalidOperationException, "Cannot modify state-action rewards, because the reward model does not have state-action rewards.");
                STORM_LOG_THROW(this->hasChoiceLabeling(), storm::exceptions::InvalidOperationException, "Cannot modify state-action rewards, because the model does not have an action labeling.");
                std::vector<LabelSet> const& choiceLabels = this->getChoiceLabeling();
                for (auto const& modification : modifications) {
                    uint_fast64_t stateIndex = modification.first.first;
                    for (uint_fast64_t row = this->getNondeterministicChoiceIndices()[stateIndex]; row < this->getNondeterministicChoiceIndices()[stateIndex + 1]; ++row) {
                        // If the action label of the row matches the requested one, we set the reward value accordingly.
                        if (choiceLabels[row] == modification.first.second) {
                            rewardModel.setStateActionRewardValue(row, modification.second);
                        }
                    }
                }
            }

            template<typename ValueType, typename RewardModelType>
            template<typename T>
            void NondeterministicModel<ValueType, RewardModelType>::modifyStateActionRewards(std::string const& modelName, std::map<uint_fast64_t, T> const& modifications) {
                RewardModelType& rewardModel = this->rewardModel(modelName);
                size_t i = 0;
                for(auto const& mod : modifications) {
                    std::cout << i++ << "/" << modifications.size() << std::endl;
                    rewardModel.setStateActionReward(mod.first, mod.second);
                }
            }

            template<typename ValueType, typename RewardModelType>
            template<typename T>
            void NondeterministicModel<ValueType, RewardModelType>::modifyStateRewards(std::string const& modelName, std::map<uint_fast64_t, T> const& modifications) {
                RewardModelType& rewardModel = this->rewardModel(modelName);
                for(auto const& mod : modifications) {
                    rewardModel.setStateReward(mod.first, mod.second);
                }
            }
            
            template<typename ValueType, typename RewardModelType>
            void NondeterministicModel<ValueType, RewardModelType>::reduceToStateBasedRewards() {
                for (auto& rewardModel : this->getRewardModels()) {
                    rewardModel.second.reduceToStateBasedRewards(this->getTransitionMatrix(), false);
                }
            }
            
            template<typename ValueType, typename RewardModelType>
            void NondeterministicModel<ValueType, RewardModelType>::printModelInformationToStream(std::ostream& out) const {
                this->printModelInformationHeaderToStream(out);
                out << "Choices: \t" << this->getNumberOfChoices() << std::endl;
                this->printModelInformationFooterToStream(out);
            }
            
            template<typename ValueType, typename RewardModelType>
            void NondeterministicModel<ValueType, RewardModelType>::writeDotToStream(std::ostream& outStream, bool includeLabeling, storm::storage::BitVector const* subsystem, std::vector<ValueType> const* firstValue, std::vector<ValueType> const* secondValue, std::vector<uint_fast64_t> const* stateColoring, std::vector<std::string> const* colors, std::vector<uint_fast64_t>* scheduler, bool finalizeOutput) const {
                Model<ValueType, RewardModelType>::writeDotToStream(outStream, includeLabeling, subsystem, firstValue, secondValue, stateColoring, colors, scheduler, false);
                
                // Write the probability distributions for all the states.
                for (uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
                    uint_fast64_t rowCount = this->getNondeterministicChoiceIndices()[state + 1] - this->getNondeterministicChoiceIndices()[state];
                    bool highlightChoice = true;
                    
                    // For this, we need to iterate over all available nondeterministic choices in the current state.
                    for (uint_fast64_t choice = 0; choice < rowCount; ++choice) {
                        uint_fast64_t rowIndex = this->getNondeterministicChoiceIndices()[state] + choice;
                        typename storm::storage::SparseMatrix<ValueType>::const_rows row = this->getTransitionMatrix().getRow(rowIndex);
                        
                        if (scheduler != nullptr) {
                            // If the scheduler picked the current choice, we will not make it dotted, but highlight it.
                            if ((*scheduler)[state] == choice) {
                                highlightChoice = true;
                            } else {
                                highlightChoice = false;
                            }
                        }
                        
                        // For each nondeterministic choice, we draw an arrow to an intermediate node to better display
                        // the grouping of transitions.
                        outStream << "\t\"" << state << "c" << choice << "\" [shape = \"point\"";
                        
                        // If we were given a scheduler to highlight, we do so now.
                        if (scheduler != nullptr) {
                            if (highlightChoice) {
                                outStream << ", fillcolor=\"red\"";
                            }
                        }
                        outStream << "];" << std::endl;
                        
                        outStream << "\t" << state << " -> \"" << state << "c" << choice << "\" [ label= \"" << rowIndex << "\"";
                        
                        // If we were given a scheduler to highlight, we do so now.
                        if (scheduler != nullptr) {
                            if (highlightChoice) {
                                outStream << ", color=\"red\", penwidth = 2";
                            } else {
                                outStream << ", style = \"dotted\"";
                            }
                        }
                        outStream << "];" << std::endl;
                        
                        // Now draw all probabilitic arcs that belong to this nondeterminstic choice.
                        for (auto const& transition : row) {
                            if (subsystem == nullptr || subsystem->get(transition.getColumn())) {
                                outStream << "\t\"" << state << "c" << choice << "\" -> " << transition.getColumn() << " [ label= \"" << transition.getValue() << "\" ]";
                                
                                // If we were given a scheduler to highlight, we do so now.
                                if (scheduler != nullptr) {
                                    if (highlightChoice) {
                                        outStream << " [color=\"red\", penwidth = 2]";
                                    } else {
                                        outStream << " [style = \"dotted\"]";
                                    }
                                }
                                outStream << ";" << std::endl;
                            }
                        }
                    }
                }
                
                if (finalizeOutput) {
                    outStream << "}" << std::endl;
                }
            }
            
            template class NondeterministicModel<double>;
            template class NondeterministicModel<float>;

#ifdef STORM_HAVE_CARL
            template class NondeterministicModel<storm::RationalNumber>;

            template class NondeterministicModel<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
            template void NondeterministicModel<double, storm::models::sparse::StandardRewardModel<storm::Interval>>::modifyStateActionRewards(std::string const& modelName, std::map<uint_fast64_t, double> const& modifications);
            template void NondeterministicModel<double, storm::models::sparse::StandardRewardModel<storm::Interval>>::modifyStateRewards(std::string const& modelName, std::map<uint_fast64_t, double> const& modifications);

            template class NondeterministicModel<storm::RationalFunction>;
#endif
        }
    }
}
