 #include "src/modelchecker/multiobjective/helper/SparseMdpMultiObjectivePreprocessingHelper.h"

#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"

#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "src/transformer/StateDuplicator.h"

#include "src/storage/BitVector.h"

#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"

#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template<typename SparseMdpModelType>
            SparseMultiObjectiveModelCheckerInformation<SparseMdpModelType> SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocess(storm::logic::MultiObjectiveFormula const& originalFormula, SparseMdpModelType originalModel) {
                Information info(std::move(originalModel));
                //Initialize the state mapping.
                info.newToOldStateIndexMapping = storm::utility::vector::buildVectorForRange(0, info.model.getNumberOfStates());
                //Gather information regarding the individual objectives
                for(auto const& subFormula : originalFormula.getSubFormulas()){
                    addObjective(subFormula, info);
                }
                // Find out whether negated rewards should be considered.
                setWhetherNegatedRewardsAreConsidered(info);
                //Invoke preprocessing on objectives
                for (auto& obj : info.objectives){
                    STORM_LOG_DEBUG("Preprocessing objective " << *obj.originalFormula << ".");
                    if(obj.originalFormula->isProbabilityOperatorFormula()){
                        preprocessFormula(obj.originalFormula->asProbabilityOperatorFormula(), info, obj);
                    } else if(obj.originalFormula->isRewardOperatorFormula()){
                        preprocessFormula(obj.originalFormula->asRewardOperatorFormula(), info, obj);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Could not preprocess the subformula " << *obj.originalFormula << " of " << originalFormula << " because it is not supported");
                    }
                }
                //We can now remove all original reward models to save some memory
                std::set<std::string> origRewardModels =  originalFormula.getReferencedRewardModels();
                for (auto const& rewModel : origRewardModels){
                    info.model.removeRewardModel(rewModel);
                }
                return info;
            }
            
            template<typename SparseMdpModelType>
            void SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::addObjective(std::shared_ptr<storm::logic::Formula const> const& formula, Information& info) {
                STORM_LOG_THROW(formula->isOperatorFormula(), storm::exceptions::InvalidPropertyException, "Expected an OperatorFormula as subformula of multi-objective query  but got " << *formula);
                
                typename Information::ObjectiveInformation objective;
                objective.originalFormula = formula;
                
                storm::logic::OperatorFormula const& opFormula = formula->asOperatorFormula();
                if(opFormula.hasBound()){
                    objective.threshold = opFormula.getBound().threshold;
                    // Note that we minimize if the comparison type is an upper bound since we are interested in the EXISTENCE of a scheduler.
                    objective.originalFormulaMinimizes = !storm::logic::isLowerBound(opFormula.getBound().comparisonType);
                } else if (opFormula.hasOptimalityType()){
                    objective.originalFormulaMinimizes = storm::solver::minimize(opFormula.getOptimalityType());
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Current objective " << opFormula << " does not specify whether to minimize or maximize");
                }
                objective.rewardModelName = "objective" + std::to_string(info.objectives.size());
                // Make sure the new reward model gets a unique name
                while(info.model.hasRewardModel(objective.rewardModelName)){
                    objective.rewardModelName = "_" + objective.rewardModelName;
                }
                setStepBoundOfObjective(objective);
                info.objectives.push_back(std::move(objective));
            }
            
            template<typename SparseMdpModelType>
            void SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::setStepBoundOfObjective(typename Information::ObjectiveInformation& objective){
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
            
            template<typename SparseMdpModelType>
            void SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::setWhetherNegatedRewardsAreConsidered(Information& info) {
                // Negative Rewards are considered  whenever there is an unbounded reward objective that requires to minimize.
                // If there is no unbounded reward objective, we make a majority vote.
                uint_fast64_t numOfMinimizingObjectives = 0;
                for(auto const& obj : info.objectives){
                    if(obj.originalFormula->isRewardOperatorFormula() && !obj.stepBound){
                        info.negatedRewardsConsidered = obj.originalFormulaMinimizes;
                        return;
                    }
                    numOfMinimizingObjectives += obj.originalFormulaMinimizes ? 1 : 0;
                }
                // Reaching this point means that we make a majority vote
                info.negatedRewardsConsidered = (numOfMinimizingObjectives*2) > info.objectives.size();
            }
            
            template<typename SparseMdpModelType>
            void SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocessFormula(storm::logic::ProbabilityOperatorFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective) {
                // Check if we need to complement the property, e.g., P<0.3 [ F "a" ] ---> P >=0.7 [ G !"a" ]
                // This is the case if the formula requires to minimize and positive rewards are considered or vice versa
                if(info.negatedRewardsConsidered != currentObjective.originalFormulaMinimizes){
                    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Inverting of properties not supported yet");
                    //TODO
                }
                
                if(info.negatedRewardsConsidered && currentObjective.threshold){
                    *(currentObjective.threshold) *= -storm::utility::one<ValueType>();
                }
                
                // Invoke preprocessing for subformula
                if(formula.getSubformula().isUntilFormula()){
                    preprocessFormula(formula.getSubformula().asUntilFormula(), info, currentObjective);
                } else if(formula.getSubformula().isBoundedUntilFormula()){
                    preprocessFormula(formula.getSubformula().asBoundedUntilFormula(), info, currentObjective);
                } else if(formula.getSubformula().isGloballyFormula()){
                    preprocessFormula(formula.getSubformula().asGloballyFormula(), info, currentObjective);
                } else if(formula.getSubformula().isEventuallyFormula()){
                    preprocessFormula(formula.getSubformula().asEventuallyFormula(), info, currentObjective);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }

            template<typename SparseMdpModelType>
            void SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocessFormula(storm::logic::RewardOperatorFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective) {
                // Make sure that our decision for negative rewards is consistent with the formula
                STORM_LOG_THROW(info.negatedRewardsConsidered == currentObjective.originalFormulaMinimizes, storm::exceptions::InvalidPropertyException, "Decided to consider " << (info.negatedRewardsConsidered ? "negated" : "non-negated") << "rewards, but the formula " << formula << (currentObjective.originalFormulaMinimizes ? " minimizes" : "maximizes") << ". Reward objectives should either all minimize or all maximize.");
                
                // Check if the reward model is uniquely specified
                STORM_LOG_THROW((formula.hasRewardModelName() && info.model.hasRewardModel(formula.getRewardModelName()))
                                || info.model.hasUniqueRewardModel(), storm::exceptions::InvalidPropertyException, "The reward model is not unique and the formula " << formula << " does not specify a reward model.");
                
                if(info.negatedRewardsConsidered && currentObjective.threshold){
                    *(currentObjective.threshold) *= -storm::utility::one<ValueType>();
                }
                
                // Invoke preprocessing for subformula
                if(formula.getSubformula().isEventuallyFormula()){
                    preprocessFormula(formula.getSubformula().asEventuallyFormula(), info, currentObjective, formula.getOptionalRewardModelName());
                } else if(formula.getSubformula().isCumulativeRewardFormula()) {
                    preprocessFormula(formula.getSubformula().asCumulativeRewardFormula(), info, currentObjective, formula.getOptionalRewardModelName());
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }
            
            template<typename SparseMdpModelType>
            void SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocessFormula(storm::logic::UntilFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective) {
                CheckTask<storm::logic::Formula> phiTask(formula.getLeftSubformula());
                CheckTask<storm::logic::Formula> psiTask(formula.getRightSubformula());
                storm::modelchecker::SparsePropositionalModelChecker<SparseMdpModelType> mc(info.model);
                STORM_LOG_THROW(mc.canHandle(phiTask) && mc.canHandle(psiTask), storm::exceptions::InvalidPropertyException, "The subformulas of " << formula << " should be propositional.");
                storm::storage::BitVector phiStates = mc.check(phiTask)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                storm::storage::BitVector psiStates = mc.check(psiTask)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                
                auto duplicatorResult = storm::transformer::StateDuplicator<SparseMdpModelType>::transform(info.model, ~phiStates | psiStates);
                info.model = std::move(*duplicatorResult.model);
                // duplicatorResult.newToOldStateIndexMapping now reffers to the indices of the model we had before preprocessing this formula.
                // This might not be the actual original model.
                for(auto & originalModelStateIndex : duplicatorResult.newToOldStateIndexMapping){
                    originalModelStateIndex = info.newToOldStateIndexMapping[originalModelStateIndex];
                }
                info.newToOldStateIndexMapping = std::move(duplicatorResult.newToOldStateIndexMapping);
                
                // build stateAction reward vector that gives (one*transitionProbability) reward whenever a transition leads from the firstCopy to a psiState
                storm::storage::BitVector newPsiStates(info.model.getNumberOfStates(), false);
                for(auto const& oldPsiState : psiStates){
                    //note that psiStates are always located in the second copy
                    newPsiStates.set(duplicatorResult.secondCopyOldToNewStateIndexMapping[oldPsiState], true);
                }
                std::vector<ValueType> objectiveRewards(info.model.getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                for (auto const& firstCopyState : duplicatorResult.firstCopy) {
                    for (uint_fast64_t row = info.model.getTransitionMatrix().getRowGroupIndices()[firstCopyState]; row < info.model.getTransitionMatrix().getRowGroupIndices()[firstCopyState + 1]; ++row) {
                        objectiveRewards[row] = info.model.getTransitionMatrix().getConstrainedRowSum(row, newPsiStates);
                    }
                }
                if(info.negatedRewardsConsidered){
                    storm::utility::vector::scaleVectorInPlace(objectiveRewards, -storm::utility::one<ValueType>());
                }
                info.model.addRewardModel(currentObjective.rewardModelName, RewardModelType(boost::none, objectiveRewards));
            }
            
            template<typename SparseMdpModelType>
            void SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocessFormula(storm::logic::BoundedUntilFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective) {
                    preprocessFormula(storm::logic::UntilFormula(formula.getLeftSubformula().asSharedPointer(), formula.getRightSubformula().asSharedPointer()), info, currentObjective);
            }
            
            template<typename SparseMdpModelType>
            void SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocessFormula(storm::logic::GloballyFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective) {
                //TODO
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Globally not yet implemented");
            }
            
            template<typename SparseMdpModelType>
            void SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocessFormula(storm::logic::EventuallyFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName) {
                if(formula.isReachabilityProbabilityFormula()){
                    preprocessFormula(storm::logic::UntilFormula(storm::logic::Formula::getTrueFormula(), formula.getSubformula().asSharedPointer()), info, currentObjective);
                    return;
                }
                STORM_LOG_THROW(formula.isReachabilityRewardFormula(), storm::exceptions::InvalidPropertyException, "The formula " << formula << " neither considers reachability Probabilities nor reachability rewards");
                
                CheckTask<storm::logic::Formula> targetTask(formula.getSubformula());
                storm::modelchecker::SparsePropositionalModelChecker<SparseMdpModelType> mc(info.model);
                STORM_LOG_THROW(mc.canHandle(targetTask), storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " should be propositional.");
                storm::storage::BitVector targetStates = mc.check(targetTask)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                
                STORM_LOG_WARN("There is no check for reward finiteness yet"); //TODO
                auto duplicatorResult = storm::transformer::StateDuplicator<SparseMdpModelType>::transform(info.model, targetStates);
                info.model = std::move(*duplicatorResult.model);
                // duplicatorResult.newToOldStateIndexMapping now reffers to the indices of the model we had before preprocessing this formula.
                // This might not be the actual original model.
                for(auto & originalModelStateIndex : duplicatorResult.newToOldStateIndexMapping){
                    originalModelStateIndex = info.newToOldStateIndexMapping[originalModelStateIndex];
                }
                info.newToOldStateIndexMapping = std::move(duplicatorResult.newToOldStateIndexMapping);
                
                // Add a reward model that gives zero reward to the states of the second copy.
                std::vector<ValueType> objectiveRewards = info.model.getRewardModel(optionalRewardModelName ? optionalRewardModelName.get() : "").getTotalRewardVector(info.model.getTransitionMatrix());
                storm::utility::vector::setVectorValues(objectiveRewards, duplicatorResult.secondCopy, storm::utility::zero<ValueType>());
                if(info.negatedRewardsConsidered){
                    storm::utility::vector::scaleVectorInPlace(objectiveRewards, -storm::utility::one<ValueType>());
                }
                info.model.addRewardModel(currentObjective.rewardModelName, RewardModelType(boost::none, objectiveRewards));
            }
            
            template<typename SparseMdpModelType>
            void SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocessFormula(storm::logic::CumulativeRewardFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName) {
                std::vector<ValueType> objectiveRewards = info.model.getRewardModel(optionalRewardModelName ? optionalRewardModelName.get() : "").getTotalRewardVector(info.model.getTransitionMatrix());
                if(info.negatedRewardsConsidered){
                    storm::utility::vector::scaleVectorInPlace(objectiveRewards, -storm::utility::one<ValueType>());
                }
                info.model.addRewardModel(currentObjective.rewardModelName, RewardModelType(boost::none, objectiveRewards));
            }
            
            template class SparseMdpMultiObjectivePreprocessingHelper<storm::models::sparse::Mdp<double>>;

            
        }
    }
}
