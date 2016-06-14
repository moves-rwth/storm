 #include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePreprocessor.h"

#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/storage/BitVector.h"
#include "src/transformer/StateDuplicator.h"
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
                PreprocessorData data(std::move(originalModel));
                //Initialize the state mapping.
                data.newToOldStateIndexMapping = storm::utility::vector::buildVectorForRange(0, data.preprocessedModel.getNumberOfStates());
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
                if(formula.getSubformula().isUntilFormula()){
                    preprocessFormula(formula.getSubformula().asUntilFormula(), data, currentObjective);
                } else if(formula.getSubformula().isBoundedUntilFormula()){
                    preprocessFormula(formula.getSubformula().asBoundedUntilFormula(), data, currentObjective);
                } else if(formula.getSubformula().isGloballyFormula()){
                    preprocessFormula(formula.getSubformula().asGloballyFormula(), data, currentObjective);
                } else if(formula.getSubformula().isEventuallyFormula()){
                    preprocessFormula(formula.getSubformula().asEventuallyFormula(), data, currentObjective);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }

            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::RewardOperatorFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective) {
                // Check if the reward model is uniquely specified
                STORM_LOG_THROW((formula.hasRewardModelName() && data.preprocessedModel.hasRewardModel(formula.getRewardModelName()))
                                || data.preprocessedModel.hasUniqueRewardModel(), storm::exceptions::InvalidPropertyException, "The reward model is not unique and the formula " << formula << " does not specify a reward model.");
                if(formula.getSubformula().isEventuallyFormula()){
                    preprocessFormula(formula.getSubformula().asEventuallyFormula(), data, currentObjective, formula.getOptionalRewardModelName());
                } else if(formula.getSubformula().isCumulativeRewardFormula()) {
                    preprocessFormula(formula.getSubformula().asCumulativeRewardFormula(), data, currentObjective, formula.getOptionalRewardModelName());
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::UntilFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective) {
                CheckTask<storm::logic::Formula> phiTask(formula.getLeftSubformula());
                CheckTask<storm::logic::Formula> psiTask(formula.getRightSubformula());
                storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> mc(data.preprocessedModel);
                STORM_LOG_THROW(mc.canHandle(phiTask) && mc.canHandle(psiTask), storm::exceptions::InvalidPropertyException, "The subformulas of " << formula << " should be propositional.");
                storm::storage::BitVector phiStates = mc.check(phiTask)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                storm::storage::BitVector psiStates = mc.check(psiTask)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                
                auto duplicatorResult = storm::transformer::StateDuplicator<SparseModelType>::transform(data.preprocessedModel, ~phiStates | psiStates);
                data.preprocessedModel = std::move(*duplicatorResult.model);
                // duplicatorResult.newToOldStateIndexMapping now reffers to the indices of the model we had before preprocessing this formula.
                // This might not be the actual original model.
                for(auto & originalModelStateIndex : duplicatorResult.newToOldStateIndexMapping){
                    originalModelStateIndex = data.newToOldStateIndexMapping[originalModelStateIndex];
                }
                data.newToOldStateIndexMapping = std::move(duplicatorResult.newToOldStateIndexMapping);
                
                // build stateAction reward vector that gives (one*transitionProbability) reward whenever a transition leads from the firstCopy to a psiState
                storm::storage::BitVector newPsiStates(data.preprocessedModel.getNumberOfStates(), false);
                for(auto const& oldPsiState : psiStates){
                    //note that psiStates are always located in the second copy
                    newPsiStates.set(duplicatorResult.secondCopyOldToNewStateIndexMapping[oldPsiState], true);
                }
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
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::BoundedUntilFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective) {
                STORM_LOG_THROW(formula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Expected a boundedUntilFormula with a discrete time bound but got " << formula << ".");
                currentObjective.stepBound = formula.getDiscreteTimeBound();
                    preprocessFormula(storm::logic::UntilFormula(formula.getLeftSubformula().asSharedPointer(), formula.getRightSubformula().asSharedPointer()), data, currentObjective);
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::GloballyFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective) {
                // The formula will be transformed to an until formula for the complementary event.
                // If the original formula minimizes, the complementary one will maximize and vice versa.
                // Hence, the decision whether to consider positive or negative rewards flips.
                currentObjective.rewardsArePositive = !currentObjective.rewardsArePositive;
                // To transform from the value of the preprocessed model back to the value of the original model, we have to add 1 to the result.
                // The transformation factor has already been set correctly
                currentObjective.toOriginalValueTransformationOffset = storm::utility::one<ValueType>();
                if(currentObjective.threshold) {
                    // Strict thresholds become non-strict and vice versa
                    currentObjective.thresholdIsStrict = !currentObjective.thresholdIsStrict;
                }
                auto negatedSubformula = std::make_shared<storm::logic::UnaryBooleanStateFormula>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, formula.getSubformula().asSharedPointer());
                preprocessFormula(storm::logic::UntilFormula(storm::logic::Formula::getTrueFormula(), negatedSubformula), data, currentObjective);
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::EventuallyFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName) {
                if(formula.isReachabilityProbabilityFormula()){
                    preprocessFormula(storm::logic::UntilFormula(storm::logic::Formula::getTrueFormula(), formula.getSubformula().asSharedPointer()), data, currentObjective);
                    return;
                }
                STORM_LOG_THROW(formula.isReachabilityRewardFormula(), storm::exceptions::InvalidPropertyException, "The formula " << formula << " neither considers reachability Probabilities nor reachability rewards");
                
                CheckTask<storm::logic::Formula> targetTask(formula.getSubformula());
                storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> mc(data.preprocessedModel);
                STORM_LOG_THROW(mc.canHandle(targetTask), storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " should be propositional.");
                storm::storage::BitVector targetStates = mc.check(targetTask)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                
                STORM_LOG_WARN("There is no check for reward finiteness yet"); //TODO
                auto duplicatorResult = storm::transformer::StateDuplicator<SparseModelType>::transform(data.preprocessedModel, targetStates);
                data.preprocessedModel = std::move(*duplicatorResult.model);
                // duplicatorResult.newToOldStateIndexMapping now reffers to the indices of the model we had before preprocessing this formula.
                // This might not be the actual original model.
                for(auto & originalModelStateIndex : duplicatorResult.newToOldStateIndexMapping){
                    originalModelStateIndex = data.newToOldStateIndexMapping[originalModelStateIndex];
                }
                data.newToOldStateIndexMapping = std::move(duplicatorResult.newToOldStateIndexMapping);
                
                // Add a reward model that gives zero reward to the states of the second copy.
                std::vector<ValueType> objectiveRewards = data.preprocessedModel.getRewardModel(optionalRewardModelName ? optionalRewardModelName.get() : "").getTotalRewardVector(data.preprocessedModel.getTransitionMatrix());
                storm::utility::vector::setVectorValues(objectiveRewards, duplicatorResult.secondCopy, storm::utility::zero<ValueType>());
                storm::storage::BitVector positiveRewards = storm::utility::vector::filterGreaterZero(objectiveRewards);
                storm::storage::BitVector nonNegativeRewards = positiveRewards | storm::utility::vector::filterZero(objectiveRewards);
                STORM_LOG_THROW(nonNegativeRewards.full() || positiveRewards.empty(), storm::exceptions::InvalidPropertyException, "The reward model for the formula " << formula << " has positive and negative rewards which is not supported.");
                if(!currentObjective.rewardsArePositive){
                    storm::utility::vector::scaleVectorInPlace(objectiveRewards, -storm::utility::one<ValueType>());
                }
                data.preprocessedModel.addRewardModel(currentObjective.rewardModelName, RewardModelType(boost::none, objectiveRewards));
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::CumulativeRewardFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName) {
                STORM_LOG_THROW(formula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Expected a cumulativeRewardFormula with a discrete time bound but got " << formula << ".");
                currentObjective.stepBound = formula.getDiscreteTimeBound();
                
                std::vector<ValueType> objectiveRewards = data.preprocessedModel.getRewardModel(optionalRewardModelName ? optionalRewardModelName.get() : "").getTotalRewardVector(data.preprocessedModel.getTransitionMatrix());
                if(!currentObjective.rewardsArePositive){
                    storm::utility::vector::scaleVectorInPlace(objectiveRewards, -storm::utility::one<ValueType>());
                }
                data.preprocessedModel.addRewardModel(currentObjective.rewardModelName, RewardModelType(boost::none, objectiveRewards));
            }
            
            template class SparseMultiObjectivePreprocessor<storm::models::sparse::Mdp<double>>;

            
        }
    }
}
