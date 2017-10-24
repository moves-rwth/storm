#include "storm/modelchecker/abstraction/PartialBisimulationMdpModelChecker.h"

#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/Mdp.h"
#include "storm/models/symbolic/StochasticTwoPlayerGame.h"

#include "storm/modelchecker/results/CheckResult.h"
#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "storm/modelchecker/results/QuantitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "storm/modelchecker/prctl/SymbolicDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SymbolicMdpPrctlModelChecker.h"

#include "storm/logic/FragmentSpecification.h"

#include "storm/storage/dd/Bdd.h"
#include "storm/storage/dd/BisimulationDecomposition.h"

#include "storm/abstraction/QualitativeMdpResultMinMax.h"

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
            return computeValuesAbstractionRefinement(checkTask.substituteFormula<storm::logic::Formula>(checkTask.getFormula()));
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            return computeValuesAbstractionRefinement(checkTask.substituteFormula<storm::logic::Formula>(checkTask.getFormula()));
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeReachabilityRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(rewardMeasureType == storm::logic::RewardMeasureType::Expectation, storm::exceptions::InvalidPropertyException, "Can only compute reward expectations.");
            return computeValuesAbstractionRefinement(checkTask.template substituteFormula<storm::logic::Formula>(checkTask.getFormula()));
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeValuesAbstractionRefinement(CheckTask<storm::logic::Formula> const& checkTask) {

            STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::InvalidPropertyException, "The game-based abstraction refinement model checker can only compute the result for the initial states.");
            
            // Create the appropriate preservation information.
            storm::dd::bisimulation::PreservationInformation<DdType, ValueType> preservationInformation(model, {checkTask.getFormula().asSharedPointer()});
            if (checkTask.getFormula().isEventuallyFormula() && checkTask.getFormula().asEventuallyFormula().getContext() == storm::logic::FormulaContext::Reward) {
                if (!checkTask.isRewardModelSet() || model.hasUniqueRewardModel()) {
                    preservationInformation.addRewardModel(model.getUniqueRewardModelName());
                } else if (checkTask.isRewardModelSet()) {
                    preservationInformation.addRewardModel(checkTask.getRewardModel());
                }
            }
            
            // Create a bisimulation object that is used to obtain (partial) quotients.
            storm::dd::BisimulationDecomposition<DdType, ValueType> bisimulation(this->model, storm::storage::BisimulationType::Strong, preservationInformation);
            
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
                    result = computeResultFullQuotient(*quotient, checkTask);
                } else {
                    // Obtain lower and upper bounds from the partial quotient.
                    std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> bounds = computeBoundsPartialQuotient(*quotient, checkTask);
                    
                    // If either of the two bounds does not exist, the answer can be derived from the existing bounds.
                    if (bounds.first == nullptr || bounds.second == nullptr) {
                        STORM_LOG_ASSERT(bounds.first || bounds.second, "Expected at least one bound.");
                        quotient->printModelInformationToStream(std::cout);
                        if (bounds.first) {
                            return std::move(bounds.first);
                        } else {
                            return std::move(bounds.second);
                        }
                    }
                    
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
                storm::dd::Add<DdType, ValueType> diffs = upperBounds.getValueVector() - lowerBounds.getValueVector();
                storm::dd::Bdd<DdType> maxDiffRepresentative = diffs.maxAbstractRepresentative(diffs.getContainedMetaVariables());
                
                std::pair<ValueType, ValueType> bounds;
                bounds.first = (lowerBounds.getValueVector() * maxDiffRepresentative.template toAdd<ValueType>()).getMax();
                bounds.second = (upperBounds.getValueVector() * maxDiffRepresentative.template toAdd<ValueType>()).getMax();

                STORM_LOG_TRACE("Largest interval over initial is [" << bounds.first << ", " << bounds.second << "], difference " << (bounds.second - bounds.first) << ".");
            }
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

        template<typename ModelType>
        typename PartialBisimulationMdpModelChecker<ModelType>::ValueType PartialBisimulationMdpModelChecker<ModelType>::getExtremalBound(storm::OptimizationDirection dir, QuantitativeCheckResult<ValueType> const& result) {
            if (dir == storm::OptimizationDirection::Minimize) {
                return result.getMin();
            } else {
                return result.getMax();
            }
        }
        
        template<typename ModelType>
        bool PartialBisimulationMdpModelChecker<ModelType>::boundsSufficient(storm::models::Model<ValueType> const& quotient, bool lowerBounds, QuantitativeCheckResult<ValueType> const& result, storm::logic::ComparisonType comparisonType, ValueType const& threshold) {
            if (lowerBounds) {
                if (storm::logic::isLowerBound(comparisonType)) {
                    ValueType minimalLowerBound = getExtremalBound(storm::OptimizationDirection::Minimize, result);
                    return (storm::logic::isStrict(comparisonType) && minimalLowerBound > threshold) || (!storm::logic::isStrict(comparisonType) && minimalLowerBound >= threshold);
                } else {
                    ValueType maximalLowerBound = getExtremalBound(storm::OptimizationDirection::Maximize, result);
                    return (storm::logic::isStrict(comparisonType) && maximalLowerBound >= threshold) || (!storm::logic::isStrict(comparisonType) && maximalLowerBound > threshold);
                }
            } else {
                if (storm::logic::isLowerBound(comparisonType)) {
                    ValueType minimalUpperBound = getExtremalBound(storm::OptimizationDirection::Minimize, result);
                    return (storm::logic::isStrict(comparisonType) && minimalUpperBound <= threshold) || (!storm::logic::isStrict(comparisonType) && minimalUpperBound < threshold);
                } else {
                    ValueType maximalUpperBound = getExtremalBound(storm::OptimizationDirection::Maximize, result);
                    return (storm::logic::isStrict(comparisonType) && maximalUpperBound < threshold) || (!storm::logic::isStrict(comparisonType) && maximalUpperBound <= threshold);
                }
            }
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeBoundsPartialQuotient(SymbolicMdpPrctlModelChecker<storm::models::symbolic::Mdp<DdType, ValueType>>& checker, storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, storm::OptimizationDirection const& dir, CheckTask<storm::logic::Formula>& checkTask) {
            
            bool rewards = checkTask.getFormula().isEventuallyFormula() && checkTask.getFormula().asEventuallyFormula().getContext() == storm::logic::FormulaContext::Reward;

            std::unique_ptr<CheckResult> result;
            checkTask.setOptimizationDirection(dir);
            if (rewards) {
                result = checker.computeRewards(storm::logic::RewardMeasureType::Expectation, checkTask);
            } else {
                result = checker.computeProbabilities(checkTask);
            }
            STORM_LOG_ASSERT(result, "Expected result.");
            result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<DdType>(quotient.getReachableStates(), quotient.getInitialStates()));
            return result;
        }
        
        template<typename ModelType>
        std::pair<storm::dd::Bdd<PartialBisimulationMdpModelChecker<ModelType>::DdType>, storm::dd::Bdd<PartialBisimulationMdpModelChecker<ModelType>::DdType>> PartialBisimulationMdpModelChecker<ModelType>::getConstraintAndTargetStates(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask) {
            std::pair<storm::dd::Bdd<DdType>, storm::dd::Bdd<DdType>> result;
            
            SymbolicPropositionalModelChecker<storm::models::symbolic::Mdp<DdType, ValueType>> checker(quotient);
            if (checkTask.getFormula().isUntilFormula()) {
                std::unique_ptr<CheckResult> subresult = checker.check(checkTask.getFormula().asUntilFormula().getLeftSubformula());
                result.first = subresult->asSymbolicQualitativeCheckResult<DdType>().getTruthValuesVector();
                subresult = checker.check(checkTask.getFormula().asUntilFormula().getRightSubformula());
                result.second = subresult->asSymbolicQualitativeCheckResult<DdType>().getTruthValuesVector();
            } else if (checkTask.getFormula().isEventuallyFormula()) {
                storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula().asEventuallyFormula();
                result.first = quotient.getReachableStates();
                std::unique_ptr<CheckResult> subresult = checker.check(eventuallyFormula.getSubformula());
                result.second = subresult->asSymbolicQualitativeCheckResult<DdType>().getTruthValuesVector();
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The given formula is not supported by this model checker.");
            }
            
            return result;
        }
        
        template<typename ModelType>
        std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> PartialBisimulationMdpModelChecker<ModelType>::computeBoundsPartialQuotient(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask) {

            std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> result;

            // We go through two phases. In phase (1) we are solving the qualitative part and in phase (2) the quantitative part.
            
            // Preparation: determine the constraint states and the target states of the reachability objective.
            bool isRewardFormula = checkTask.getFormula().isEventuallyFormula() && checkTask.getFormula().asEventuallyFormula().getContext() == storm::logic::FormulaContext::Reward;
            std::pair<storm::dd::Bdd<DdType>, storm::dd::Bdd<DdType>> constraintTargetStates = getConstraintAndTargetStates(quotient, checkTask);
            
            // Phase (1): solve qualitatively.
            storm::abstraction::QualitativeMdpResultMinMax<DdType> qualitativeResults;
            storm::dd::Bdd<DdType> transitionMatrixBdd = quotient.getTransitionMatrix().notZero();
            if (isRewardFormula) {
                qualitativeResults.min.second = storm::utility::graph::performProb1E(quotient, transitionMatrixBdd, constraintTargetStates.first, constraintTargetStates.second, storm::utility::graph::performProbGreater0E(quotient, transitionMatrixBdd, constraintTargetStates.first, constraintTargetStates.second));
                qualitativeResults.max.second = storm::utility::graph::performProb1A(quotient, transitionMatrixBdd, constraintTargetStates.first, storm::utility::graph::performProbGreater0A(quotient, transitionMatrixBdd, constraintTargetStates.first, constraintTargetStates.second));
            } else {
                qualitativeResults.min = storm::utility::graph::performProb01Min(quotient, transitionMatrixBdd, constraintTargetStates.first, constraintTargetStates.second);
                qualitativeResults.max = storm::utility::graph::performProb01Max(quotient, transitionMatrixBdd, constraintTargetStates.first, constraintTargetStates.second);
            }
            
            
//            CheckTask<storm::logic::Formula> newCheckTask(checkTask);
//
//            if (!checkTask.isBoundSet() || storm::logic::isLowerBound(checkTask.getBoundComparisonType())) {
//                // Check whether we can answer the query (lower bound above bound).
//                result.first = computeBoundsPartialQuotient(checker, quotient, rewards, storm::OptimizationDirection::Minimize, newCheckTask);
//                if (checkTask.isBoundSet()) {
//                    bool sufficient = boundsSufficient(quotient, true, result.first->asQuantitativeCheckResult<ValueType>(), checkTask.getBoundComparisonType(), checkTask.getBoundThreshold());
//                    if (sufficient) {
//                        return result;
//                    }
//                }
//
//                // Check whether we can answer the query (upper bound below bound).
//                result.second = computeBoundsPartialQuotient(checker, quotient, rewards, storm::OptimizationDirection::Maximize, newCheckTask);
//                if (checkTask.isBoundSet()) {
//                    bool sufficient = boundsSufficient(quotient, false, result.second->asQuantitativeCheckResult<ValueType>(), checkTask.getBoundComparisonType(), checkTask.getBoundThreshold());
//                    if (sufficient) {
//                        result.first = nullptr;
//                        return result;
//                    }
//                }
//            } else {
//                // Check whether we can answer the query (upper bound below bound).
//                result.second = computeBoundsPartialQuotient(checker, quotient, rewards, storm::OptimizationDirection::Maximize, newCheckTask);
//                if (checkTask.isBoundSet()) {
//                    bool sufficient = boundsSufficient(quotient, false, result.second->asQuantitativeCheckResult<ValueType>(), checkTask.getBoundComparisonType(), checkTask.getBoundThreshold());
//                    if (sufficient) {
//                        return result;
//                    }
//                }
//
//                // Check whether we can answer the query (lower bound above bound).
//                result.first = computeBoundsPartialQuotient(checker, quotient, rewards, storm::OptimizationDirection::Minimize, newCheckTask);
//                if (checkTask.isBoundSet()) {
//                    bool sufficient = boundsSufficient(quotient, true, result.first->asQuantitativeCheckResult<ValueType>(), checkTask.getBoundComparisonType(), checkTask.getBoundThreshold());
//                    if (sufficient) {
//                        result.second = nullptr;
//                        return result;
//                    }
//                }
//            }
            
            return result;
        }

        template<typename ModelType>
        std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> PartialBisimulationMdpModelChecker<ModelType>::computeBoundsPartialQuotient(storm::models::symbolic::StochasticTwoPlayerGame<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Currently not implemented.");
        }

        template<typename ModelType>
        std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> PartialBisimulationMdpModelChecker<ModelType>::computeBoundsPartialQuotient(storm::models::Model<ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask) {
            
            // Sanity checks.
            STORM_LOG_THROW(quotient.isSymbolicModel(), storm::exceptions::NotSupportedException, "Expecting symbolic quotient.");
            storm::models::ModelType modelType = quotient.getType();
            STORM_LOG_THROW(modelType == storm::models::ModelType::Mdp || modelType == storm::models::ModelType::S2pg, storm::exceptions::NotSupportedException, "Only MDPs and stochastic games are supported as partial quotients.");
            
            if (modelType == storm::models::ModelType::Mdp) {
                return computeBoundsPartialQuotient(static_cast<storm::models::symbolic::Mdp<DdType, ValueType> const&>(quotient), checkTask);
            } else {
                return computeBoundsPartialQuotient(static_cast<storm::models::symbolic::StochasticTwoPlayerGame<DdType, ValueType> const&>(quotient), checkTask);
            }
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeResultFullQuotient(storm::models::symbolic::Dtmc<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask) {
            bool rewards = checkTask.getFormula().isEventuallyFormula() && checkTask.getFormula().asEventuallyFormula().getContext() == storm::logic::FormulaContext::Reward;
            
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
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeResultFullQuotient(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask) {
            bool rewards = checkTask.getFormula().isEventuallyFormula() && checkTask.getFormula().asEventuallyFormula().getContext() == storm::logic::FormulaContext::Reward;

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
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeResultFullQuotient(storm::models::Model<ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask) {
            
            // Sanity checks.
            STORM_LOG_THROW(quotient.isSymbolicModel(), storm::exceptions::NotSupportedException, "Expecting symbolic quotient.");
            storm::models::ModelType modelType = quotient.getType();
            STORM_LOG_THROW(modelType == storm::models::ModelType::Dtmc || modelType == storm::models::ModelType::Mdp, storm::exceptions::NotSupportedException, "Only DTMCs and MDPs supported as full quotients.");
            
            if (modelType == storm::models::ModelType::Dtmc) {
                return computeResultFullQuotient(static_cast<storm::models::symbolic::Dtmc<DdType, ValueType> const&>(quotient), checkTask);
            } else {
                return computeResultFullQuotient(static_cast<storm::models::symbolic::Mdp<DdType, ValueType> const&>(quotient), checkTask);
            }
        }
        
        template class PartialBisimulationMdpModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>>;
        template class PartialBisimulationMdpModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double>>;
        template class PartialBisimulationMdpModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>>;
        template class PartialBisimulationMdpModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double>>;
    }
}
