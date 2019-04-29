#include "storm/modelchecker/multiobjective/preprocessing/SparseMultiObjectiveRewardAnalysis.h"

#include <algorithm>
#include <set>

#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/prctl/helper/BaierUpperRewardBoundsComputer.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/transformer/EndComponentEliminator.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"


#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            namespace preprocessing {
                
                
                template<typename SparseModelType>
                typename SparseMultiObjectiveRewardAnalysis<SparseModelType>::ReturnType SparseMultiObjectiveRewardAnalysis<SparseModelType>::analyze(storm::modelchecker::multiobjective::preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType> const& preprocessorResult) {
                    ReturnType result;
                    auto backwardTransitions = preprocessorResult.preprocessedModel->getBackwardTransitions();
                    
                    setReward0States(result, preprocessorResult, backwardTransitions);
                    checkRewardFiniteness(result, preprocessorResult, backwardTransitions);
                    
                    return result;
                }
                
                template<typename SparseModelType>
                void SparseMultiObjectiveRewardAnalysis<SparseModelType>::setReward0States(ReturnType& result, storm::modelchecker::multiobjective::preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType> const& preprocessorResult, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {
                    
                    uint_fast64_t stateCount = preprocessorResult.preprocessedModel->getNumberOfStates();
                    auto const& transitions = preprocessorResult.preprocessedModel->getTransitionMatrix();
                    std::vector<uint_fast64_t> const& groupIndices = transitions.getRowGroupIndices();
                    storm::storage::BitVector allStates(stateCount, true);
    
                    // Get the choices that yield non-zero reward
                    storm::storage::BitVector zeroRewardChoices(preprocessorResult.preprocessedModel->getNumberOfChoices(), true);
                    for (auto const& obj : preprocessorResult.objectives) {
                        if (obj.formula->isRewardOperatorFormula()) {
                            STORM_LOG_WARN_COND(obj.formula->getSubformula().isTotalRewardFormula() || obj.formula->getSubformula().isCumulativeRewardFormula(), "Analyzing reachability reward formulas is not supported properly.");
                            auto const& rewModel = preprocessorResult.preprocessedModel->getRewardModel(obj.formula->asRewardOperatorFormula().getRewardModelName());
                            zeroRewardChoices &= rewModel.getChoicesWithZeroReward(transitions);
                        }
                    }
                    
                    // Get the states that have reward for at least one (or for all) choices assigned to it.
                    storm::storage::BitVector statesWithRewardForOneChoice = storm::storage::BitVector(stateCount, false);
                    storm::storage::BitVector statesWithRewardForAllChoices = storm::storage::BitVector(stateCount, true);
                    for (uint_fast64_t state = 0; state < stateCount; ++state) {
                        bool stateHasChoiceWithReward = false;
                        bool stateHasChoiceWithoutReward = false;
                        uint_fast64_t const& groupEnd = groupIndices[state + 1];
                        for (uint_fast64_t choice = groupIndices[state]; choice < groupEnd; ++choice) {
                            if (zeroRewardChoices.get(choice)) {
                                stateHasChoiceWithoutReward = true;
                            } else {
                                stateHasChoiceWithReward = true;
                            }
                        }
                        if (stateHasChoiceWithReward) {
                            statesWithRewardForOneChoice.set(state, true);
                        }
                        if (stateHasChoiceWithoutReward) {
                            statesWithRewardForAllChoices.set(state, false);
                        }
                    }
                    
                    // get the states for which there is a scheduler yielding reward zero
                    result.reward0EStates = storm::utility::graph::performProbGreater0A(transitions, groupIndices, backwardTransitions, allStates, statesWithRewardForAllChoices, false, 0, zeroRewardChoices);
                    result.reward0EStates.complement();
                    result.reward0AStates = storm::utility::graph::performProb0A(backwardTransitions, allStates, statesWithRewardForOneChoice);
                    assert(result.reward0AStates.isSubsetOf(result.reward0EStates));
                }
             
                template<typename SparseModelType>
                void SparseMultiObjectiveRewardAnalysis<SparseModelType>::checkRewardFiniteness(ReturnType& result, storm::modelchecker::multiobjective::preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType> const& preprocessorResult, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {
                    
                    result.rewardFinitenessType = RewardFinitenessType::AllFinite;
                    
                    auto const& transitions = preprocessorResult.preprocessedModel->getTransitionMatrix();
                    std::vector<uint_fast64_t> const& groupIndices = transitions.getRowGroupIndices();
                    
                    storm::storage::BitVector maxRewardsToCheck(preprocessorResult.preprocessedModel->getNumberOfChoices(), true);
                    storm::storage::BitVector minRewardsToCheck(preprocessorResult.preprocessedModel->getNumberOfChoices(), true);
                    for (auto const& objIndex : preprocessorResult.maybeInfiniteRewardObjectives) {
                        STORM_LOG_ASSERT(preprocessorResult.objectives[objIndex].formula->isRewardOperatorFormula(), "Objective needs to be checked for finite reward but has no reward operator.");
                        auto const& rewModel = preprocessorResult.preprocessedModel->getRewardModel(preprocessorResult.objectives[objIndex].formula->asRewardOperatorFormula().getRewardModelName());
                        auto unrelevantChoices = rewModel.getChoicesWithZeroReward(transitions);
                        // For (upper) reward bounded cumulative reward formulas, we do not need to consider the choices where boundReward is collected.
                        if (preprocessorResult.objectives[objIndex].formula->getSubformula().isCumulativeRewardFormula()) {
                            auto const& timeBoundReference = preprocessorResult.objectives[objIndex].formula->getSubformula().asCumulativeRewardFormula().getTimeBoundReference();
                            // Only reward bounded formulas need a finiteness check
                            assert(timeBoundReference.isRewardBound());
                            auto const& rewModelOfBound = preprocessorResult.preprocessedModel->getRewardModel(timeBoundReference.getRewardName());
                            unrelevantChoices |= ~rewModelOfBound.getChoicesWithZeroReward(transitions);
                        }
                        if (storm::solver::minimize(preprocessorResult.objectives[objIndex].formula->getOptimalityType())) {
                            minRewardsToCheck &= unrelevantChoices;
                        } else {
                            maxRewardsToCheck &= unrelevantChoices;
                        }
                    }
                    maxRewardsToCheck.complement();
                    minRewardsToCheck.complement();
                    
                    // Check reward finiteness under all schedulers
                    storm::storage::BitVector allStates(preprocessorResult.preprocessedModel->getNumberOfStates(), true);
                    if (storm::utility::graph::checkIfECWithChoiceExists(transitions, backwardTransitions, allStates, maxRewardsToCheck | minRewardsToCheck)) {
                        // Check whether there is a scheduler yielding infinite reward for a maximizing objective
                        if (storm::utility::graph::checkIfECWithChoiceExists(transitions, backwardTransitions, allStates, maxRewardsToCheck)) {
                            result.rewardFinitenessType = RewardFinitenessType::Infinite;
                        } else {
                            // Check whether there is a scheduler under which all rewards are finite.
                            result.rewardLessInfinityEStates = storm::utility::graph::performProb1E(transitions, groupIndices, backwardTransitions, allStates, result.reward0EStates);
                            if ((result.rewardLessInfinityEStates.get() & preprocessorResult.preprocessedModel->getInitialStates()).empty()) {
                                // There is no scheduler that induces finite reward for the initial state
                                result.rewardFinitenessType = RewardFinitenessType::Infinite;
                            } else {
                                result.rewardFinitenessType = RewardFinitenessType::ExistsParetoFinite;
                            }
                        }
                    } else {
                        result.rewardLessInfinityEStates = allStates;
                    }
                }
            
                template<typename SparseModelType>
                void SparseMultiObjectiveRewardAnalysis<SparseModelType>::computeUpperResultBound(SparseModelType const& model, storm::modelchecker::multiobjective::Objective<ValueType>& objective, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {
                    STORM_LOG_INFO_COND(!objective.upperResultBound.is_initialized(), "Tried to find an upper result bound for an objective, but a result bound is already there.");
                    
                    if (model.isOfType(storm::models::ModelType::Mdp)) {
                        
                        auto const& transitions = model.getTransitionMatrix();
                        
                        if (objective.formula->isRewardOperatorFormula()) {
                            auto const& rewModel = model.getRewardModel(objective.formula->asRewardOperatorFormula().getRewardModelName());
                            auto actionRewards = rewModel.getTotalRewardVector(transitions);
                            
                            if (objective.formula->getSubformula().isTotalRewardFormula() || objective.formula->getSubformula().isCumulativeRewardFormula()) {
                                // We have to eliminate ECs here to treat zero-reward ECs
         
                                storm::storage::BitVector allStates(model.getNumberOfStates(), true);
                                
                                // Get the set of states from which no reward is reachable
                                auto nonZeroRewardStates = rewModel.getStatesWithZeroReward(transitions);
                                nonZeroRewardStates.complement();
                                auto expRewGreater0EStates = storm::utility::graph::performProbGreater0E(backwardTransitions, allStates, nonZeroRewardStates);
                                
                                auto zeroRewardChoices = rewModel.getChoicesWithZeroReward(transitions);
                                
                                auto ecElimRes = storm::transformer::EndComponentEliminator<ValueType>::transform(transitions, expRewGreater0EStates, zeroRewardChoices, ~allStates);
                                
                                allStates.resize(ecElimRes.matrix.getRowGroupCount());
                                storm::storage::BitVector outStates(allStates.size(), false);
                                std::vector<ValueType> rew0StateProbs;
                                rew0StateProbs.reserve(ecElimRes.matrix.getRowCount());
                                for (uint64_t state = 0; state < allStates.size(); ++ state) {
                                    for (uint64_t choice = ecElimRes.matrix.getRowGroupIndices()[state]; choice < ecElimRes.matrix.getRowGroupIndices()[state + 1]; ++choice) {
                                        // Check whether the choice lead to a state with expRew 0 in the original model
                                        bool isOutChoice = false;
                                        uint64_t originalModelChoice = ecElimRes.newToOldRowMapping[choice];
                                        for (auto const& entry : transitions.getRow(originalModelChoice)) {
                                            if (!expRewGreater0EStates.get(entry.getColumn())) {
                                                isOutChoice = true;
                                                outStates.set(state, true);
                                                rew0StateProbs.push_back(storm::utility::one<ValueType>() - ecElimRes.matrix.getRowSum(choice));
                                                assert (!storm::utility::isZero(rew0StateProbs.back()));
                                                break;
                                            }
                                        }
                                        if (!isOutChoice) {
                                            rew0StateProbs.push_back(storm::utility::zero<ValueType>());
                                        }
                                    }
                                }
                                
                                // An upper reward bound can only be computed if it is below infinity
                                if (storm::utility::graph::performProb1A(ecElimRes.matrix, ecElimRes.matrix.getRowGroupIndices(), ecElimRes.matrix.transpose(true), allStates, outStates).full()) {
                                    
                                    std::vector<ValueType> rewards;
                                    rewards.reserve(ecElimRes.matrix.getRowCount());
                                    for (auto row : ecElimRes.newToOldRowMapping) {
                                        rewards.push_back(actionRewards[row]);
                                    }
                                    
                                    storm::modelchecker::helper::BaierUpperRewardBoundsComputer<ValueType> baier(ecElimRes.matrix, rewards, rew0StateProbs);
                                    if (objective.upperResultBound) {
                                        objective.upperResultBound = std::min(objective.upperResultBound.get(), baier.computeUpperBound());
                                    } else {
                                        objective.upperResultBound = baier.computeUpperBound();
                                    }
                                }
                            }
                        }
                        
                        if (objective.upperResultBound) {
                            STORM_LOG_INFO("Computed upper result bound " << objective.upperResultBound.get() << " for objective " << *objective.formula << ".");
                        } else {
                            STORM_LOG_WARN("Could not compute upper result bound for objective " << *objective.formula);
                        }
                    }
                }
    
                
                template class SparseMultiObjectiveRewardAnalysis<storm::models::sparse::Mdp<double>>;
                template class SparseMultiObjectiveRewardAnalysis<storm::models::sparse::MarkovAutomaton<double>>;
                
                template class SparseMultiObjectiveRewardAnalysis<storm::models::sparse::Mdp<storm::RationalNumber>>;
                template class SparseMultiObjectiveRewardAnalysis<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
            }
        }
    }
}
