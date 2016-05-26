 #include "src/modelchecker/multiobjective/helper/SparseMdpMultiObjectivePreprocessingHelper.h"

#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"

#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "src/storage/BitVector.h"

#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"

#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template<typename SparseMdpModelType>
            SparseMultiObjectiveModelCheckerInformation<SparseMdpModelType> SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocess(storm::logic::MultiObjectiveFormula const& originalFormula, SparseMdpModelType originalModel) {
                Information info(std::move(originalModel));
                
                //Initialize the state mapping.
                info.getNewToOldStateMapping().reserve(info.getModel().getNumberOfStates());
                for(uint_fast64_t state = 0; state < info.getModel().getNumberOfStates(); ++state){
                    info.getNewToOldStateMapping().push_back(state);
                }
                
                //Gather information regarding the individual objectives
                if(!gatherObjectiveInformation(originalFormula, info)){
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Could not gather information for objectives " << originalFormula << ".");
                }
                
                // Find out whether negative rewards should be considered.
                if(!setWhetherNegativeRewardsAreConsidered(info)){
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Could not find out whether to consider negative rewards " << originalFormula << ".");
                }
                
                //Invoke preprocessing on objectives
                bool success = false;
                for (auto& obj : info.getObjectives()){
                    STORM_LOG_DEBUG("Preprocessing objective " << *obj.originalFormula << ".");
                    if(obj.originalFormula->isProbabilityOperatorFormula()){
                        success = preprocess(obj.originalFormula->asProbabilityOperatorFormula(), info, obj);
                    } else if(obj.originalFormula->isRewardOperatorFormula()){
                        success = preprocess(obj.originalFormula->asRewardOperatorFormula(), info, obj);
                    }
                }
                STORM_LOG_THROW(success, storm::exceptions::InvalidPropertyException, "Could not preprocess for the formula " << originalFormula << ".");
                return info;
            }
            
            template<typename SparseMdpModelType>
            bool SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::gatherObjectiveInformation(storm::logic::MultiObjectiveFormula const& formula, Information& info) {
                for(auto const& subF : formula.getSubFormulas()){
                    if(!subF->isOperatorFormula()){
                        STORM_LOG_ERROR("Expected an OperatorFormula as subformula of " << formula << " but got " << *subF);
                        return false;
                    }
                    storm::logic::OperatorFormula const& f = subF->asOperatorFormula();
                    typename Information::ObjectiveInformation objective;
                    
                    objective.originalFormula = subF;
                    
                    if(f.hasBound()){
                        objective.threshold = f.getBound().threshold;
                        // Note that we minimize if the comparison type is an upper bound since we are interested in the EXISTENCE of a scheduler...
                        objective.originalFormulaMinimizes = !storm::logic::isLowerBound(f.getBound().comparisonType);
                    } else if (f.hasOptimalityType()){
                        objective.originalFormulaMinimizes = storm::solver::minimize(f.getOptimalityType());
                    } else {
                        STORM_LOG_ERROR("Current objective" << f << "does not specify whether to minimize or maximize");
                    }
                    
                    objective.rewardModelName = "objective" + std::to_string(info.getObjectives().size());
                    // Make sure the new reward model gets a unique name
                    while(info.getModel().hasRewardModel(objective.rewardModelName)){
                        objective.rewardModelName = "_" + objective.rewardModelName;
                    }
                    
                    if(!setStepBoundOfObjective(objective)) {
                        return false;
                    }
                    
                    info.getObjectives().push_back(std::move(objective));
                }
                return true;
            }
            
            template<typename SparseMdpModelType>
            bool SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::setStepBoundOfObjective(typename Information::ObjectiveInformation& objective){
                if(objective.originalFormula->isProbabilityOperatorFormula()){
                    storm::logic::Formula const& f = objective.originalFormula->asProbabilityOperatorFormula().getSubformula();
                    if(f.isBoundedUntilFormula()){
                        if(f.asBoundedUntilFormula().hasDiscreteTimeBound()){
                            objective.stepBound = f.asBoundedUntilFormula().getDiscreteTimeBound();
                        } else {
                            STORM_LOG_ERROR("Expected a discrete time bound at boundedUntilFormula but got" << f << ".");
                            return false;
                        }
                    }
                } else if(objective.originalFormula->isRewardOperatorFormula()){
                    storm::logic::Formula const& f = objective.originalFormula->asRewardOperatorFormula();
                    if(f.isCumulativeRewardFormula()){
                        if(f.asCumulativeRewardFormula().hasDiscreteTimeBound()){
                            objective.stepBound = f.asCumulativeRewardFormula().getDiscreteTimeBound();
                        } else {
                            STORM_LOG_ERROR("Expected a discrete time bound at cumulativeRewardFormula but got" << f << ".");
                            return false;
                        }
                    }
                } else {
                    STORM_LOG_ERROR("Expected a Probability or Reward OperatorFormula but got " << *objective.originalFormula << ".");
                    return false;
                }
                return true;
            }
            
            template<typename SparseMdpModelType>
            bool SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::setWhetherNegativeRewardsAreConsidered(Information& info) {
                // Negative Rewards are considered  whenever there is an unbounded reward objective that requires to minimize.
                // If there is no unbounded reward objective, we make a majority vote.
                uint_fast64_t numOfMinimizingObjectives = 0;
                for(auto const& obj : info.getObjectives()){
                    if(obj.originalFormula->isRewardOperatorFormula() && !obj.stepBound){
                        info.setNegativeRewardsConsidered(obj.originalFormulaMinimizes);
                        return true;
                    }
                    numOfMinimizingObjectives += obj.originalFormulaMinimizes ? 1 : 0;
                }
                // Reaching this point means that we make a majority vote
                info.setNegativeRewardsConsidered(numOfMinimizingObjectives*2 > info.getObjectives().size());
                return true;
            }
            
            template<typename SparseMdpModelType>
            bool SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocess(storm::logic::ProbabilityOperatorFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective) {
                // Check if we need to complement the property, e.g., P<0.3 [ F "a" ] ---> P >=0.7 [ G !"a" ]
                // This is the case if the formula requires to minimize and positive rewards are considered or vice versa
                if(info.areNegativeRewardsConsidered() != currentObjective.originalFormulaMinimizes){
                    STORM_LOG_ERROR("Inverting of properties not yet supported");
                    //TODO
                    return false;
                }
                
                bool success = false;
                // Invoke preprocessing for subformula
                if(formula.getSubformula().isUntilFormula()){
                    success = preprocess(formula.getSubformula().asUntilFormula(), info, currentObjective);
                } else if(formula.getSubformula().isBoundedUntilFormula()){
                    success = preprocess(formula.getSubformula().asBoundedUntilFormula(), info, currentObjective);
                } else if(formula.getSubformula().isGloballyFormula()){
                    success = preprocess(formula.getSubformula().asGloballyFormula(), info, currentObjective);
                } else if(formula.getSubformula().isEventuallyFormula()){
                    success = preprocess(formula.getSubformula().asEventuallyFormula(), info, currentObjective);
                }
                STORM_LOG_ERROR_COND(success, "No implementation for the subformula of " << formula << ".");
                
                return success;
            }

            template<typename SparseMdpModelType>
            bool SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocess(storm::logic::RewardOperatorFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective) {
                // Make sure that our decision for negative rewards is consistent with the formula
                if(info.areNegativeRewardsConsidered() != currentObjective.originalFormulaMinimizes){
                    STORM_LOG_ERROR("Decided to consider " << (info.areNegativeRewardsConsidered() ? "negative" : "non-negative") << "rewards, but the formula " << formula << (currentObjective.originalFormulaMinimizes ? " minimizes" : "maximizes") << ". Reward objectives should either all minimize or all maximize.");
                    return false;
                }
                
                // Check if the reward model is uniquely specified
                if((formula.hasRewardModelName() && info.getModel().hasRewardModel(formula.getRewardModelName()))
                        || info.getModel().hasUniqueRewardModel()){
                    STORM_LOG_ERROR("The reward model is not unique and the formula " << formula << " does not specify a reward model.");
                    return false;
                }
                
                bool success = false;
                // Invoke preprocessing for subformula
                if(formula.getSubformula().isEventuallyFormula()){
                    success = preprocess(formula.getSubformula().asEventuallyFormula(), info, currentObjective, formula.getOptionalRewardModelName());
                } else if(formula.getSubformula().isCumulativeRewardFormula()) {
                    success = preprocess(formula.getSubformula().asCumulativeRewardFormula(), info, currentObjective, formula.getOptionalRewardModelName());
                }
                STORM_LOG_ERROR_COND(success, "No implementation for the subformula of " << formula << ".");
                
                return success;
                
            }
            
            template<typename SparseMdpModelType>
            bool SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocess(storm::logic::UntilFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective) {
                bool success = false;
                CheckTask<storm::logic::Formula> phiTask(formula.getLeftSubformula());
                CheckTask<storm::logic::Formula> psiTask(formula.getRightSubformula());
                storm::modelchecker::SparsePropositionalModelChecker<SparseMdpModelType> mc(info.getModel());
                success = mc.canHandle(phiTask) && mc.canHandle(psiTask);
                STORM_LOG_ERROR_COND(success, "The subformulas of " << formula << " should be propositional.");
                storm::storage::BitVector phiStates = mc.check(phiTask)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                storm::storage::BitVector psiStates = mc.check(psiTask)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                
                // modelUnfolder(info.model, (~phiStates) | psiStates)
                // info.model = modelUnfolder.info()
                // build info.stateMapping from modelUnfolder.stateMapping
                // build stateaction reward vector
                // insert it in the model information
    
                // TODO
                
               return success;
            }
            
            template<typename SparseMdpModelType>
            bool SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocess(storm::logic::BoundedUntilFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective) {
                    return preprocess(storm::logic::UntilFormula(formula.getLeftSubformula().asSharedPointer(), formula.getRightSubformula().asSharedPointer()), info, currentObjective);
            }
            
            template<typename SparseMdpModelType>
            bool SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocess(storm::logic::GloballyFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective) {
                //TODO
                STORM_LOG_ERROR("Globally not yet implemented");
                return false;
            }
            
            template<typename SparseMdpModelType>
            bool SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocess(storm::logic::EventuallyFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName) {
                if(formula.isReachabilityProbabilityFormula()){
                    return preprocess(storm::logic::UntilFormula(storm::logic::Formula::getTrueFormula(), formula.getSubformula().asSharedPointer()), info, currentObjective);
                }
                if (!formula.isReachabilityRewardFormula()){
                    STORM_LOG_ERROR("The formula " << formula << " neither considers reachability Probabilities nor reachability rewards");
                    return false;
                }
                //TODO
                STORM_LOG_ERROR("Rewards not yet implemented");
                return false;
            }
            
            template<typename SparseMdpModelType>
            bool SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocess(storm::logic::CumulativeRewardFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName) {
                //TODO
                STORM_LOG_ERROR("Rewards not yet implemented");
                return false;
            }

            
            template class SparseMdpMultiObjectivePreprocessingHelper<storm::models::sparse::Mdp<double>>;

            
        }
    }
}
