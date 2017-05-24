#pragma once

#include <boost/optional.hpp>
#include <memory>

#include "storm/logic/Formula.h"
#include "storm/logic/Bound.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/vector.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <typename SparseModelType>
            class SparseMultiObjectivePreprocessorTask {
            public:
                SparseMultiObjectivePreprocessorTask(std::shared_ptr<Objective<typename SparseModelType::ValueType>> const& objective) : objective(objective) {
                    // intentionally left empty
                }
                
                virtual void perform(SparseModelType& preprocessedModel) const = 0;
                
                virtual bool requiresEndComponentAnalysis() const {
                    return false;
                }
                
                
            protected:
                std::shared_ptr<Objective<typename SparseModelType::ValueType>> objective;
            };
            
            // Transforms reachability probabilities to total expected rewards by adding a rewardModel
            // such that one reward is given whenever a goal state is reached from a relevant state
            template <typename SparseModelType>
            class SparseMultiObjectivePreprocessorReachProbToTotalRewTask : public SparseMultiObjectivePreprocessorTask<SparseModelType> {
            public:
                SparseMultiObjectivePreprocessorReachProbToTotalRewTask(std::shared_ptr<Objective<typename SparseModelType::ValueType>> const& objective, std::shared_ptr<storm::logic::Formula const> const& relevantStateFormula, std::shared_ptr<storm::logic::Formula const> const& goalStateFormula) : SparseMultiObjectivePreprocessorTask<SparseModelType>(objective), relevantStateFormula(relevantStateFormula), goalStateFormula(goalStateFormula) {
                    // Intentionally left empty
                }
                
                virtual void perform(SparseModelType& preprocessedModel) const override  {
                       
                    // build stateAction reward vector that gives (one*transitionProbability) reward whenever a transition leads from a relevantState to a goalState
                    storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> mc(preprocessedModel);
                    storm::storage::BitVector relevantStates = mc.check(*relevantStateFormula)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                    storm::storage::BitVector goalStates = mc.check(*goalStateFormula)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                    
                    std::vector<typename SparseModelType::ValueType> objectiveRewards(preprocessedModel.getTransitionMatrix().getRowCount(), storm::utility::zero<typename SparseModelType::ValueType>());
                    for (auto const& state : relevantStates) {
                        for (uint_fast64_t row = preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state]; row < preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state + 1]; ++row) {
                            objectiveRewards[row] = preprocessedModel.getTransitionMatrix().getConstrainedRowSum(row, goalStates);
                        }
                    }
                    STORM_LOG_ASSERT(this->objective->rewardModelName.is_initialized(), "No reward model name has been specified");
                    preprocessedModel.addRewardModel(this->objective->rewardModelName.get(), typename SparseModelType::RewardModelType(boost::none, std::move(objectiveRewards)));
                }
                        
            private:
                std::shared_ptr<storm::logic::Formula const> relevantStateFormula;
                std::shared_ptr<storm::logic::Formula const> goalStateFormula;
            };
            
            // Transforms expected reachability rewards to total expected rewards by adding a rewardModel
            // such that non-relevant states get reward zero
            template <typename SparseModelType>
            class SparseMultiObjectivePreprocessorReachRewToTotalRewTask : public SparseMultiObjectivePreprocessorTask<SparseModelType> {
            public:
                SparseMultiObjectivePreprocessorReachRewToTotalRewTask(std::shared_ptr<Objective<typename SparseModelType::ValueType>> const& objective, std::shared_ptr<storm::logic::Formula const> const& relevantStateFormula, std::string const& originalRewardModelName) : SparseMultiObjectivePreprocessorTask<SparseModelType>(objective), relevantStateFormula(relevantStateFormula), originalRewardModelName(originalRewardModelName) {
                    // Intentionally left empty
                }
                
                virtual void perform(SparseModelType& preprocessedModel) const override  {
                       
                    // build stateAction reward vector that only gives reward for relevant states
                    storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> mc(preprocessedModel);
                    storm::storage::BitVector nonRelevantStates = ~mc.check(*relevantStateFormula)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                    typename SparseModelType::RewardModelType objectiveRewards = preprocessedModel.getRewardModel(originalRewardModelName);
                    objectiveRewards.reduceToStateBasedRewards(preprocessedModel.getTransitionMatrix(), false);
                    if (objectiveRewards.hasStateRewards()) {
                        storm::utility::vector::setVectorValues(objectiveRewards.getStateRewardVector(), nonRelevantStates, storm::utility::zero<typename SparseModelType::ValueType>());
                    }
                    if (objectiveRewards.hasStateActionRewards()) {
                        for (auto state : nonRelevantStates) {
                            std::fill_n(objectiveRewards.getStateActionRewardVector().begin() + preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state], preprocessedModel.getTransitionMatrix().getRowGroupSize(state), storm::utility::zero<typename SparseModelType::ValueType>());
                        }
                    }
                    STORM_LOG_ASSERT(this->objective->rewardModelName.is_initialized(), "No reward model name has been specified");
                    preprocessedModel.addRewardModel(this->objective->rewardModelName.get(), std::move(objectiveRewards));
                }
                        
            private:
                std::shared_ptr<storm::logic::Formula const> relevantStateFormula;
                std::string originalRewardModelName;
            };
                        
            // Transforms expected reachability time to total expected rewards by adding a rewardModel
            // such that every time step done from a relevant state yields one reward
            template <typename SparseModelType>
            class SparseMultiObjectivePreprocessorReachTimeToTotalRewTask : public SparseMultiObjectivePreprocessorTask<SparseModelType> {
            public:
                SparseMultiObjectivePreprocessorReachTimeToTotalRewTask(std::shared_ptr<Objective<typename SparseModelType::ValueType>> const& objective, std::shared_ptr<storm::logic::Formula const> const& relevantStateFormula) : SparseMultiObjectivePreprocessorTask<SparseModelType>(objective), relevantStateFormula(relevantStateFormula) {
                    // Intentionally left empty
                }
                
                virtual void perform(SparseModelType& preprocessedModel) const override  {
                       
                    // build stateAction reward vector that only gives reward for relevant states
                    storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> mc(preprocessedModel);
                    storm::storage::BitVector relevantStates = mc.check(*relevantStateFormula)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                    
                    std::vector<typename SparseModelType::ValueType> timeRewards(preprocessedModel.getNumberOfStates(), storm::utility::zero<typename SparseModelType::ValueType>());
                    storm::utility::vector::setVectorValues(timeRewards, dynamic_cast<storm::models::sparse::MarkovAutomaton<typename SparseModelType::ValueType> const&>(preprocessedModel).getMarkovianStates() & relevantStates, storm::utility::one<typename SparseModelType::ValueType>());
                    STORM_LOG_ASSERT(this->objective->rewardModelName.is_initialized(), "No reward model name has been specified");
                    preprocessedModel.addRewardModel(this->objective->rewardModelName.get(), typename SparseModelType::RewardModelType(std::move(timeRewards)));
                }
                        
            private:
                std::shared_ptr<storm::logic::Formula const> relevantStateFormula;
            };
            
        }
    }
}

