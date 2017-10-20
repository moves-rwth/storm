#include "storm/modelchecker/abstraction/PartialBisimulationMdpModelChecker.h"

#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/Mdp.h"
#include "storm/models/symbolic/StochasticTwoPlayerGame.h"

#include "storm/modelchecker/results/CheckResult.h"
#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "storm/modelchecker/prctl/SymbolicDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SymbolicMdpPrctlModelChecker.h"

#include "storm/logic/FragmentSpecification.h"

#include "storm/storage/dd/Bdd.h"
#include "storm/storage/dd/BisimulationDecomposition.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/AbstractionSettings.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/InvalidTypeException.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ModelType>
        PartialBisimulationMdpModelChecker<ModelType>::PartialBisimulationMdpModelChecker(ModelType const& model) : AbstractModelChecker<ModelType>(), model(model) {
            // Intentionally left empty.
        }
        
        template<typename ModelType>
        bool PartialBisimulationMdpModelChecker<ModelType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            storm::logic::FragmentSpecification fragment = storm::logic::reachability().setRewardOperatorsAllowed(true).setReachabilityRewardFormulasAllowed(true);
            return formula.isInFragment(fragment) && checkTask.isOnlyInitialStatesRelevantSet();
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) {
            return computeValuesAbstractionRefinement(false, checkTask.substituteFormula<storm::logic::Formula>(checkTask.getFormula()));
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            return computeValuesAbstractionRefinement(false, checkTask.substituteFormula<storm::logic::Formula>(checkTask.getFormula()));
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeReachabilityRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(rewardMeasureType == storm::logic::RewardMeasureType::Expectation, storm::exceptions::InvalidPropertyException, "Can only compute reward expectations.");
            return computeValuesAbstractionRefinement(true, checkTask.template substituteFormula<storm::logic::Formula>(checkTask.getFormula()));
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeValuesAbstractionRefinement(bool rewards, CheckTask<storm::logic::Formula> const& checkTask) {

            STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::InvalidPropertyException, "The game-based abstraction refinement model checker can only compute the result for the initial states.");
            
            // Create the appropriate preservation information.
            storm::dd::bisimulation::PreservationInformation<DdType, ValueType> preservationInformation(model, storm::storage::BisimulationType::Strong);
            if (checkTask.isRewardModelSet()) {
                if (checkTask.getRewardModel() != "" || model.hasRewardModel(checkTask.getRewardModel())) {
                    preservationInformation.addRewardModel(checkTask.getRewardModel());
                } else if (model.hasUniqueRewardModel()) {
                    preservationInformation.addRewardModel(model.getUniqueRewardModelName());
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Property refers to illegal reward model '" << checkTask.getRewardModel() << "'.");
                }
            }
            
            // Create a bisimulation object that is used to obtain (partial) quotients.
            storm::dd::BisimulationDecomposition<DdType, ValueType> bisimulation(this->model, {checkTask.getFormula().asSharedPointer()}, storm::storage::BisimulationType::Strong);
            
            auto start = std::chrono::high_resolution_clock::now();
            
            uint64_t iterations = 0;
            std::unique_ptr<CheckResult> result;
            while (!result) {
                bool fullQuotient = bisimulation.getReachedFixedPoint();
                std::shared_ptr<storm::models::Model<ValueType>> quotient = bisimulation.getQuotient();
                STORM_LOG_TRACE("Model in iteration " << (iterations + 1) << " has " << quotient->getNumberOfStates() << " states and " << quotient->getNumberOfTransitions() << " transitions.");
                
                if (fullQuotient) {
                    STORM_LOG_TRACE("Reached final quotient.");
                    quotient->printModelInformationToStream(std::cout);
                    result = computeResultFullQuotient(*quotient, rewards, checkTask);
                } else {
                    // Obtain lower and upper bounds from the partial quotient.
                    std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> bounds = computeBoundsPartialQuotient(*quotient, rewards, checkTask);
                    
                    // Check whether the bounds are sufficiently close.
                    bool converged = checkBoundsSufficientlyClose(bounds);
                    if (converged) {
                        result = getAverageOfBounds(bounds);
                    } else {
                        printBoundsInformation(bounds);
                        
                        STORM_LOG_TRACE("Performing bisimulation step.");
                        bisimulation.compute(10);
                    }
                }
                
                ++iterations;
            }

            auto end = std::chrono::high_resolution_clock::now();
            STORM_LOG_TRACE("Completed abstraction-refinement in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");
            
            return result;
        }
        
        template<typename ModelType>
        void PartialBisimulationMdpModelChecker<ModelType>::printBoundsInformation(std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> const& bounds) {
            STORM_LOG_THROW(bounds.first->isSymbolicQuantitativeCheckResult(), storm::exceptions::InvalidTypeException, "Expected symbolic quantitative check result.");
            storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType> const& lowerBounds = bounds.first->asSymbolicQuantitativeCheckResult<DdType, ValueType>();
            STORM_LOG_THROW(bounds.second->isSymbolicQuantitativeCheckResult(), storm::exceptions::InvalidTypeException, "Expected symbolic quantitative check result.");
            storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType> const& upperBounds = bounds.second->asSymbolicQuantitativeCheckResult<DdType, ValueType>();

            // If there is exactly one value that we stored, we print the current bounds as an interval.
            if (lowerBounds.getStates().getNonZeroCount() == 1 && upperBounds.getStates().getNonZeroCount() == 1) {
                STORM_LOG_TRACE("Obtained bounds [" << lowerBounds.getValueVector().getMax() << ", " << upperBounds.getValueVector().getMax() << "] on actual result.");
            } else {
                STORM_LOG_TRACE("Largest difference over initial states is " << getLargestDifference(bounds) << ".");
            }
        }
        
        template<typename ModelType>
        typename PartialBisimulationMdpModelChecker<ModelType>::ValueType PartialBisimulationMdpModelChecker<ModelType>::getLargestDifference(std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> const& bounds) {
            STORM_LOG_THROW(bounds.first->isSymbolicQuantitativeCheckResult(), storm::exceptions::InvalidTypeException, "Expected symbolic quantitative check result.");
            storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType> const& lowerBounds = bounds.first->asSymbolicQuantitativeCheckResult<DdType, ValueType>();
            STORM_LOG_THROW(bounds.second->isSymbolicQuantitativeCheckResult(), storm::exceptions::InvalidTypeException, "Expected symbolic quantitative check result.");
            storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType> const& upperBounds = bounds.second->asSymbolicQuantitativeCheckResult<DdType, ValueType>();

            return (upperBounds.getValueVector() - lowerBounds.getValueVector()).getMax();
        }

        template<typename ModelType>
        bool PartialBisimulationMdpModelChecker<ModelType>::checkBoundsSufficientlyClose(std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> const& bounds) {
            STORM_LOG_THROW(bounds.first->isSymbolicQuantitativeCheckResult(), storm::exceptions::InvalidTypeException, "Expected symbolic quantitative check result.");
            storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType> const& lowerBounds = bounds.first->asSymbolicQuantitativeCheckResult<DdType, ValueType>();
            STORM_LOG_THROW(bounds.second->isSymbolicQuantitativeCheckResult(), storm::exceptions::InvalidTypeException, "Expected symbolic quantitative check result.");
            storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType> const& upperBounds = bounds.second->asSymbolicQuantitativeCheckResult<DdType, ValueType>();

            return lowerBounds.getValueVector().equalModuloPrecision(upperBounds.getValueVector(), storm::settings::getModule<storm::settings::modules::AbstractionSettings>().getPrecision(), false);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::getAverageOfBounds(std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> const& bounds) {
            STORM_LOG_THROW(bounds.first->isSymbolicQuantitativeCheckResult(), storm::exceptions::InvalidTypeException, "Expected symbolic quantitative check result.");
            storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType> const& lowerBounds = bounds.first->asSymbolicQuantitativeCheckResult<DdType, ValueType>();
            STORM_LOG_THROW(bounds.second->isSymbolicQuantitativeCheckResult(), storm::exceptions::InvalidTypeException, "Expected symbolic quantitative check result.");
            storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType> const& upperBounds = bounds.second->asSymbolicQuantitativeCheckResult<DdType, ValueType>();
            
            return std::make_unique<storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType>>(lowerBounds.getReachableStates(), lowerBounds.getStates(), (lowerBounds.getValueVector() + upperBounds.getValueVector()) / lowerBounds.getValueVector().getDdManager().getConstant(storm::utility::convertNumber<ValueType>(std::string("2.0"))));
        }
        
        static int i = 0;
        
        template<typename ModelType>
        std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> PartialBisimulationMdpModelChecker<ModelType>::computeBoundsPartialQuotient(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, bool rewards, CheckTask<storm::logic::Formula> const& checkTask) {
            
            std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> result;
            
            CheckTask<storm::logic::Formula> newCheckTask(checkTask);
            SymbolicMdpPrctlModelChecker<storm::models::symbolic::Mdp<DdType, ValueType>> checker(quotient);

            newCheckTask.setOptimizationDirection(storm::OptimizationDirection::Minimize);
            if (rewards) {
                result.first = checker.computeRewards(storm::logic::RewardMeasureType::Expectation,newCheckTask);
            } else {
                result.first = checker.computeProbabilities(newCheckTask);
            }
            result.first->asSymbolicQuantitativeCheckResult<DdType, ValueType>().getValueVector().exportToDot("lower_values" + std::to_string(i) + ".dot");
            result.first->filter(storm::modelchecker::SymbolicQualitativeCheckResult<DdType>(quotient.getReachableStates(), quotient.getInitialStates()));
            
            newCheckTask.setOptimizationDirection(storm::OptimizationDirection::Maximize);
            if (rewards) {
                result.first = checker.computeRewards(storm::logic::RewardMeasureType::Expectation, newCheckTask);
            } else {
                result.second = checker.computeProbabilities(newCheckTask);
            }
            result.first->asSymbolicQuantitativeCheckResult<DdType, ValueType>().getValueVector().exportToDot("upper_values" + std::to_string(i++) + ".dot");
            result.second->filter(storm::modelchecker::SymbolicQualitativeCheckResult<DdType>(quotient.getReachableStates(), quotient.getInitialStates()));

            return result;
        }

        template<typename ModelType>
        std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> PartialBisimulationMdpModelChecker<ModelType>::computeBoundsPartialQuotient(storm::models::symbolic::StochasticTwoPlayerGame<DdType, ValueType> const& quotient, bool rewards, CheckTask<storm::logic::Formula> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Currently not implemented.");
        }

        template<typename ModelType>
        std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> PartialBisimulationMdpModelChecker<ModelType>::computeBoundsPartialQuotient(storm::models::Model<ValueType> const& quotient, bool rewards, CheckTask<storm::logic::Formula> const& checkTask) {
            
            // Sanity checks.
            STORM_LOG_THROW(quotient.isSymbolicModel(), storm::exceptions::NotSupportedException, "Expecting symbolic quotient.");
            storm::models::ModelType modelType = quotient.getType();
            STORM_LOG_THROW(modelType == storm::models::ModelType::Mdp || modelType == storm::models::ModelType::S2pg, storm::exceptions::NotSupportedException, "Only MDPs and stochastic games are supported as partial quotients.");
            
            if (modelType == storm::models::ModelType::Mdp) {
                return computeBoundsPartialQuotient(*quotient.template as<storm::models::symbolic::Mdp<DdType, ValueType>>(), rewards, checkTask);
            } else {
                return computeBoundsPartialQuotient(*quotient.template as<storm::models::symbolic::StochasticTwoPlayerGame<DdType, ValueType>>(), rewards, checkTask);
            }
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeResultFullQuotient(storm::models::symbolic::Dtmc<DdType, ValueType> const& quotient, bool rewards, CheckTask<storm::logic::Formula> const& checkTask) {
            SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<DdType, ValueType>> checker(quotient);
            std::unique_ptr<CheckResult> result;
            if (rewards) {
                result = checker.computeRewards(storm::logic::RewardMeasureType::Expectation, checkTask);
            } else {
                result = checker.computeProbabilities(checkTask);
            }
            result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<DdType>(quotient.getReachableStates(), quotient.getInitialStates()));
            return result;
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeResultFullQuotient(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, bool rewards, CheckTask<storm::logic::Formula> const& checkTask) {
            SymbolicMdpPrctlModelChecker<storm::models::symbolic::Mdp<DdType, ValueType>> checker(quotient);
            std::unique_ptr<CheckResult> result;
            if (rewards) {
                result = checker.computeRewards(storm::logic::RewardMeasureType::Expectation, checkTask);
            } else {
                result = checker.computeProbabilities(checkTask);
            }
            result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<DdType>(quotient.getReachableStates(), quotient.getInitialStates()));
            return result;
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeResultFullQuotient(storm::models::Model<ValueType> const& quotient, bool rewards, CheckTask<storm::logic::Formula> const& checkTask) {
            
            // Sanity checks.
            STORM_LOG_THROW(quotient.isSymbolicModel(), storm::exceptions::NotSupportedException, "Expecting symbolic quotient.");
            storm::models::ModelType modelType = quotient.getType();
            STORM_LOG_THROW(modelType == storm::models::ModelType::Dtmc || modelType == storm::models::ModelType::Mdp, storm::exceptions::NotSupportedException, "Only DTMCs and MDPs supported as full quotients.");
            
            if (modelType == storm::models::ModelType::Dtmc) {
                return computeResultFullQuotient(*quotient.template as<storm::models::symbolic::Dtmc<DdType, ValueType>>(), rewards, checkTask);
            } else {
                return computeResultFullQuotient(*quotient.template as<storm::models::symbolic::Mdp<DdType, ValueType>>(), rewards, checkTask);
            }
        }
        
        template class PartialBisimulationMdpModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>>;
        template class PartialBisimulationMdpModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double>>;
        template class PartialBisimulationMdpModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>>;
        template class PartialBisimulationMdpModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double>>;
    }
}
