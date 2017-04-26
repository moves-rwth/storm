#include "storm/modelchecker/multiobjective/pcaa/SparsePcaaPreprocessor.h"

#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/storage/memorystructure/MemoryStructureBuilder.h"
#include "storm/storage/memorystructure/SparseModelMemoryProduct.h"
#include "storm/transformer/SubsystemBuilder.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"

#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
                
            template<typename SparseModelType>
            typename SparsePcaaPreprocessor<SparseModelType>::ReturnType SparsePcaaPreprocessor<SparseModelType>::preprocess(SparseModelType const& originalModel, storm::logic::MultiObjectiveFormula const& originalFormula) {
                
                ReturnType result(originalFormula, originalModel, SparseModelType(originalModel));
                result.newToOldStateIndexMapping = storm::utility::vector::buildVectorForRange(0, originalModel.getNumberOfStates());
                
                //Invoke preprocessing on the individual objectives
                for (auto const& subFormula : originalFormula.getSubformulas()){
                    STORM_LOG_INFO("Preprocessing objective " << *subFormula<< ".");
                    result.objectives.emplace_back();
                    PcaaObjective<ValueType>& currentObjective = result.objectives.back();
                    currentObjective.originalFormula = subFormula;
                    if (currentObjective.originalFormula->isOperatorFormula()) {
                        preprocessOperatorFormula(currentObjective.originalFormula->asOperatorFormula(), result, currentObjective);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Could not preprocess the subformula " << *subFormula << " of " << originalFormula << " because it is not supported");
                    }
                }
                // Set the query type. In case of a quantitative query, also set the index of the objective to be optimized.
                // Note: If there are only zero (or one) objectives left, we should not consider a pareto query!
                storm::storage::BitVector objectivesWithoutThreshold(result.objectives.size());
                for (uint_fast64_t objIndex = 0; objIndex < result.objectives.size(); ++objIndex) {
                    objectivesWithoutThreshold.set(objIndex, !result.objectives[objIndex].threshold);
                }
                uint_fast64_t numOfObjectivesWithoutThreshold = objectivesWithoutThreshold.getNumberOfSetBits();
                if (numOfObjectivesWithoutThreshold == 0) {
                    result.queryType = ReturnType::QueryType::Achievability;
                } else if (numOfObjectivesWithoutThreshold == 1) {
                    result.queryType = ReturnType::QueryType::Quantitative;
                    result.indexOfOptimizingObjective = objectivesWithoutThreshold.getNextSetIndex(0);
                } else if (numOfObjectivesWithoutThreshold == result.objectives.size()) {
                    result.queryType = ReturnType::QueryType::Pareto;
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "The number of objectives without threshold is not valid. It should be either 0 (achievability query), 1 (quantitative query), or " << result.objectives.size() << " (Pareto Query). Got " << numOfObjectivesWithoutThreshold << " instead.");
                }

                //We can remove the original reward models to save some memory
                std::set<std::string> origRewardModels = originalFormula.getReferencedRewardModels();
                for (auto const& rewModel : origRewardModels){
                    result.preprocessedModel.removeRewardModel(rewModel);
                }

                //Get actions to which a positive or negative reward is assigned for at least one objective
                result.actionsWithPositiveReward = storm::storage::BitVector(result.preprocessedModel.getNumberOfChoices(), false);
                result.actionsWithNegativeReward = storm::storage::BitVector(result.preprocessedModel.getNumberOfChoices(), false);
                for (uint_fast64_t objIndex = 0; objIndex < result.objectives.size(); ++objIndex) {
                    if (!result.objectives[objIndex].upperTimeBound) {
                        if (result.objectives[objIndex].rewardsArePositive) {
                            result.actionsWithPositiveReward |= ~storm::utility::vector::filterZero(result.preprocessedModel.getRewardModel(result.objectives[objIndex].rewardModelName).getTotalRewardVector(result.preprocessedModel.getTransitionMatrix()));
                        } else {
                            result.actionsWithNegativeReward |= ~storm::utility::vector::filterZero(result.preprocessedModel.getRewardModel(result.objectives[objIndex].rewardModelName).getTotalRewardVector(result.preprocessedModel.getTransitionMatrix()));
                        }
                    }
                }

                // Analyze End components and ensure reward finiteness.
                // Note that this is only necessary if there is at least one objective with no upper time bound
                for (auto const& obj : result.objectives) {
                    if (!obj.upperTimeBound) {
                        auto backwardTransitions = result.preprocessedModel.getBackwardTransitions();
                        analyzeEndComponents(result, backwardTransitions);
                        ensureRewardFiniteness(result, backwardTransitions);
                    }
                }
                return result;
            }
            
            template<typename SparseModelType>
            void SparsePcaaPreprocessor<SparseModelType>::updatePreprocessedModel(ReturnType& result, SparseModelType& newPreprocessedModel, std::vector<uint_fast64_t>& newToOldStateIndexMapping) {
                result.preprocessedModel = std::move(newPreprocessedModel);
                // the given newToOldStateIndexMapping refers to the indices of the former preprocessedModel as 'old indices'
                for (auto & preprocessedModelStateIndex : newToOldStateIndexMapping){
                    preprocessedModelStateIndex = result.newToOldStateIndexMapping[preprocessedModelStateIndex];
                }
                result.newToOldStateIndexMapping = std::move(newToOldStateIndexMapping);
            }
            
            template<typename SparseModelType>
            void SparsePcaaPreprocessor<SparseModelType>::addMemoryToPreprocessedModel(ReturnType& result, storm::storage::MemoryStructure& memory) {
                storm::storage::SparseModelMemoryProduct<ValueType> product = memory.product(result.preprocessedModel);
                auto newModel = product.build();
                
                // update the newToOldStateIndexMapping
                std::vector<uint_fast64_t> updatedMapping;
                updatedMapping.reserve(newModel->getNumberOfStates());
                for (uint_fast64_t oldState = 0; oldState < result.preprocessedModel.getNumberOfStates(); ++oldState) {
                    for (uint_fast64_t memoryState = 0; memoryState < memory.getNumberOfStates(); ++memoryState) {
                        uint_fast64_t const& newState = product.getResultState(oldState, memoryState);
                        // Check if the state actually exists in the new model
                        if (newState < newModel->getNumberOfStates()) {
                            assert(newState == updatedMapping.size());
                            updatedMapping.push_back(result.newToOldStateIndexMapping[oldState]);
                        }
                    }
                }
                
                result.preprocessedModel = std::move(dynamic_cast<SparseModelType&>(*newModel));
                result.newToOldStateIndexMapping = std::move(updatedMapping);
            }
            
            template<typename SparseModelType>
            void SparsePcaaPreprocessor<SparseModelType>::preprocessOperatorFormula(storm::logic::OperatorFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective) {
                
                // Get a unique name for the new reward model.
                currentObjective.rewardModelName = "objective" + std::to_string(result.objectives.size());
                while(result.preprocessedModel.hasRewardModel(currentObjective.rewardModelName)){
                    currentObjective.rewardModelName = "_" + currentObjective.rewardModelName;
                }
                
                currentObjective.toOriginalValueTransformationFactor = storm::utility::one<ValueType>();
                currentObjective.toOriginalValueTransformationOffset = storm::utility::zero<ValueType>();
                currentObjective.rewardsArePositive = true;
                
                bool formulaMinimizes = false;
                if (formula.hasBound()) {
                    currentObjective.threshold = formula.template getThresholdAs<ValueType>();
                    currentObjective.thresholdIsStrict = storm::logic::isStrict(formula.getBound().comparisonType);
                    //Note that we minimize for upper bounds since we are looking for the EXISTENCE of a satisfying scheduler
                    formulaMinimizes = !storm::logic::isLowerBound(formula.getBound().comparisonType);
                } else if (formula.hasOptimalityType()){
                    formulaMinimizes = storm::solver::minimize(formula.getOptimalityType());
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Current objective " << formula << " does not specify whether to minimize or maximize");
                }
                if (formulaMinimizes) {
                    // We negate all the values so we can consider the maximum for this objective
                    // Thus, all objectives will be maximized.
                    currentObjective.rewardsArePositive = false;
                    currentObjective.toOriginalValueTransformationFactor = -storm::utility::one<ValueType>();
                }
                
                if (formula.isProbabilityOperatorFormula()){
                    preprocessProbabilityOperatorFormula(formula.asProbabilityOperatorFormula(), result, currentObjective);
                } else if (formula.isRewardOperatorFormula()){
                    preprocessRewardOperatorFormula(formula.asRewardOperatorFormula(), result, currentObjective);
                } else if (formula.isTimeOperatorFormula()){
                    preprocessTimeOperatorFormula(formula.asTimeOperatorFormula(), result, currentObjective);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Could not preprocess the objective " << formula << " because it is not supported");
                }
                
                // Transform the threshold for the preprocessed Model
                if (currentObjective.threshold) {
                    currentObjective.threshold = (currentObjective.threshold.get() - currentObjective.toOriginalValueTransformationOffset) / currentObjective.toOriginalValueTransformationFactor;
                }
            }
            
            template<typename SparseModelType>
            void SparsePcaaPreprocessor<SparseModelType>::preprocessProbabilityOperatorFormula(storm::logic::ProbabilityOperatorFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective) {
                
                if (formula.getSubformula().isUntilFormula()){
                    preprocessUntilFormula(formula.getSubformula().asUntilFormula(), result, currentObjective);
                } else if (formula.getSubformula().isBoundedUntilFormula()){
                    preprocessBoundedUntilFormula(formula.getSubformula().asBoundedUntilFormula(), result, currentObjective);
                } else if (formula.getSubformula().isGloballyFormula()){
                    preprocessGloballyFormula(formula.getSubformula().asGloballyFormula(), result, currentObjective);
                } else if (formula.getSubformula().isEventuallyFormula()){
                    preprocessEventuallyFormula(formula.getSubformula().asEventuallyFormula(), result, currentObjective);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }

            template<typename SparseModelType>
            void SparsePcaaPreprocessor<SparseModelType>::preprocessRewardOperatorFormula(storm::logic::RewardOperatorFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective) {
                // Check if the reward model is uniquely specified
                STORM_LOG_THROW((formula.hasRewardModelName() && result.preprocessedModel.hasRewardModel(formula.getRewardModelName()))
                                || result.preprocessedModel.hasUniqueRewardModel(), storm::exceptions::InvalidPropertyException, "The reward model is not unique and the formula " << formula << " does not specify a reward model.");
                
                if (formula.getSubformula().isEventuallyFormula()){
                    preprocessEventuallyFormula(formula.getSubformula().asEventuallyFormula(), result, currentObjective, formula.getOptionalRewardModelName());
                } else if (formula.getSubformula().isCumulativeRewardFormula()) {
                    preprocessCumulativeRewardFormula(formula.getSubformula().asCumulativeRewardFormula(), result, currentObjective, formula.getOptionalRewardModelName());
                } else if (formula.getSubformula().isTotalRewardFormula()) {
                    preprocessTotalRewardFormula(result, currentObjective, formula.getOptionalRewardModelName());
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }

            template<typename SparseModelType>
            void SparsePcaaPreprocessor<SparseModelType>::preprocessTimeOperatorFormula(storm::logic::TimeOperatorFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective) {
                // Time formulas are only supported for Markov automata
                STORM_LOG_THROW(result.originalModel.isOfType(storm::models::ModelType::MarkovAutomaton), storm::exceptions::InvalidPropertyException, "Time operator formulas are only supported for Markov automata.");
                
                if (formula.getSubformula().isEventuallyFormula()){
                    preprocessEventuallyFormula(formula.getSubformula().asEventuallyFormula(), result, currentObjective);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }
            
            template<typename SparseModelType>
            void SparsePcaaPreprocessor<SparseModelType>::preprocessUntilFormula(storm::logic::UntilFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective) {
                
                // Create a memory structure that stores whether a PhiState or a PsiState has already been reached
                storm::storage::MemoryStructureBuilder builder(2);
                // Get a unique label that is not already present in the model
                std::string memoryLabel = "obj" + std::to_string(result.objectives.size());
                while (result.preprocessedModel.hasLabel(memoryLabel)) memoryLabel = "_" + memoryLabel;
                builder.setLabel(0, memoryLabel);
                auto negatedLeftSubFormula = std::make_shared<storm::logic::UnaryBooleanStateFormula>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, formula.getLeftSubformula().asSharedPointer());
                auto targetFormula = std::make_shared<storm::logic::BinaryBooleanStateFormula>(storm::logic::BinaryBooleanStateFormula::OperatorType::Or, negatedLeftSubFormula, formula.getRightSubformula().asSharedPointer());
                builder.setTransition(0, 0, std::make_shared<storm::logic::UnaryBooleanStateFormula>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, targetFormula));
                builder.setTransition(0, 1, targetFormula);
                builder.setTransition(1, 1, std::make_shared<storm::logic::BooleanLiteralFormula>(true));
                storm::storage::MemoryStructure memory = builder.build();
                addMemoryToPreprocessedModel(result, memory);
                
                // build stateAction reward vector that gives (one*transitionProbability) reward whenever a transition leads from a memoryLabel-state to a psiState
                storm::storage::BitVector const& statesWithMemoryLabel = result.preprocessedModel.getStates(memoryLabel);
                if ((statesWithMemoryLabel & result.preprocessedModel.getInitialStates()).empty() && !currentObjective.lowerTimeBound) {
                    // The probability is always one as the initial state is a target state.
                    // For this special case, the transformation to an expected reward objective fails.
                    // We could handle this with further preprocessing steps but as this case is trivial anyway, we reject the input.
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The Probability for the objective " << *currentObjective.originalFormula << " is always one as the rhs of the until formula is true in the initial state. Please omit this objective.");
                }
                storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> mc(result.preprocessedModel);
                storm::storage::BitVector psiStates = mc.check(formula.getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                
                std::vector<ValueType> objectiveRewards(result.preprocessedModel.getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                for (auto const& state : statesWithMemoryLabel) {
                    for (uint_fast64_t row = result.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state]; row < result.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state + 1]; ++row) {
                        objectiveRewards[row] = result.preprocessedModel.getTransitionMatrix().getConstrainedRowSum(row, psiStates);
                    }
                }
                if (!currentObjective.rewardsArePositive) {
                    storm::utility::vector::scaleVectorInPlace(objectiveRewards, -storm::utility::one<ValueType>());
                }
                result.preprocessedModel.addRewardModel(currentObjective.rewardModelName, RewardModelType(boost::none, objectiveRewards));
            }
            
            template<typename SparseModelType>
            void SparsePcaaPreprocessor<SparseModelType>::preprocessBoundedUntilFormula(storm::logic::BoundedUntilFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective) {
                STORM_LOG_THROW(!result.originalModel.isOfType(storm::models::ModelType::MarkovAutomaton) || !formula.isStepBounded(), storm::exceptions::InvalidPropertyException, "Multi-objective model checking currently does not support STEP-bounded properties for Markov automata.");
                
                if (formula.hasLowerBound()) {
                    currentObjective.lowerTimeBound = formula.getLowerBound();
                    currentObjective.lowerTimeBoundStrict = formula.isLowerBoundStrict();
                }
                if (formula.hasUpperBound()) {
                    currentObjective.upperTimeBound = formula.getUpperBound();
                    currentObjective.upperTimeBoundStrict = formula.isUpperBoundStrict();
                }
                preprocessUntilFormula(storm::logic::UntilFormula(formula.getLeftSubformula().asSharedPointer(), formula.getRightSubformula().asSharedPointer()), result, currentObjective);
            }
            
            template<typename SparseModelType>
            void SparsePcaaPreprocessor<SparseModelType>::preprocessGloballyFormula(storm::logic::GloballyFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective) {
                // The formula will be transformed to an until formula for the complementary event.
                // If the original formula minimizes, the complementary one will maximize and vice versa.
                // Hence, the decision whether to consider positive or negative rewards flips.
                currentObjective.rewardsArePositive = !currentObjective.rewardsArePositive;
                // To transform from the value of the preprocessed model back to the value of the original model, we have to add 1 to the result.
                // The transformation factor has already been set correctly.
                currentObjective.toOriginalValueTransformationOffset = storm::utility::one<ValueType>();
                
                auto negatedSubformula = std::make_shared<storm::logic::UnaryBooleanStateFormula>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, formula.getSubformula().asSharedPointer());
                
                preprocessUntilFormula(storm::logic::UntilFormula(storm::logic::Formula::getTrueFormula(), negatedSubformula), result, currentObjective);
            }
            
            template<typename SparseModelType>
            void SparsePcaaPreprocessor<SparseModelType>::preprocessEventuallyFormula(storm::logic::EventuallyFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective, boost::optional<std::string> const& optionalRewardModelName) {
                if (formula.isReachabilityProbabilityFormula()){
                    preprocessUntilFormula(storm::logic::UntilFormula(storm::logic::Formula::getTrueFormula(), formula.getSubformula().asSharedPointer()), result, currentObjective);
                    return;
                }
                                
                // Create a memory structure that stores whether a target state has already been reached
                storm::storage::MemoryStructureBuilder builder(2);
                // Get a unique label that is not already present in the model
                std::string memoryLabel = "obj" + std::to_string(result.objectives.size());
                while (result.preprocessedModel.hasLabel(memoryLabel)) memoryLabel = "_" + memoryLabel;
                builder.setLabel(0, memoryLabel);
                builder.setTransition(0, 0, std::make_shared<storm::logic::UnaryBooleanStateFormula>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, formula.getSubformula().asSharedPointer()));
                builder.setTransition(0, 1, formula.getSubformula().asSharedPointer());
                builder.setTransition(1, 1, std::make_shared<storm::logic::BooleanLiteralFormula>(true));
                storm::storage::MemoryStructure memory = builder.build();
                addMemoryToPreprocessedModel(result, memory);
                
                // Add a reward model that only gives reward to states with the memory label
                RewardModelType objectiveRewards(boost::none);
                if (formula.isReachabilityRewardFormula()) {
                    storm::storage::BitVector statesWithoutMemoryLabel = ~result.preprocessedModel.getStates(memoryLabel);
                    objectiveRewards = result.preprocessedModel.getRewardModel(optionalRewardModelName ? optionalRewardModelName.get() : "");
                    objectiveRewards.reduceToStateBasedRewards(result.preprocessedModel.getTransitionMatrix(), false);
                    if (objectiveRewards.hasStateRewards()) {
                        storm::utility::vector::setVectorValues(objectiveRewards.getStateRewardVector(), statesWithoutMemoryLabel, storm::utility::zero<ValueType>());
                    }
                    if (objectiveRewards.hasStateActionRewards()) {
                        for (auto state : statesWithoutMemoryLabel) {
                            std::fill_n(objectiveRewards.getStateActionRewardVector().begin() + result.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state], result.preprocessedModel.getTransitionMatrix().getRowGroupSize(state), storm::utility::zero<ValueType>());
                        }
                    }
                } else if (formula.isReachabilityTimeFormula() && result.preprocessedModel.isOfType(storm::models::ModelType::MarkovAutomaton)) {
                    objectiveRewards = RewardModelType(std::vector<ValueType>(result.preprocessedModel.getNumberOfStates(), storm::utility::zero<ValueType>()));
                    storm::utility::vector::setVectorValues(objectiveRewards.getStateRewardVector(), dynamic_cast<storm::models::sparse::MarkovAutomaton<ValueType>*>(&result.preprocessedModel)->getMarkovianStates() & result.preprocessedModel.getStates(memoryLabel), storm::utility::one<ValueType>());
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The formula " << formula << " neither considers reachability probabilities nor reachability rewards " << (result.preprocessedModel.isOfType(storm::models::ModelType::MarkovAutomaton) ?  "nor reachability time" : "") << ". This is not supported.");
                }
                if (!currentObjective.rewardsArePositive){
                    if (objectiveRewards.hasStateRewards()) {
                        storm::utility::vector::scaleVectorInPlace(objectiveRewards.getStateRewardVector(), -storm::utility::one<ValueType>());
                    }
                    if (objectiveRewards.hasStateActionRewards()) {
                        storm::utility::vector::scaleVectorInPlace(objectiveRewards.getStateActionRewardVector(), -storm::utility::one<ValueType>());
                    }
                }
                result.preprocessedModel.addRewardModel(currentObjective.rewardModelName, std::move(objectiveRewards));
            }
            
            template<typename SparseModelType>
            void SparsePcaaPreprocessor<SparseModelType>::preprocessCumulativeRewardFormula(storm::logic::CumulativeRewardFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective, boost::optional<std::string> const& optionalRewardModelName) {
                STORM_LOG_THROW(result.originalModel.isOfType(storm::models::ModelType::Mdp), storm::exceptions::InvalidPropertyException, "Cumulative reward formulas are not supported for the given model type.");
                currentObjective.upperTimeBound = formula.getBound();
                currentObjective.upperTimeBoundStrict = formula.isBoundStrict();
                
                RewardModelType objectiveRewards = result.preprocessedModel.getRewardModel(optionalRewardModelName ? optionalRewardModelName.get() : "");
                objectiveRewards.reduceToStateBasedRewards(result.preprocessedModel.getTransitionMatrix(), false);
                if (!currentObjective.rewardsArePositive){
                    if (objectiveRewards.hasStateRewards()) {
                        storm::utility::vector::scaleVectorInPlace(objectiveRewards.getStateRewardVector(), -storm::utility::one<ValueType>());
                    }
                    if (objectiveRewards.hasStateActionRewards()) {
                        storm::utility::vector::scaleVectorInPlace(objectiveRewards.getStateActionRewardVector(), -storm::utility::one<ValueType>());
                    }
                }
                result.preprocessedModel.addRewardModel(currentObjective.rewardModelName, std::move(objectiveRewards));
            }
            
            template<typename SparseModelType>
            void SparsePcaaPreprocessor<SparseModelType>::preprocessTotalRewardFormula(ReturnType& result, PcaaObjective<ValueType>& currentObjective, boost::optional<std::string> const& optionalRewardModelName) {
                RewardModelType objectiveRewards = result.preprocessedModel.getRewardModel(optionalRewardModelName ? optionalRewardModelName.get() : "");
                objectiveRewards.reduceToStateBasedRewards(result.preprocessedModel.getTransitionMatrix(), false);
                if (!currentObjective.rewardsArePositive){
                    if (objectiveRewards.hasStateRewards()) {
                        storm::utility::vector::scaleVectorInPlace(objectiveRewards.getStateRewardVector(), -storm::utility::one<ValueType>());
                    }
                    if (objectiveRewards.hasStateActionRewards()) {
                        storm::utility::vector::scaleVectorInPlace(objectiveRewards.getStateActionRewardVector(), -storm::utility::one<ValueType>());
                    }
                }
                result.preprocessedModel.addRewardModel(currentObjective.rewardModelName, std::move(objectiveRewards));
            }
            
            
            template<typename SparseModelType>
            void SparsePcaaPreprocessor<SparseModelType>::analyzeEndComponents(ReturnType& result, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {

                result.ecActions = storm::storage::BitVector(result.preprocessedModel.getNumberOfChoices(), false);
                std::vector<storm::storage::MaximalEndComponent> ecs;
                auto mecDecomposition = storm::storage::MaximalEndComponentDecomposition<ValueType>(result.preprocessedModel.getTransitionMatrix(), backwardTransitions);
                STORM_LOG_ASSERT(!mecDecomposition.empty(), "Empty maximal end component decomposition.");
                ecs.reserve(mecDecomposition.size());
                for (auto& mec : mecDecomposition) {
                    for (auto const& stateActionsPair : mec) {
                        for (auto const& action : stateActionsPair.second) {
                            result.ecActions.set(action);
                        }
                    }
                    ecs.push_back(std::move(mec));
                }
                
                result.possiblyRecurrentStates = storm::storage::BitVector(result.preprocessedModel.getNumberOfStates(), false);
                storm::storage::BitVector neutralActions = ~(result.actionsWithNegativeReward | result.actionsWithPositiveReward);
                storm::storage::BitVector statesInCurrentECWithNeutralAction(result.preprocessedModel.getNumberOfStates(), false);
                for (uint_fast64_t ecIndex = 0; ecIndex < ecs.size(); ++ecIndex) { //we will insert new ecs in the vector (thus no iterators for the loop)
                    bool currentECIsNeutral = true;
                    for (auto const& stateActionsPair : ecs[ecIndex]) {
                        bool stateHasNeutralActionWithinEC = false;
                        for (auto const& action : stateActionsPair.second) {
                            stateHasNeutralActionWithinEC |= neutralActions.get(action);
                        }
                        statesInCurrentECWithNeutralAction.set(stateActionsPair.first, stateHasNeutralActionWithinEC);
                        currentECIsNeutral &= stateHasNeutralActionWithinEC;
                    }
                    if (currentECIsNeutral) {
                        result.possiblyRecurrentStates |= statesInCurrentECWithNeutralAction;
                    }else{
                        // Check if the ec contains neutral sub ecs. This is done by adding the subECs to our list of ECs
                        // A neutral subEC only consist of states that can stay in statesInCurrentECWithNeutralAction
                        statesInCurrentECWithNeutralAction = storm::utility::graph::performProb0E(result.preprocessedModel.getTransitionMatrix(), result.preprocessedModel.getTransitionMatrix().getRowGroupIndices(), backwardTransitions,statesInCurrentECWithNeutralAction, ~statesInCurrentECWithNeutralAction);
                        auto subECs = storm::storage::MaximalEndComponentDecomposition<ValueType>(result.preprocessedModel.getTransitionMatrix(), backwardTransitions, statesInCurrentECWithNeutralAction);
                        ecs.reserve(ecs.size() + subECs.size());
                        for (auto& ec : subECs){
                            ecs.push_back(std::move(ec));
                        }
                    }
                    statesInCurrentECWithNeutralAction.clear();
                }
            }
            
            template<typename SparseModelType>
            void SparsePcaaPreprocessor<SparseModelType>::ensureRewardFiniteness(ReturnType& result, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {
                STORM_LOG_THROW((result.actionsWithPositiveReward & result.ecActions).empty(), storm::exceptions::InvalidPropertyException, "Infinite reward: There is one maximizing objective for which infinite reward is possible. This is not supported.");
                
                //Check whether the states that can be visited inf. often are reachable with prob. 1 under some scheduler
                storm::storage::BitVector statesReachingNegativeRewardsFinitelyOftenForSomeScheduler = storm::utility::graph::performProb1E(result.preprocessedModel.getTransitionMatrix(), result.preprocessedModel.getTransitionMatrix().getRowGroupIndices(), backwardTransitions, storm::storage::BitVector(result.preprocessedModel.getNumberOfStates(), true), result.possiblyRecurrentStates);
                STORM_LOG_THROW(!(statesReachingNegativeRewardsFinitelyOftenForSomeScheduler & result.preprocessedModel.getInitialStates()).empty(), storm::exceptions::InvalidPropertyException, "Infinite Rewards: For every scheduler, the induced reward for one or more of the objectives that minimize rewards is infinity.");
                
                if (!statesReachingNegativeRewardsFinitelyOftenForSomeScheduler.full()) {
                    // Remove the states that for any scheduler have one objective with infinite expected reward.
                    auto subsystemBuilderResult = storm::transformer::SubsystemBuilder<SparseModelType>::transform(result.preprocessedModel, statesReachingNegativeRewardsFinitelyOftenForSomeScheduler, storm::storage::BitVector(result.preprocessedModel.getNumberOfChoices(), true));
                    updatePreprocessedModel(result, *subsystemBuilderResult.model, subsystemBuilderResult.newToOldStateIndexMapping);
                    result.ecActions = result.ecActions % subsystemBuilderResult.keptActions;
                    result.actionsWithPositiveReward = result.actionsWithPositiveReward % subsystemBuilderResult.keptActions;
                    result.actionsWithNegativeReward = result.actionsWithNegativeReward % subsystemBuilderResult.keptActions;
                    result.possiblyRecurrentStates = result.possiblyRecurrentStates % statesReachingNegativeRewardsFinitelyOftenForSomeScheduler;
                }
            }
        
        
        
            template class SparsePcaaPreprocessor<storm::models::sparse::Mdp<double>>;
            template class SparsePcaaPreprocessor<storm::models::sparse::MarkovAutomaton<double>>;
            
#ifdef STORM_HAVE_CARL
            template class SparsePcaaPreprocessor<storm::models::sparse::Mdp<storm::RationalNumber>>;
            template class SparsePcaaPreprocessor<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
#endif
        }
    }
}
