#include "src/models/sparse/Mdp.h"

#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            template <typename ValueType>
            Mdp<ValueType>::Mdp(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                storm::models::sparse::StateLabeling const& stateLabeling,
                                boost::optional<std::vector<ValueType>> const& optionalStateRewardVector,
                                boost::optional<storm::storage::SparseMatrix<ValueType>> const& optionalTransitionRewardMatrix,
                                boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling)
            : NondeterministicModel<ValueType>(storm::models::ModelType::Mdp, transitionMatrix, stateLabeling, optionalStateRewardVector, optionalTransitionRewardMatrix, optionalChoiceLabeling) {
                STORM_LOG_THROW(this->checkValidityOfProbabilityMatrix(), storm::exceptions::InvalidArgumentException, "The probability matrix is invalid.");
                STORM_LOG_THROW(!this->hasTransitionRewards() || this->getTransitionRewardMatrix().isSubmatrixOf(this->getTransitionMatrix()), storm::exceptions::InvalidArgumentException, "The transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
            }
            
            
            template <typename ValueType>
            Mdp<ValueType>::Mdp(storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                storm::models::sparse::StateLabeling&& stateLabeling,
                                boost::optional<std::vector<ValueType>>&& optionalStateRewardVector,
                                boost::optional<storm::storage::SparseMatrix<ValueType>>&& optionalTransitionRewardMatrix,
                                boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling)
            : NondeterministicModel<ValueType>(storm::models::ModelType::Mdp, std::move(transitionMatrix), std::move(stateLabeling), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix), std::move(optionalChoiceLabeling)) {
                STORM_LOG_THROW(this->checkValidityOfProbabilityMatrix(), storm::exceptions::InvalidArgumentException, "The probability matrix is invalid.");
                STORM_LOG_THROW(!this->hasTransitionRewards() || this->getTransitionRewardMatrix().isSubmatrixOf(this->getTransitionMatrix()), storm::exceptions::InvalidArgumentException, "The transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
            }
            
            template <typename ValueType>
            Mdp<ValueType> Mdp<ValueType>::restrictChoiceLabels(LabelSet const& enabledChoiceLabels) const {
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
                
                Mdp<ValueType> restrictedMdp(transitionMatrixBuilder.build(), storm::models::sparse::StateLabeling(this->getStateLabeling()),
                                             this->hasStateRewards() ? boost::optional<std::vector<ValueType>>(this->getStateRewardVector()) : boost::optional<std::vector<ValueType>>(),
                                             this->hasTransitionRewards() ? boost::optional<storm::storage::SparseMatrix<ValueType>>(this->getTransitionRewardMatrix()) : boost::optional<storm::storage::SparseMatrix<ValueType>>(),
                                             boost::optional<std::vector<LabelSet>>(newChoiceLabeling));
                
                return restrictedMdp;
            }
            
            template <typename ValueType>
            Mdp<ValueType> Mdp<ValueType>::restrictActions(storm::storage::BitVector const& enabledActions) const {
                storm::storage::SparseMatrix<ValueType> restrictedTransitions = this->getTransitionMatrix().restrictRows(enabledActions);
                if(this->hasTransitionRewards()) {
                    return Mdp<ValueType>(restrictedTransitions, this->getStateLabeling(), this->getOptionalStateRewardVector(), boost::optional<storm::storage::SparseMatrix<ValueType>>(this->getTransitionRewardMatrix().restrictRows(enabledActions)), this->getOptionalChoiceLabeling());
                } else {
                    return Mdp<ValueType>( restrictedTransitions, this->getStateLabeling(), this->getOptionalStateRewardVector(), boost::optional<storm::storage::SparseMatrix<ValueType>>(), this->getOptionalChoiceLabeling());
                }
                
            }
            
            template <typename ValueType>
            bool Mdp<ValueType>::checkValidityOfProbabilityMatrix() const {
                storm::utility::ConstantsComparator<ValueType> comparator;
                // Get the settings object to customize linear solving.
                for (uint_fast64_t row = 0; row < this->getTransitionMatrix().getRowCount(); row++) {
                    ValueType sum = this->getTransitionMatrix().getRowSum(row);
                    
                    // If the sum is not a constant, for example for parametric models, we cannot check whether the sum is one or not.
                    if (!comparator.isConstant(sum)) {
                        continue;
                    }
                    
                    if (!comparator.isOne(sum))  {
                        return false;
                    }
                }
                return true;
            }
            
            template class Mdp<double>;
            template class Mdp<float>;

#ifdef STORM_HAVE_CARL
            template class Mdp<storm::RationalFunction>;
#endif

        } // namespace sparse
    } // namespace models
} // namespace storm