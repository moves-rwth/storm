 #include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePreprocessor.h"

#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/storage/MaximalEndComponentDecomposition.h"
#include "src/transformer/StateDuplicator.h"
#include "src/transformer/SubsystemBuilder.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"

#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template<typename SparseModelType>
            typename SparseMultiObjectivePreprocessor<SparseModelType>::PreprocessorData SparseMultiObjectivePreprocessor<SparseModelType>::preprocess(storm::logic::MultiObjectiveFormula const& originalFormula, SparseModelType const& originalModel) {
                
                PreprocessorData data(originalFormula, originalModel, SparseModelType(originalModel), storm::utility::vector::buildVectorForRange(0, originalModel.getNumberOfStates()));
                data.objectivesSolvedInPreprocessing = storm::storage::BitVector(originalFormula.getNumberOfSubformulas(), false);
                // get a unique name for the labels of states that have to be reached with probability 1 and add the label
                data.prob1StatesLabel = "prob1";
                while(data.preprocessedModel.hasLabel(data.prob1StatesLabel)) {
                    data.prob1StatesLabel = "_" + data.prob1StatesLabel;
                }
                data.preprocessedModel.getStateLabeling().addLabel(data.prob1StatesLabel);
                
                //Invoke preprocessing on the individual objectives
                for(auto const& subFormula : originalFormula.getSubFormulas()){
                    STORM_LOG_DEBUG("Preprocessing objective " << *subFormula<< ".");
                    data.objectives.emplace_back();
                    ObjectiveInformation& currentObjective = data.objectives.back();
                    currentObjective.originalFormula = subFormula;
                    if(currentObjective.originalFormula->isOperatorFormula()) {
                        preprocessFormula(currentObjective.originalFormula->asOperatorFormula(), data, currentObjective);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Could not preprocess the subformula " << *subFormula << " of " << originalFormula << " because it is not supported");
                    }
                }
                
                //We can now remove all original reward models to save some memory
                std::set<std::string> origRewardModels = originalFormula.getReferencedRewardModels();
                for (auto const& rewModel : origRewardModels){
                    data.preprocessedModel.removeRewardModel(rewModel);
                }
                
                assertRewardFiniteness(data);
                
                data.objectives = storm::utility::vector::filterVector(data.objectives, ~data.objectivesSolvedInPreprocessing);
                
                //Set the query type. In case of a numerical query, also set the index of the objective to be optimized.
                storm::storage::BitVector objectivesWithoutThreshold(data.objectives.size());
                for(uint_fast64_t objIndex = 0; objIndex < data.objectives.size(); ++objIndex) {
                    objectivesWithoutThreshold.set(objIndex, !data.objectives[objIndex].threshold);
                }
                uint_fast64_t numOfObjectivesWithoutThreshold = objectivesWithoutThreshold.getNumberOfSetBits();
                if(numOfObjectivesWithoutThreshold == 0) {
                    data.queryType = PreprocessorData::QueryType::Achievability;
                } else if (numOfObjectivesWithoutThreshold == 1) {
                    data.queryType = PreprocessorData::QueryType::Numerical;
                    data.indexOfOptimizingObjective = objectivesWithoutThreshold.getNextSetIndex(0);
                } else if (numOfObjectivesWithoutThreshold == data.objectives.size()) {
                    data.queryType = PreprocessorData::QueryType::Pareto;
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "The number of objecties without threshold is not valid. It should be either 0 (achievabilityQuery), 1 (numericalQuery), or " << data.objectives.size() << " (paretoQuery). Got " << numOfObjectivesWithoutThreshold << " instead.");
                }
                return data;
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::OperatorFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective) {
                
                // Get a unique name for the new reward model.
                currentObjective.rewardModelName = "objective" + std::to_string(data.objectives.size());
                while(data.preprocessedModel.hasRewardModel(currentObjective.rewardModelName)){
                    currentObjective.rewardModelName = "_" + currentObjective.rewardModelName;
                }
                
                currentObjective.toOriginalValueTransformationFactor = storm::utility::one<ValueType>();
                currentObjective.toOriginalValueTransformationOffset = storm::utility::zero<ValueType>();
                currentObjective.rewardsArePositive = true;
                
                bool formulaMinimizes = false;
                if(formula.hasBound()) {
                    currentObjective.threshold = storm::utility::convertNumber<ValueType>(formula.getBound().threshold);
                    currentObjective.thresholdIsStrict = storm::logic::isStrict(formula.getBound().comparisonType);
                    //Note that we minimize for upper bounds since we are looking for the EXISTENCE of a satisfying scheduler
                    formulaMinimizes = !storm::logic::isLowerBound(formula.getBound().comparisonType);
                } else if (formula.hasOptimalityType()){
                    formulaMinimizes = storm::solver::minimize(formula.getOptimalityType());
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Current objective " << formula << " does not specify whether to minimize or maximize");
                }
                if(formulaMinimizes) {
                    // We negate all the values so we can consider the maximum for this objective
                    // Thus, all objectives will be maximized.
                    currentObjective.rewardsArePositive = false;
                    currentObjective.toOriginalValueTransformationFactor = -storm::utility::one<ValueType>();
                }
                
                if(formula.isProbabilityOperatorFormula()){
                    preprocessFormula(formula.asProbabilityOperatorFormula(), data, currentObjective);
                } else if(formula.isRewardOperatorFormula()){
                    preprocessFormula(formula.asRewardOperatorFormula(), data, currentObjective);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Could not preprocess the objective " << formula << " because it is not supported");
                }
                
                if(currentObjective.threshold) {
                    currentObjective.threshold = (currentObjective.threshold.get() - currentObjective.toOriginalValueTransformationOffset) / currentObjective.toOriginalValueTransformationFactor;
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::ProbabilityOperatorFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective) {
                currentObjective.rewardFinitenessChecked = true;
                bool isProb0Formula = false;
                bool isProb1Formula = false;
                if(currentObjective.threshold && !currentObjective.thresholdIsStrict) {
                    isProb0Formula = !currentObjective.rewardsArePositive && storm::utility::isZero(*currentObjective.threshold);
                    isProb1Formula = currentObjective.rewardsArePositive && storm::utility::isOne(*currentObjective.threshold);
                }
                
                if(formula.getSubformula().isUntilFormula()){
                    preprocessFormula(formula.getSubformula().asUntilFormula(), data, currentObjective, isProb0Formula, isProb1Formula);
                } else if(formula.getSubformula().isBoundedUntilFormula()){
                    preprocessFormula(formula.getSubformula().asBoundedUntilFormula(), data, currentObjective);
                } else if(formula.getSubformula().isGloballyFormula()){
                    preprocessFormula(formula.getSubformula().asGloballyFormula(), data, currentObjective, isProb0Formula, isProb1Formula);
                } else if(formula.getSubformula().isEventuallyFormula()){
                    preprocessFormula(formula.getSubformula().asEventuallyFormula(), data, currentObjective, isProb0Formula, isProb1Formula);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }

            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::RewardOperatorFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective) {
                // Check if the reward model is uniquely specified
                STORM_LOG_THROW((formula.hasRewardModelName() && data.preprocessedModel.hasRewardModel(formula.getRewardModelName()))
                                || data.preprocessedModel.hasUniqueRewardModel(), storm::exceptions::InvalidPropertyException, "The reward model is not unique and the formula " << formula << " does not specify a reward model.");
                
                // reward finiteness has to be checked later iff infinite reward is possible for the subformula
                currentObjective.rewardFinitenessChecked = formula.getSubformula().isCumulativeRewardFormula() || (formula.getSubformula().isEventuallyFormula() && !currentObjective.rewardsArePositive);
                
                if(formula.getSubformula().isEventuallyFormula()){
                    preprocessFormula(formula.getSubformula().asEventuallyFormula(), data, currentObjective, false, false, formula.getOptionalRewardModelName());
                } else if(formula.getSubformula().isCumulativeRewardFormula()) {
                    preprocessFormula(formula.getSubformula().asCumulativeRewardFormula(), data, currentObjective, formula.getOptionalRewardModelName());
                } else if(formula.getSubformula().isTotalRewardFormula()) {
                    preprocessFormula(formula.getSubformula().asTotalRewardFormula(), data, currentObjective, formula.getOptionalRewardModelName());
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::UntilFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, bool isProb0Formula, bool isProb1Formula) {
                CheckTask<storm::logic::Formula> phiTask(formula.getLeftSubformula());
                CheckTask<storm::logic::Formula> psiTask(formula.getRightSubformula());
                storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> mc(data.preprocessedModel);
                STORM_LOG_THROW(mc.canHandle(phiTask) && mc.canHandle(psiTask), storm::exceptions::InvalidPropertyException, "The subformulas of " << formula << " should be propositional.");
                storm::storage::BitVector phiStates = mc.check(phiTask)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                storm::storage::BitVector psiStates = mc.check(psiTask)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                
                auto duplicatorResult = storm::transformer::StateDuplicator<SparseModelType>::transform(data.preprocessedModel, ~phiStates | psiStates);
                updatePreprocessedModel(data, *duplicatorResult.model, duplicatorResult.newToOldStateIndexMapping);
                
                storm::storage::BitVector newPsiStates(data.preprocessedModel.getNumberOfStates(), false);
                for(auto const& oldPsiState : psiStates){
                    //note that psiStates are always located in the second copy
                    newPsiStates.set(duplicatorResult.secondCopyOldToNewStateIndexMapping[oldPsiState], true);
                }
                
                if(isProb0Formula || isProb1Formula) {
                    storm::storage::BitVector subsystemStates;
                    storm::storage::BitVector noIncomingTransitionFromFirstCopyStates;
                    if(isProb0Formula) {
                        subsystemStates = storm::utility::graph::performProb0E(data.preprocessedModel, data.preprocessedModel.getBackwardTransitions(), duplicatorResult.firstCopy, newPsiStates);
                        subsystemStates |= duplicatorResult.secondCopy;
                        noIncomingTransitionFromFirstCopyStates = newPsiStates;
                    } else {
                        for(auto psiState : newPsiStates) {
                            data.preprocessedModel.getStateLabeling().addLabelToState(data.prob1StatesLabel, psiState);
                        }
                        subsystemStates = storm::utility::graph::performProb1E(data.preprocessedModel, data.preprocessedModel.getBackwardTransitions(), duplicatorResult.firstCopy, newPsiStates);
                        subsystemStates |= duplicatorResult.secondCopy;
                        noIncomingTransitionFromFirstCopyStates = duplicatorResult.secondCopy & (~newPsiStates);
                    }
                    storm::storage::BitVector consideredActions(data.preprocessedModel.getTransitionMatrix().getRowCount(), true);
                    for(auto state : duplicatorResult.firstCopy) {
                        for(uint_fast64_t action = data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state]; action < data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state +1] ; ++action) {
                            for(auto const& entry : data.preprocessedModel.getTransitionMatrix().getRow(action)) {
                                if(noIncomingTransitionFromFirstCopyStates.get(entry.getColumn())) {
                                    consideredActions.set(action, false);
                                    break;
                                }
                            }
                        }
                    }
                    subsystemStates = storm::utility::graph::getReachableStates(data.preprocessedModel.getTransitionMatrix(), data.preprocessedModel.getInitialStates(), subsystemStates, storm::storage::BitVector(subsystemStates.size(), false));
                    auto subsystemBuilderResult = storm::transformer::SubsystemBuilder<SparseModelType>::transform(data.preprocessedModel, subsystemStates, consideredActions);
                    updatePreprocessedModel(data, *subsystemBuilderResult.model, subsystemBuilderResult.newToOldStateIndexMapping);
                    data.objectivesSolvedInPreprocessing.set(data.objectives.size());
                } else {
                    // build stateAction reward vector that gives (one*transitionProbability) reward whenever a transition leads from the firstCopy to a psiState
                    std::vector<ValueType> objectiveRewards(data.preprocessedModel.getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                    for (auto const& firstCopyState : duplicatorResult.firstCopy) {
                        for (uint_fast64_t row = data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[firstCopyState]; row < data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[firstCopyState + 1]; ++row) {
                            objectiveRewards[row] = data.preprocessedModel.getTransitionMatrix().getConstrainedRowSum(row, newPsiStates);
                        }
                    }
                    if(!currentObjective.rewardsArePositive) {
                        storm::utility::vector::scaleVectorInPlace(objectiveRewards, -storm::utility::one<ValueType>());
                    }
                    data.preprocessedModel.addRewardModel(currentObjective.rewardModelName, RewardModelType(boost::none, objectiveRewards));
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::BoundedUntilFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective) {
                STORM_LOG_THROW(formula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Expected a boundedUntilFormula with a discrete time bound but got " << formula << ".");
                currentObjective.stepBound = formula.getDiscreteTimeBound();
                STORM_LOG_THROW(*currentObjective.stepBound > 0, storm::exceptions::InvalidPropertyException, "Got a boundedUntilFormula with time bound 0. This is not supported.");
                preprocessFormula(storm::logic::UntilFormula(formula.getLeftSubformula().asSharedPointer(), formula.getRightSubformula().asSharedPointer()), data, currentObjective, false, false);
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::GloballyFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, bool isProb0Formula, bool isProb1Formula) {
                // The formula will be transformed to an until formula for the complementary event.
                // If the original formula minimizes, the complementary one will maximize and vice versa.
                // Hence, the decision whether to consider positive or negative rewards flips.
                currentObjective.rewardsArePositive = !currentObjective.rewardsArePositive;
                // To transform from the value of the preprocessed model back to the value of the original model, we have to add 1 to the result.
                // The transformation factor has already been set correctly.
                currentObjective.toOriginalValueTransformationOffset = storm::utility::one<ValueType>();
                auto negatedSubformula = std::make_shared<storm::logic::UnaryBooleanStateFormula>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, formula.getSubformula().asSharedPointer());
                preprocessFormula(storm::logic::UntilFormula(storm::logic::Formula::getTrueFormula(), negatedSubformula), data, currentObjective, isProb1Formula, isProb0Formula);
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::EventuallyFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective,  bool isProb0Formula, bool isProb1Formula, boost::optional<std::string> const& optionalRewardModelName) {
                if(formula.isReachabilityProbabilityFormula()){
                    preprocessFormula(storm::logic::UntilFormula(storm::logic::Formula::getTrueFormula(), formula.getSubformula().asSharedPointer()), data, currentObjective, isProb0Formula, isProb1Formula);
                    return;
                }
                STORM_LOG_THROW(formula.isReachabilityRewardFormula(), storm::exceptions::InvalidPropertyException, "The formula " << formula << " neither considers reachability Probabilities nor reachability rewards");
                
                CheckTask<storm::logic::Formula> targetTask(formula.getSubformula());
                storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> mc(data.preprocessedModel);
                STORM_LOG_THROW(mc.canHandle(targetTask), storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " should be propositional.");
                storm::storage::BitVector targetStates = mc.check(targetTask)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                auto duplicatorResult = storm::transformer::StateDuplicator<SparseModelType>::transform(data.preprocessedModel, targetStates);
                updatePreprocessedModel(data, *duplicatorResult.model, duplicatorResult.newToOldStateIndexMapping);
                
                // Add a reward model that gives zero reward to the actions of states of the second copy.
                std::vector<ValueType> objectiveRewards = data.preprocessedModel.getRewardModel(optionalRewardModelName ? optionalRewardModelName.get() : "").getTotalRewardVector(data.preprocessedModel.getTransitionMatrix());
                for(auto secondCopyState : duplicatorResult.secondCopy) {
                    for(uint_fast64_t choice = data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[secondCopyState]; choice < data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[secondCopyState+1]; ++choice) {
                        objectiveRewards[choice] = storm::utility::zero<ValueType>();
                    }
                }
                storm::storage::BitVector positiveRewards = storm::utility::vector::filterGreaterZero(objectiveRewards);
                storm::storage::BitVector nonNegativeRewards = positiveRewards | storm::utility::vector::filterZero(objectiveRewards);
                STORM_LOG_THROW(nonNegativeRewards.full() || positiveRewards.empty(), storm::exceptions::InvalidPropertyException, "The reward model for the formula " << formula << " has positive and negative rewards which is not supported.");
                if(!currentObjective.rewardsArePositive){
                    storm::utility::vector::scaleVectorInPlace(objectiveRewards, -storm::utility::one<ValueType>());
                }
                data.preprocessedModel.addRewardModel(currentObjective.rewardModelName, RewardModelType(boost::none, objectiveRewards));
                
                // States of the first copy from which the second copy is not reachable with prob 1 under any scheduler can
                // be removed as the expected reward is not defined for these states.
                // We also need to enforce that the second copy will be reached eventually with prob 1.
                for(auto targetState : duplicatorResult.gateStates) {
                    data.preprocessedModel.getStateLabeling().addLabelToState(data.prob1StatesLabel, targetState);
                }
                storm::storage::BitVector subsystemStates = storm::utility::graph::performProb1E(data.preprocessedModel, data.preprocessedModel.getBackwardTransitions(), duplicatorResult.firstCopy, duplicatorResult.gateStates);
                subsystemStates |= duplicatorResult.secondCopy;
                if(!subsystemStates.full()) {
                    auto subsystemBuilderResult = storm::transformer::SubsystemBuilder<SparseModelType>::transform(data.preprocessedModel, subsystemStates, storm::storage::BitVector(data.preprocessedModel.getTransitionMatrix().getRowCount(), true));
                    updatePreprocessedModel(data, *subsystemBuilderResult.model, subsystemBuilderResult.newToOldStateIndexMapping);
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::CumulativeRewardFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName) {
                STORM_LOG_THROW(formula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Expected a cumulativeRewardFormula with a discrete time bound but got " << formula << ".");
                currentObjective.stepBound = formula.getDiscreteTimeBound();
                STORM_LOG_THROW(*currentObjective.stepBound > 0, storm::exceptions::InvalidPropertyException, "Got a cumulativeRewardFormula with time bound 0. This is not supported.");
                
                std::vector<ValueType> objectiveRewards = data.preprocessedModel.getRewardModel(optionalRewardModelName ? optionalRewardModelName.get() : "").getTotalRewardVector(data.preprocessedModel.getTransitionMatrix());
                if(!currentObjective.rewardsArePositive){
                    storm::utility::vector::scaleVectorInPlace(objectiveRewards, -storm::utility::one<ValueType>());
                }
                data.preprocessedModel.addRewardModel(currentObjective.rewardModelName, RewardModelType(boost::none, objectiveRewards));
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::TotalRewardFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName) {
                std::vector<ValueType> objectiveRewards = data.preprocessedModel.getRewardModel(optionalRewardModelName ? optionalRewardModelName.get() : "").getTotalRewardVector(data.preprocessedModel.getTransitionMatrix());
                storm::storage::BitVector positiveRewards = storm::utility::vector::filterGreaterZero(objectiveRewards);
                storm::storage::BitVector nonNegativeRewards = positiveRewards | storm::utility::vector::filterZero(objectiveRewards);
                STORM_LOG_THROW(nonNegativeRewards.full() || positiveRewards.empty(), storm::exceptions::InvalidPropertyException, "The reward model for the formula " << formula << " has positive and negative rewards which is not supported.");
                if(!currentObjective.rewardsArePositive){
                    storm::utility::vector::scaleVectorInPlace(objectiveRewards, -storm::utility::one<ValueType>());
                }
                data.preprocessedModel.addRewardModel(currentObjective.rewardModelName, RewardModelType(boost::none, objectiveRewards));
            }
            
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::assertRewardFiniteness(PreprocessorData& data) {
                bool negativeRewardsOccur = false;
                bool positiveRewardsOccur = false;
                for(auto& obj : data.objectives) {
                    if (!obj.rewardFinitenessChecked) {
                        negativeRewardsOccur |= !obj.rewardsArePositive;
                        positiveRewardsOccur |= obj.rewardsArePositive;
                    }
                }
                storm::storage::BitVector actionsWithNegativeReward;
                if(negativeRewardsOccur) {
                    actionsWithNegativeReward = assertNegativeRewardFiniteness(data);
                } else {
                    actionsWithNegativeReward = storm::storage::BitVector(data.preprocessedModel.getTransitionMatrix().getRowCount(), false);
                }
                if(positiveRewardsOccur) {
                    assertPositiveRewardFiniteness(data, actionsWithNegativeReward);
                }
            }
            
            template<typename SparseModelType>
            storm::storage::BitVector SparseMultiObjectivePreprocessor<SparseModelType>::assertNegativeRewardFiniteness(PreprocessorData& data) {

                storm::storage::BitVector actionsWithNonNegativeReward(data.preprocessedModel.getTransitionMatrix().getRowCount(), true);
                for(auto& obj : data.objectives) {
                    if (!obj.rewardFinitenessChecked && !obj.rewardsArePositive) {
                        obj.rewardFinitenessChecked = true;
                        actionsWithNonNegativeReward &= storm::utility::vector::filterZero(data.preprocessedModel.getRewardModel(obj.rewardModelName).getStateActionRewardVector());
                    }
                }
                
                storm::storage::BitVector statesWithNegativeRewardForAllChoices(data.preprocessedModel.getNumberOfStates(), true);
                for(uint_fast64_t state = 0; state < data.preprocessedModel.getNumberOfStates(); ++state) {
                    // state has negative reward for all choices iff there is no bit set in actionsWithNonNegativeReward for all actions of state
                    statesWithNegativeRewardForAllChoices.set(state, actionsWithNonNegativeReward.getNextSetIndex(data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state]) >= data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state+1]);
                }
                
                storm::storage::BitVector allStates(data.preprocessedModel.getNumberOfStates(), true);
                storm::storage::SparseMatrix<ValueType> transitionsWithNonNegativeReward = data.preprocessedModel.getTransitionMatrix().restrictRows(actionsWithNonNegativeReward);
                storm::storage::BitVector statesNeverReachingNegativeRewardForSomeScheduler = storm::utility::graph::performProb0E(transitionsWithNonNegativeReward, transitionsWithNonNegativeReward.getRowGroupIndices(), transitionsWithNonNegativeReward.transpose(true), allStates, statesWithNegativeRewardForAllChoices);
                storm::storage::BitVector statesReachingNegativeRewardsFinitelyOftenForSomeScheduler = storm::utility::graph::performProb1E(data.preprocessedModel.getTransitionMatrix(), data.preprocessedModel.getTransitionMatrix().getRowGroupIndices(), data.preprocessedModel.getBackwardTransitions(), allStates, statesNeverReachingNegativeRewardForSomeScheduler);
                
                auto subsystemBuilderResult = storm::transformer::SubsystemBuilder<SparseModelType>::transform(data.preprocessedModel, statesReachingNegativeRewardsFinitelyOftenForSomeScheduler, storm::storage::BitVector(data.preprocessedModel.getTransitionMatrix().getRowCount(), true));
                data.preprocessedModel = std::move(*subsystemBuilderResult.model);
                // subsystemBuilderResult.newToOldStateIndexMapping now reffers to the indices of the model we had before building the subsystem
                for(auto & originalModelStateIndex : subsystemBuilderResult.newToOldStateIndexMapping){
                    originalModelStateIndex = data.newToOldStateIndexMapping[originalModelStateIndex];
                }
                data.newToOldStateIndexMapping = std::move(subsystemBuilderResult.newToOldStateIndexMapping);
                return (~actionsWithNonNegativeReward) % subsystemBuilderResult.subsystemActions;
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::assertPositiveRewardFiniteness(PreprocessorData& data, storm::storage::BitVector const& actionsWithNegativeReward) {
                
                auto mecDecomposition = storm::storage::MaximalEndComponentDecomposition<ValueType>(data.preprocessedModel.getTransitionMatrix(), data.preprocessedModel.getBackwardTransitions());
                if(mecDecomposition.empty()) {
                    return;
                }
                for(uint_fast64_t objIndex = 0; objIndex < data.objectives.size(); ++objIndex) {
                    auto& obj = data.objectives[objIndex];
                    if (!obj.rewardFinitenessChecked && obj.rewardsArePositive) {
                        obj.rewardFinitenessChecked = true;
                        // Find maximal end components that contain a state with positive reward
                        storm::storage::BitVector actionsWithPositiveRewards = storm::utility::vector::filterGreaterZero(data.preprocessedModel.getRewardModel(obj.rewardModelName).getStateActionRewardVector());
                        for(auto const& mec : mecDecomposition) {
                            bool ecHasActionWithPositiveReward = false;
                            for(auto const& stateActionsPair : mec) {
                                for(auto const& action : stateActionsPair.second) {
                                    STORM_LOG_THROW(!actionsWithNegativeReward.get(action), storm::exceptions::InvalidPropertyException, "Found an end componet that contains rewards for a maximizing and a minimizing objective. This is not supported");
                                    // Note: we could also check whether some sub EC exists that does not contain negative rewards.
                                    ecHasActionWithPositiveReward |= (actionsWithPositiveRewards.get(action));
                                }
                            }
                            if(ecHasActionWithPositiveReward) {
                                STORM_LOG_DEBUG("Found end component that contains positive rewards for current objective " << *obj.originalFormula << ".");
                                data.objectivesSolvedInPreprocessing.set(objIndex);
                                break;
                            }
                        }
                    }
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::updatePreprocessedModel(PreprocessorData& data, SparseModelType& newPreprocessedModel, std::vector<uint_fast64_t>& newToOldStateIndexMapping) {
                data.preprocessedModel = std::move(newPreprocessedModel);
                // the given newToOldStateIndexMapping reffers to the indices of the former preprocessedModel as 'old indices'
                for(auto & preprocessedModelStateIndex : newToOldStateIndexMapping){
                    preprocessedModelStateIndex = data.newToOldStateIndexMapping[preprocessedModelStateIndex];
                }
                data.newToOldStateIndexMapping = std::move(newToOldStateIndexMapping);
            }
            
            
            template class SparseMultiObjectivePreprocessor<storm::models::sparse::Mdp<double>>;

            
        }
    }
}
