 #include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePreprocessor.h"

#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/storage/MaximalEndComponentDecomposition.h"
#include "src/transformer/StateDuplicator.h"
#include "src/transformer/SubsystemBuilder.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"
#include "src/utility/Stopwatch.h"

#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template<typename SparseModelType>
            typename SparseMultiObjectivePreprocessor<SparseModelType>::PreprocessorData SparseMultiObjectivePreprocessor<SparseModelType>::preprocess(storm::logic::MultiObjectiveFormula const& originalFormula, SparseModelType const& originalModel) {
                
                PreprocessorData data(originalFormula, originalModel, SparseModelType(originalModel), storm::utility::vector::buildVectorForRange(0, originalModel.getNumberOfStates()));
                
                //Invoke preprocessing on the individual objectives
                for(auto const& subFormula : originalFormula.getSubformulas()){
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
                
                ensureRewardFiniteness(data);
                handleObjectivesWithSolutionFromPreprocessing(data);
                
                // Set the query type. In case of a numerical query, also set the index of the objective to be optimized.
                // Note: If there are only zero (or one) objectives left, we should not consider a pareto query!
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
            void SparseMultiObjectivePreprocessor<SparseModelType>::updatePreprocessedModel(PreprocessorData& data, SparseModelType& newPreprocessedModel, std::vector<uint_fast64_t>& newToOldStateIndexMapping) {
                data.preprocessedModel = std::move(newPreprocessedModel);
                // the given newToOldStateIndexMapping reffers to the indices of the former preprocessedModel as 'old indices'
                for(auto & preprocessedModelStateIndex : newToOldStateIndexMapping){
                    preprocessedModelStateIndex = data.newToOldStateIndexMapping[preprocessedModelStateIndex];
                }
                data.newToOldStateIndexMapping = std::move(newToOldStateIndexMapping);
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
                } else if(formula.isTimeOperatorFormula()){
                    preprocessFormula(formula.asTimeOperatorFormula(), data, currentObjective);
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
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::TimeOperatorFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective) {
                // Time formulas are only supported for Markov automata
                STORM_LOG_THROW(data.originalModel.isOfType(storm::models::ModelType::MarkovAutomaton), storm::exceptions::InvalidPropertyException, "Time operator formulas are only supported for Markov automata.");
                
                // reward finiteness does not need to be checked if we want to minimize time
                currentObjective.rewardFinitenessChecked = !currentObjective.rewardsArePositive;
                
                if(formula.getSubformula().isEventuallyFormula()){
                    preprocessFormula(formula.getSubformula().asEventuallyFormula(), data, currentObjective, false, false);
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
                
                if(!(psiStates & data.preprocessedModel.getInitialStates()).empty() && !currentObjective.lowerTimeBound) {
                    // The probability is always one
                    data.solutionsFromPreprocessing[data.objectives.size()-1].first = PreprocessorData::PreprocessorObjectiveSolution::Numerical;
                    data.solutionsFromPreprocessing[data.objectives.size()-1].second = currentObjective.rewardsArePositive ? storm::utility::one<ValueType>() : -storm::utility::one<ValueType>();
                    return;
                }
                
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
                        storm::storage::BitVector statesReachableInSecondCopy = storm::utility::graph::getReachableStates(data.preprocessedModel.getTransitionMatrix(), duplicatorResult.gateStates & (~newPsiStates), duplicatorResult.secondCopy, storm::storage::BitVector(data.preprocessedModel.getNumberOfStates(), false));
                        subsystemStates = statesReachableInSecondCopy | (duplicatorResult.firstCopy & storm::utility::graph::performProb0E(data.preprocessedModel, data.preprocessedModel.getBackwardTransitions(), duplicatorResult.firstCopy, newPsiStates));
                        noIncomingTransitionFromFirstCopyStates = newPsiStates;
                    } else {
                        storm::storage::BitVector statesReachableInSecondCopy = storm::utility::graph::getReachableStates(data.preprocessedModel.getTransitionMatrix(), newPsiStates, duplicatorResult.secondCopy, storm::storage::BitVector(data.preprocessedModel.getNumberOfStates(), false));
                        data.preprocessedModel.getStateLabeling().setStates(data.prob1StatesLabel, data.preprocessedModel.getStateLabeling().getStates(data.prob1StatesLabel) & statesReachableInSecondCopy);
                        subsystemStates = statesReachableInSecondCopy | storm::utility::graph::performProb1E(data.preprocessedModel, data.preprocessedModel.getBackwardTransitions(), duplicatorResult.firstCopy, newPsiStates);
                        noIncomingTransitionFromFirstCopyStates = duplicatorResult.gateStates & (~newPsiStates);
                    }
                    if((subsystemStates & data.preprocessedModel.getInitialStates()).empty()) {
                        data.solutionsFromPreprocessing[data.objectives.size()-1].first = PreprocessorData::PreprocessorObjectiveSolution::False;
                    } else {
                        data.solutionsFromPreprocessing[data.objectives.size()-1].first = PreprocessorData::PreprocessorObjectiveSolution::True;
                        // Get a subsystem that deletes actions for which the property would be violated
                        storm::storage::BitVector consideredActions(data.preprocessedModel.getTransitionMatrix().getRowCount(), true);
                        for(auto state : duplicatorResult.firstCopy) {
                            for(uint_fast64_t action = data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state]; action < data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state +1]; ++action) {
                                for(auto const& entry : data.preprocessedModel.getTransitionMatrix().getRow(action)) {
                                    if(noIncomingTransitionFromFirstCopyStates.get(entry.getColumn())) {
                                        consideredActions.set(action, false);
                                        break;
                                    }
                                }
                            }
                        }
                        if(!subsystemStates.full() || !consideredActions.full()) {
                            auto subsystemBuilderResult = storm::transformer::SubsystemBuilder<SparseModelType>::transform(data.preprocessedModel, subsystemStates, consideredActions);
                            updatePreprocessedModel(data, *subsystemBuilderResult.model, subsystemBuilderResult.newToOldStateIndexMapping);
                        }
                    }
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
                
                if(formula.hasDiscreteTimeBound()) {
                    currentObjective.upperTimeBound = storm::utility::convertNumber<ValueType>(formula.getDiscreteTimeBound());
                } else {
                    if(data.originalModel.isOfType(storm::models::ModelType::Mdp)) {
                        STORM_LOG_THROW(formula.getIntervalBounds().first == std::round(formula.getIntervalBounds().first), storm::exceptions::InvalidPropertyException, "Expected a boundedUntilFormula with discrete lower time bound but got " << formula << ".");
                        STORM_LOG_THROW(formula.getIntervalBounds().second == std::round(formula.getIntervalBounds().second), storm::exceptions::InvalidPropertyException, "Expected a boundedUntilFormula with discrete upper time bound but got " << formula << ".");
                    } else {
                        STORM_LOG_THROW(data.originalModel.isOfType(storm::models::ModelType::MarkovAutomaton), storm::exceptions::InvalidPropertyException, "Got a boundedUntilFormula which can not be checked for the current model type.");
                        STORM_LOG_THROW(formula.getIntervalBounds().second > formula.getIntervalBounds().first, storm::exceptions::InvalidPropertyException, "Neither empty nor point intervalls are allowed but got " << formula << ".");
                    }
                    if(!storm::utility::isZero(formula.getIntervalBounds().first)) {
                        currentObjective.lowerTimeBound = storm::utility::convertNumber<ValueType>(formula.getIntervalBounds().first);
                    }
                    currentObjective.upperTimeBound = storm::utility::convertNumber<ValueType>(formula.getIntervalBounds().second);
                }
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
                
                // We need to swap the two flags isProb0Formula and isProb1Formula
                preprocessFormula(storm::logic::UntilFormula(storm::logic::Formula::getTrueFormula(), negatedSubformula), data, currentObjective, isProb1Formula, isProb0Formula);
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::EventuallyFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective,  bool isProb0Formula, bool isProb1Formula, boost::optional<std::string> const& optionalRewardModelName) {
                if(formula.isReachabilityProbabilityFormula()){
                    preprocessFormula(storm::logic::UntilFormula(storm::logic::Formula::getTrueFormula(), formula.getSubformula().asSharedPointer()), data, currentObjective, isProb0Formula, isProb1Formula);
                    return;
                }
                
                CheckTask<storm::logic::Formula> targetTask(formula.getSubformula());
                storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> mc(data.preprocessedModel);
                STORM_LOG_THROW(mc.canHandle(targetTask), storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " should be propositional.");
                storm::storage::BitVector targetStates = mc.check(targetTask)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                
                if(!(targetStates & data.preprocessedModel.getInitialStates()).empty()) {
                    // The value is always zero
                    data.solutionsFromPreprocessing[data.objectives.size()-1].first = PreprocessorData::PreprocessorObjectiveSolution::Numerical;
                    data.solutionsFromPreprocessing[data.objectives.size()-1].second = storm::utility::zero<ValueType>();
                    return;
                }
                
                auto duplicatorResult = storm::transformer::StateDuplicator<SparseModelType>::transform(data.preprocessedModel, targetStates);
                updatePreprocessedModel(data, *duplicatorResult.model, duplicatorResult.newToOldStateIndexMapping);
                
                // Add a reward model that gives zero reward to the actions of states of the second copy.
                RewardModelType objectiveRewards(boost::none);
                if(formula.isReachabilityRewardFormula()) {
                    objectiveRewards = data.preprocessedModel.getRewardModel(optionalRewardModelName ? optionalRewardModelName.get() : "");
                    objectiveRewards.reduceToStateBasedRewards(data.preprocessedModel.getTransitionMatrix(), false);
                    if(objectiveRewards.hasStateRewards()) {
                        storm::utility::vector::setVectorValues(objectiveRewards.getStateRewardVector(), duplicatorResult.secondCopy, storm::utility::zero<ValueType>());
                    }
                    if(objectiveRewards.hasStateActionRewards()) {
                        for(auto secondCopyState : duplicatorResult.secondCopy) {
                            std::fill_n(objectiveRewards.getStateActionRewardVector().begin() + data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[secondCopyState], data.preprocessedModel.getTransitionMatrix().getRowGroupSize(secondCopyState), storm::utility::zero<ValueType>());
                        }
                    }
                } else if(formula.isReachabilityTimeFormula() && data.preprocessedModel.isOfType(storm::models::ModelType::MarkovAutomaton)) {
                    objectiveRewards = RewardModelType(std::vector<ValueType>(data.preprocessedModel.getNumberOfStates(), storm::utility::zero<ValueType>()));
                    storm::utility::vector::setVectorValues(objectiveRewards.getStateRewardVector(), data.getMarkovianStatesOfPreprocessedModel() & duplicatorResult.firstCopy, storm::utility::one<ValueType>());
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The formula " << formula << " neither considers reachability probabilities nor reachability rewards " << (data.preprocessedModel.isOfType(storm::models::ModelType::MarkovAutomaton) ?  "nor reachability time" : "") << ". This is not supported.");
                }
                if(!currentObjective.rewardsArePositive){
                    if(objectiveRewards.hasStateRewards()) {
                        storm::utility::vector::scaleVectorInPlace(objectiveRewards.getStateRewardVector(), -storm::utility::one<ValueType>());
                    }
                    if(objectiveRewards.hasStateActionRewards()) {
                        storm::utility::vector::scaleVectorInPlace(objectiveRewards.getStateActionRewardVector(), -storm::utility::one<ValueType>());
                    }
                }
                data.preprocessedModel.addRewardModel(currentObjective.rewardModelName, std::move(objectiveRewards));
                
                // States of the first copy from which the second copy is not reachable with prob 1 under any scheduler can
                // be removed as the expected reward/time is not defined for these states.
                // We also need to enforce that the second copy will be reached eventually with prob 1.
                data.preprocessedModel.getStateLabeling().setStates(data.prob1StatesLabel, data.preprocessedModel.getStateLabeling().getStates(data.prob1StatesLabel) & duplicatorResult.secondCopy);
                storm::storage::BitVector subsystemStates =  duplicatorResult.secondCopy | storm::utility::graph::performProb1E(data.preprocessedModel, data.preprocessedModel.getBackwardTransitions(), duplicatorResult.firstCopy, duplicatorResult.gateStates);
                if((subsystemStates & data.preprocessedModel.getInitialStates()).empty()) {
                    data.solutionsFromPreprocessing[data.objectives.size()-1].first = PreprocessorData::PreprocessorObjectiveSolution::Undefined;
                } else if(!subsystemStates.full()) {
                    auto subsystemBuilderResult = storm::transformer::SubsystemBuilder<SparseModelType>::transform(data.preprocessedModel, subsystemStates, storm::storage::BitVector(data.preprocessedModel.getTransitionMatrix().getRowCount(), true));
                    updatePreprocessedModel(data, *subsystemBuilderResult.model, subsystemBuilderResult.newToOldStateIndexMapping);
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::CumulativeRewardFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName) {
                STORM_LOG_THROW(data.originalModel.isOfType(storm::models::ModelType::Mdp), storm::exceptions::InvalidPropertyException, "Cumulative reward formulas are not supported for the given model type.");
                STORM_LOG_THROW(formula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Expected a cumulativeRewardFormula with a discrete time bound but got " << formula << ".");
                if(formula.getDiscreteTimeBound()==0) {
                    data.solutionsFromPreprocessing[data.objectives.size()-1].first = PreprocessorData::PreprocessorObjectiveSolution::Numerical;
                    data.solutionsFromPreprocessing[data.objectives.size()-1].second = storm::utility::zero<ValueType>();
                    return;
                }
                currentObjective.upperTimeBound = storm::utility::convertNumber<ValueType>(formula.getDiscreteTimeBound());
                
                RewardModelType objectiveRewards = data.preprocessedModel.getRewardModel(optionalRewardModelName ? optionalRewardModelName.get() : "");
                objectiveRewards.reduceToStateBasedRewards(data.preprocessedModel.getTransitionMatrix(), false);
                if(!currentObjective.rewardsArePositive){
                    if(objectiveRewards.hasStateRewards()) {
                        storm::utility::vector::scaleVectorInPlace(objectiveRewards.getStateRewardVector(), -storm::utility::one<ValueType>());
                    }
                    if(objectiveRewards.hasStateActionRewards()) {
                        storm::utility::vector::scaleVectorInPlace(objectiveRewards.getStateActionRewardVector(), -storm::utility::one<ValueType>());
                    }
                }
                data.preprocessedModel.addRewardModel(currentObjective.rewardModelName, std::move(objectiveRewards));
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::TotalRewardFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName) {
                RewardModelType objectiveRewards = data.preprocessedModel.getRewardModel(optionalRewardModelName ? optionalRewardModelName.get() : "");
                objectiveRewards.reduceToStateBasedRewards(data.preprocessedModel.getTransitionMatrix(), false);
                if(!currentObjective.rewardsArePositive){
                    if(objectiveRewards.hasStateRewards()) {
                        storm::utility::vector::scaleVectorInPlace(objectiveRewards.getStateRewardVector(), -storm::utility::one<ValueType>());
                    }
                    if(objectiveRewards.hasStateActionRewards()) {
                        storm::utility::vector::scaleVectorInPlace(objectiveRewards.getStateActionRewardVector(), -storm::utility::one<ValueType>());
                    }
                }
                data.preprocessedModel.addRewardModel(currentObjective.rewardModelName, std::move(objectiveRewards));
            }
            
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::ensureRewardFiniteness(PreprocessorData& data) {
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
                    actionsWithNegativeReward = ensureNegativeRewardFiniteness(data);
                } else {
                    actionsWithNegativeReward = storm::storage::BitVector(data.preprocessedModel.getTransitionMatrix().getRowCount(), false);
                }
                if(positiveRewardsOccur) {
                    ensurePositiveRewardFiniteness(data, actionsWithNegativeReward);
                }
            }
            
            template<typename SparseModelType>
            storm::storage::BitVector SparseMultiObjectivePreprocessor<SparseModelType>::ensureNegativeRewardFiniteness(PreprocessorData& data) {

                storm::storage::BitVector actionsWithNonNegativeReward(data.preprocessedModel.getTransitionMatrix().getRowCount(), true);
                storm::storage::BitVector objectivesWithNegativeReward(data.objectives.size(), false);
                for(uint_fast64_t objIndex = 0; objIndex < data.objectives.size(); ++objIndex) {
                    if (!data.objectives[objIndex].rewardFinitenessChecked && !data.objectives[objIndex].rewardsArePositive) {
                        data.objectives[objIndex].rewardFinitenessChecked = true;
                        actionsWithNonNegativeReward &= storm::utility::vector::filterZero(data.preprocessedModel.getRewardModel(data.objectives[objIndex].rewardModelName).getTotalRewardVector(data.preprocessedModel.getTransitionMatrix()));
                        objectivesWithNegativeReward.set(objIndex, true);
                    }
                }
                
                storm::storage::BitVector statesWithNegativeRewardForAllChoices(data.preprocessedModel.getNumberOfStates(), false);
                storm::storage::BitVector submatrixRows = actionsWithNonNegativeReward;
                for(uint_fast64_t state = 0; state < data.preprocessedModel.getNumberOfStates(); ++state) {
                    // state has negative reward for all choices iff there is no bit set in actionsWithNonNegativeReward for all actions of state
                    if(actionsWithNonNegativeReward.getNextSetIndex(data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state]) >= data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state+1]) {
                        statesWithNegativeRewardForAllChoices.set(state, true);
                        // enable one row for the current state to avoid deleting the row group
                        submatrixRows.set(data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state], true);
                    }
                }
                storm::storage::SparseMatrix<ValueType> transitionsWithNonNegativeReward = data.preprocessedModel.getTransitionMatrix().restrictRows(submatrixRows);
                
                storm::storage::BitVector allStates(data.preprocessedModel.getNumberOfStates(), true);
                storm::storage::BitVector statesNeverReachingNegativeRewardForSomeScheduler = storm::utility::graph::performProb0E(transitionsWithNonNegativeReward, transitionsWithNonNegativeReward.getRowGroupIndices(), transitionsWithNonNegativeReward.transpose(true), allStates, statesWithNegativeRewardForAllChoices);
                storm::storage::BitVector statesReachingNegativeRewardsFinitelyOftenForSomeScheduler = storm::utility::graph::performProb1E(data.preprocessedModel.getTransitionMatrix(), data.preprocessedModel.getTransitionMatrix().getRowGroupIndices(), data.preprocessedModel.getBackwardTransitions(), allStates, statesNeverReachingNegativeRewardForSomeScheduler);
                if((statesReachingNegativeRewardsFinitelyOftenForSomeScheduler & data.preprocessedModel.getInitialStates()).empty()) {
                    STORM_LOG_WARN("For every scheduler, the induced reward for one or more of the objectives that minimize rewards is infinity.");
                    for(auto objIndex : objectivesWithNegativeReward) {
                        if(data.objectives[objIndex].threshold) {
                            data.solutionsFromPreprocessing[objIndex].first = PreprocessorData::PreprocessorObjectiveSolution::False;
                        } else {
                            data.solutionsFromPreprocessing[objIndex].first = PreprocessorData::PreprocessorObjectiveSolution::Unbounded;
                        }
                    }
                } else if(!statesReachingNegativeRewardsFinitelyOftenForSomeScheduler.full()) {
                    auto subsystemBuilderResult = storm::transformer::SubsystemBuilder<SparseModelType>::transform(data.preprocessedModel, statesReachingNegativeRewardsFinitelyOftenForSomeScheduler, storm::storage::BitVector(data.preprocessedModel.getTransitionMatrix().getRowCount(), true));
                    updatePreprocessedModel(data, *subsystemBuilderResult.model, subsystemBuilderResult.newToOldStateIndexMapping);
                    return (~actionsWithNonNegativeReward) % subsystemBuilderResult.subsystemActions;
                }
                return ~actionsWithNonNegativeReward;
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::ensurePositiveRewardFiniteness(PreprocessorData& data, storm::storage::BitVector const& actionsWithNegativeReward) {
                storm::utility::Stopwatch sw;
                auto mecDecomposition = storm::storage::MaximalEndComponentDecomposition<ValueType>(data.preprocessedModel.getTransitionMatrix(), data.preprocessedModel.getBackwardTransitions());
                STORM_LOG_DEBUG("Maximal end component decomposition for ensuring positive reward finiteness took " << sw << " seconds.");
                if(mecDecomposition.empty()) {
                    return;
                }
                for(uint_fast64_t objIndex = 0; objIndex < data.objectives.size(); ++objIndex) {
                    auto& obj = data.objectives[objIndex];
                    if (!obj.rewardFinitenessChecked && obj.rewardsArePositive) {
                        obj.rewardFinitenessChecked = true;
                        // Find maximal end components that contain a state with positive reward
                        storm::storage::BitVector actionsWithPositiveRewards = storm::utility::vector::filterGreaterZero(data.preprocessedModel.getRewardModel(obj.rewardModelName).getTotalRewardVector(data.preprocessedModel.getTransitionMatrix()));
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
                                if(obj.threshold) {
                                    data.solutionsFromPreprocessing[objIndex].first = PreprocessorData::PreprocessorObjectiveSolution::True;
                                } else {
                                    data.solutionsFromPreprocessing[objIndex].first = PreprocessorData::PreprocessorObjectiveSolution::Unbounded;
                                }
                                break;
                            }
                        }
                    }
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::handleObjectivesWithSolutionFromPreprocessing(PreprocessorData& data) {
                // Set solution for objectives for which there are no rewards left
                for(uint_fast64_t objIndex = 0; objIndex < data.objectives.size(); ++objIndex) {
                    if(data.solutionsFromPreprocessing[objIndex].first == PreprocessorData::PreprocessorObjectiveSolution::None &&
                       data.preprocessedModel.getRewardModel(data.objectives[objIndex].rewardModelName).isAllZero()) {
                        data.solutionsFromPreprocessing[objIndex].first = PreprocessorData::PreprocessorObjectiveSolution::Numerical;
                        data.solutionsFromPreprocessing[objIndex].second = storm::utility::zero<ValueType>();
                    }
                }
                
                // Translate numerical solutions from preprocessing to Truth values (if the objective specifies a threshold) or to values for the original model (otherwise).
                for(uint_fast64_t objIndex = 0; objIndex < data.objectives.size(); ++objIndex) {
                    if(data.solutionsFromPreprocessing[objIndex].first == PreprocessorData::PreprocessorObjectiveSolution::Numerical) {
                        ValueType& value = data.solutionsFromPreprocessing[objIndex].second;
                        ObjectiveInformation const& obj = data.objectives[objIndex];
                        if(obj.threshold) {
                            if((obj.thresholdIsStrict && value > (*obj.threshold)) || (!obj.thresholdIsStrict && value >= (*obj.threshold))) {
                                data.solutionsFromPreprocessing[objIndex].first = PreprocessorData::PreprocessorObjectiveSolution::True;
                            } else {
                                data.solutionsFromPreprocessing[objIndex].first = PreprocessorData::PreprocessorObjectiveSolution::False;
                            }
                        } else {
                            value = value * obj.toOriginalValueTransformationFactor + obj.toOriginalValueTransformationOffset;
                        }
                    }
                }
                
                // Only keep the objectives for which we have no solution yet
                storm::storage::BitVector objWithNoSolution = storm::utility::vector::filter<std::pair<typename PreprocessorData::PreprocessorObjectiveSolution, ValueType>>(data.solutionsFromPreprocessing, [&] (std::pair<typename PreprocessorData::PreprocessorObjectiveSolution, ValueType> const& value) -> bool { return value.first == PreprocessorData::PreprocessorObjectiveSolution::None; });
                data.objectives = storm::utility::vector::filterVector(data.objectives, objWithNoSolution);
            }
            
            
            template class SparseMultiObjectivePreprocessor<storm::models::sparse::Mdp<double>>;
            template class SparseMultiObjectivePreprocessor<storm::models::sparse::MarkovAutomaton<double>>;
            
#ifdef STORM_HAVE_CARL
            template class SparseMultiObjectivePreprocessor<storm::models::sparse::Mdp<storm::RationalNumber>>;
            template class SparseMultiObjectivePreprocessor<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
#endif

            
        }
    }
}
