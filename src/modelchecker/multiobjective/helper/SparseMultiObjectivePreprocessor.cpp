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
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template<typename SparseModelType>
            typename SparseMultiObjectivePreprocessor<SparseModelType>::ReturnType SparseMultiObjectivePreprocessor<SparseModelType>::preprocess(storm::logic::MultiObjectiveFormula const& originalFormula, SparseModelType const& originalModel) {
                ReturnType data(std::move(originalModel));
                //Initialize the state mapping.
                data.newToOldStateIndexMapping = storm::utility::vector::buildVectorForRange(0, data.preprocessedModel.getNumberOfStates());
                //Gather information regarding the individual objectives
                for(auto const& subFormula : originalFormula.getSubFormulas()){
                    addObjective(subFormula, data);
                }
                //Invoke preprocessing on objectives
                for (auto& obj : data.objectives){
                    STORM_LOG_DEBUG("Preprocessing objective " << *obj.originalFormula << ".");
                    if(obj.originalFormula->isProbabilityOperatorFormula()){
                        preprocessFormula(obj.originalFormula->asProbabilityOperatorFormula(), data, obj);
                    } else if(obj.originalFormula->isRewardOperatorFormula()){
                        preprocessFormula(obj.originalFormula->asRewardOperatorFormula(), data, obj);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Could not preprocess the subformula " << *obj.originalFormula << " of " << originalFormula << " because it is not supported");
                    }
                }
                //We can now remove all original reward models to save some memory
                std::set<std::string> origRewardModels =  originalFormula.getReferencedRewardModels();
                for (auto const& rewModel : origRewardModels){
                    data.preprocessedModel.removeRewardModel(rewModel);
                }
                return data;
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::addObjective(std::shared_ptr<storm::logic::Formula const> const& formula, ReturnType& data) {
                STORM_LOG_THROW(formula->isOperatorFormula(), storm::exceptions::InvalidPropertyException, "Expected an OperatorFormula as subformula of multi-objective query  but got " << *formula);
                
                ObjectiveInformation objective;
                objective.originalFormula = formula;
                objective.rewardModelName = "objective" + std::to_string(data.objectives.size());
                // Make sure the new reward model gets a unique name
                while(data.preprocessedModel.hasRewardModel(objective.rewardModelName)){
                    objective.rewardModelName = "_" + objective.rewardModelName;
                }
                data.objectives.push_back(std::move(objective));
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::setStepBoundOfObjective(ObjectiveInformation& objective){
                if(objective.originalFormula->isProbabilityOperatorFormula()){
                    storm::logic::Formula const& f = objective.originalFormula->asProbabilityOperatorFormula().getSubformula();
                    if(f.isBoundedUntilFormula()){
                        STORM_LOG_THROW(f.asBoundedUntilFormula().hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Expected a boundedUntilFormula with a discrete time bound but got " << f << ".");
                        objective.stepBound = f.asBoundedUntilFormula().getDiscreteTimeBound();
                    }
                } else if(objective.originalFormula->isRewardOperatorFormula()){
                    storm::logic::Formula const& f = objective.originalFormula->asRewardOperatorFormula();
                    if(f.isCumulativeRewardFormula()){
                        STORM_LOG_THROW(f.asCumulativeRewardFormula().hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Expected a cumulativeRewardFormula with a discrete time bound but got " << f << ".");
                        objective.stepBound = f.asCumulativeRewardFormula().getDiscreteTimeBound();
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Expected a Probability or Reward OperatorFormula but got " << *objective.originalFormula << ".");
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::ProbabilityOperatorFormula const& formula, ReturnType& data, ObjectiveInformation& currentObjective) {
                //Set the information for the objective
                if(formula.hasBound()) {
                    currentObjective.threshold = formula.getBound().threshold;
                    currentObjective.thresholdIsStrict = storm::logic::isStrict(formula.getBound().comparisonType);
                    currentObjective.isNegative = !storm::logic::isLowerBound(formula.getBound().comparisonType);
                    if(currentObjective.isNegative) {
                        *currentObjective.threshold *= -storm::utility::one<double>();
                    }
                } else if (formula.hasOptimalityType()){
                    currentObjective.isNegative = storm::solver::minimize(formula.getOptimalityType());
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Current objective " << formula << " does not specify whether to minimize or maximize");
                }
                
                // Invoke preprocessing for subformula
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
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::RewardOperatorFormula const& formula, ReturnType& data, ObjectiveInformation& currentObjective) {
                //TODO  Make sure that our decision for negative rewards is consistent with the formula
               // STORM_LOG_THROW(data.negatedRewardsConsidered == currentObjective.originalFormulaMinimizes, storm::exceptions::InvalidPropertyException, "Decided to consider " << (data.negatedRewardsConsidered ? "negated" : "non-negated") << "rewards, but the formula " << formula << (currentObjective.originalFormulaMinimizes ? " minimizes" : "maximizes") << ". Reward objectives should either all minimize or all maximize.");
                
                // Check if the reward model is uniquely specified
                STORM_LOG_THROW((formula.hasRewardModelName() && data.preprocessedModel.hasRewardModel(formula.getRewardModelName()))
                                || data.preprocessedModel.hasUniqueRewardModel(), storm::exceptions::InvalidPropertyException, "The reward model is not unique and the formula " << formula << " does not specify a reward model.");
                
                //Set the information for the objective
                if(formula.hasBound()) {
                    currentObjective.threshold = formula.getBound().threshold;
                    currentObjective.thresholdIsStrict = storm::logic::isStrict(formula.getBound().comparisonType);
                    currentObjective.isNegative = !storm::logic::isLowerBound(formula.getBound().comparisonType);
                    if(currentObjective.isNegative) {
                        *currentObjective.threshold *= -storm::utility::one<double>();
                    }
                } else if (formula.hasOptimalityType()){
                    currentObjective.isNegative = storm::solver::minimize(formula.getOptimalityType());
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Current objective " << formula << " does not specify whether to minimize or maximize");
                }
                
                // Invoke preprocessing for subformula
                if(formula.getSubformula().isEventuallyFormula()){
                    preprocessFormula(formula.getSubformula().asEventuallyFormula(), data, currentObjective, formula.getOptionalRewardModelName());
                } else if(formula.getSubformula().isCumulativeRewardFormula()) {
                    preprocessFormula(formula.getSubformula().asCumulativeRewardFormula(), data, currentObjective, formula.getOptionalRewardModelName());
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::UntilFormula const& formula, ReturnType& data, ObjectiveInformation& currentObjective) {
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
                if(currentObjective.isNegative){
                    storm::utility::vector::scaleVectorInPlace(objectiveRewards, -storm::utility::one<ValueType>());
                }
                data.preprocessedModel.addRewardModel(currentObjective.rewardModelName, RewardModelType(boost::none, objectiveRewards));
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::BoundedUntilFormula const& formula, ReturnType& data, ObjectiveInformation& currentObjective) {
                STORM_LOG_THROW(formula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Expected a boundedUntilFormula with a discrete time bound but got " << formula << ".");
                currentObjective.stepBound = formula.getDiscreteTimeBound();
                    preprocessFormula(storm::logic::UntilFormula(formula.getLeftSubformula().asSharedPointer(), formula.getRightSubformula().asSharedPointer()), data, currentObjective);
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::GloballyFormula const& formula, ReturnType& data, ObjectiveInformation& currentObjective) {
                // The formula will be transformed to an until formula for the complementary event
                currentObjective.isInverted = true;
                if(currentObjective.threshold) {
                    // the threshold changes from e.g. '> -p' to '>= 1-p' and '> p' to '>= p-1'
                    if(currentObjective.isNegative) {
                        *currentObjective.threshold += storm::utility::one<double>();
                    } else {
                        *currentObjective.threshold -= storm::utility::one<double>();
                    }
                    currentObjective.thresholdIsStrict = !currentObjective.thresholdIsStrict;
                }
                currentObjective.isNegative = !currentObjective.isNegative;
                auto negatedSubformula = std::make_shared<storm::logic::UnaryBooleanStateFormula>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, formula.getSubformula().asSharedPointer());
                preprocessFormula(storm::logic::UntilFormula(storm::logic::Formula::getTrueFormula(), negatedSubformula), data, currentObjective);
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::EventuallyFormula const& formula, ReturnType& data, ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName) {
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
                if(currentObjective.isNegative){
                    storm::utility::vector::scaleVectorInPlace(objectiveRewards, -storm::utility::one<ValueType>());
                }
                data.preprocessedModel.addRewardModel(currentObjective.rewardModelName, RewardModelType(boost::none, objectiveRewards));
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessFormula(storm::logic::CumulativeRewardFormula const& formula, ReturnType& data, ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName) {
                STORM_LOG_THROW(formula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Expected a cumulativeRewardFormula with a discrete time bound but got " << formula << ".");
                currentObjective.stepBound = formula.getDiscreteTimeBound();
                
                std::vector<ValueType> objectiveRewards = data.preprocessedModel.getRewardModel(optionalRewardModelName ? optionalRewardModelName.get() : "").getTotalRewardVector(data.preprocessedModel.getTransitionMatrix());
                if(currentObjective.isNegative){
                    storm::utility::vector::scaleVectorInPlace(objectiveRewards, -storm::utility::one<ValueType>());
                }
                data.preprocessedModel.addRewardModel(currentObjective.rewardModelName, RewardModelType(boost::none, objectiveRewards));
            }
            
            template class SparseMultiObjectivePreprocessor<storm::models::sparse::Mdp<double>>;

            
        }
    }
}
