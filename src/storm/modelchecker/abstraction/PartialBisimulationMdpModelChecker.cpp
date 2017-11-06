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
#include "storm/modelchecker/prctl/helper/SymbolicMdpPrctlHelper.h"

#include "storm/logic/FragmentSpecification.h"

#include "storm/storage/dd/Bdd.h"
#include "storm/storage/dd/BisimulationDecomposition.h"

#include "storm/abstraction/QualitativeMdpResultMinMax.h"
#include "storm/abstraction/QualitativeGameResultMinMax.h"
#include "storm/abstraction/QuantitativeGameResult.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/AbstractionSettings.h"

#include "storm/solver/SymbolicGameSolver.h"

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
                    
                    bool converged = false;
                    if (!bounds.first && !bounds.second) {
                        STORM_LOG_TRACE("Did not compute any bounds, skipping convergence check.");
                    } else {
                        // If either of the two bounds does not exist, the answer can be derived from the existing bounds.
                        if (bounds.first == nullptr || bounds.second == nullptr) {
                            STORM_LOG_ASSERT(bounds.first || bounds.second, "Expected at least one bound.");
                            STORM_LOG_TRACE("Obtained result on partial quotient.");
                            quotient->printModelInformationToStream(std::cout);
                            if (bounds.first) {
                                return std::move(bounds.first);
                            } else {
                                return std::move(bounds.second);
                            }
                        }
                    
                        // Check whether the bounds are sufficiently close.
                        converged = checkBoundsSufficientlyClose(bounds);
                        if (converged) {
                            result = getAverageOfBounds(bounds);
                        }
                    }
                        
                    if (!converged) {
                        STORM_LOG_TRACE("Performing bisimulation step.");
                        bisimulation.compute(1);
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
        bool PartialBisimulationMdpModelChecker<ModelType>::checkForResult(storm::models::Model<ValueType> const& quotient, bool lowerBounds, QuantitativeCheckResult<ValueType> const& result, CheckTask<storm::logic::Formula> const& checkTask) {
            storm::logic::ComparisonType comparisonType = checkTask.getBoundComparisonType();
            ValueType threshold = checkTask.getBoundThreshold();
            
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
        template<typename QuotientModelType>
        std::pair<storm::dd::Bdd<PartialBisimulationMdpModelChecker<ModelType>::DdType>, storm::dd::Bdd<PartialBisimulationMdpModelChecker<ModelType>::DdType>> PartialBisimulationMdpModelChecker<ModelType>::getConstraintAndTargetStates(QuotientModelType const& quotient, CheckTask<storm::logic::Formula> const& checkTask) {
            std::pair<storm::dd::Bdd<DdType>, storm::dd::Bdd<DdType>> result;
            
            SymbolicPropositionalModelChecker<QuotientModelType> checker(quotient);
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
        storm::abstraction::QualitativeMdpResultMinMax<PartialBisimulationMdpModelChecker<ModelType>::DdType> PartialBisimulationMdpModelChecker<ModelType>::computeQualitativeResult(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask, storm::dd::Bdd<DdType> const& constraintStates, storm::dd::Bdd<DdType> const& targetStates) {

            STORM_LOG_DEBUG("Computing qualitative solution for quotient MDP.");
            storm::abstraction::QualitativeMdpResultMinMax<DdType> result;
            
            auto start = std::chrono::high_resolution_clock::now();
            bool isRewardFormula = checkTask.getFormula().isEventuallyFormula() && checkTask.getFormula().asEventuallyFormula().getContext() == storm::logic::FormulaContext::Reward;
            storm::dd::Bdd<DdType> transitionMatrixBdd = quotient.getTransitionMatrix().notZero();
            if (isRewardFormula) {
                auto prob1 = storm::utility::graph::performProb1E(quotient, transitionMatrixBdd, constraintStates, targetStates, storm::utility::graph::performProbGreater0E(quotient, transitionMatrixBdd, constraintStates, targetStates));
                result.prob1Min = storm::abstraction::QualitativeMdpResult<DdType>(prob1);
                prob1 = storm::utility::graph::performProb1A(quotient, transitionMatrixBdd, targetStates, storm::utility::graph::performProbGreater0A(quotient, transitionMatrixBdd, constraintStates, targetStates));
                result.prob1Max = storm::abstraction::QualitativeMdpResult<DdType>(prob1);
            } else {
                auto prob01 = storm::utility::graph::performProb01Min(quotient, transitionMatrixBdd, constraintStates, targetStates);
                result.prob0Min = storm::abstraction::QualitativeMdpResult<DdType>(prob01.first);
                result.prob1Min = storm::abstraction::QualitativeMdpResult<DdType>(prob01.second);
                prob01 = storm::utility::graph::performProb01Max(quotient, transitionMatrixBdd, constraintStates, targetStates);
                result.prob0Max = storm::abstraction::QualitativeMdpResult<DdType>(prob01.first);
                result.prob1Max = storm::abstraction::QualitativeMdpResult<DdType>(prob01.second);
            }
            auto end = std::chrono::high_resolution_clock::now();

            auto timeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            STORM_LOG_DEBUG("Computed qualitative solution in " << timeInMilliseconds << "ms.");

            return result;
        }

        template<typename ModelType>
        storm::abstraction::QualitativeGameResultMinMax<PartialBisimulationMdpModelChecker<ModelType>::DdType> PartialBisimulationMdpModelChecker<ModelType>::computeQualitativeResult(storm::models::symbolic::StochasticTwoPlayerGame<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask, storm::dd::Bdd<DdType> const& constraintStates, storm::dd::Bdd<DdType> const& targetStates, storm::OptimizationDirection optimizationDirectionInModel) {
            
            STORM_LOG_DEBUG("Computing qualitative solution for quotient game.");
            storm::abstraction::QualitativeGameResultMinMax<DdType> result;
            
            auto start = std::chrono::high_resolution_clock::now();
            bool isRewardFormula = checkTask.getFormula().isEventuallyFormula() && checkTask.getFormula().asEventuallyFormula().getContext() == storm::logic::FormulaContext::Reward;
            storm::dd::Bdd<DdType> transitionMatrixBdd = quotient.getTransitionMatrix().notZero();
            if (isRewardFormula) {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Rewards are currently not supported for quotient stochastic games.");
            } else {
                result.prob0Min = storm::utility::graph::performProb0(quotient, quotient.getQualitativeTransitionMatrix(), constraintStates, targetStates, storm::OptimizationDirection::Minimize, optimizationDirectionInModel);
                result.prob1Min = storm::utility::graph::performProb1(quotient, quotient.getQualitativeTransitionMatrix(), constraintStates, targetStates, storm::OptimizationDirection::Minimize, optimizationDirectionInModel);
                result.prob0Max = storm::utility::graph::performProb0(quotient, quotient.getQualitativeTransitionMatrix(), constraintStates, targetStates, storm::OptimizationDirection::Maximize, optimizationDirectionInModel);
                result.prob1Max = storm::utility::graph::performProb1(quotient, quotient.getQualitativeTransitionMatrix(), constraintStates, targetStates, storm::OptimizationDirection::Maximize, optimizationDirectionInModel);
            }
            auto end = std::chrono::high_resolution_clock::now();
            
            auto timeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            STORM_LOG_DEBUG("Computed qualitative solution in " << timeInMilliseconds << "ms.");
            
            return result;
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::checkForResult(storm::models::symbolic::Model<DdType, ValueType> const& quotient, storm::abstraction::SymbolicQualitativeResultMinMax<DdType> const& qualitativeResults, CheckTask<storm::logic::Formula> const& checkTask) {
            std::unique_ptr<CheckResult> result;
            
            bool isRewardFormula = checkTask.getFormula().isEventuallyFormula() && checkTask.getFormula().asEventuallyFormula().getContext() == storm::logic::FormulaContext::Reward;
            if (isRewardFormula) {
                // In the reachability reward case, we can give an answer if all initial states of the system are infinity
                // states in the min result.
                if ((quotient.getInitialStates() && !qualitativeResults.getProb1Min().getStates()) == quotient.getInitialStates()) {
                    result = std::make_unique<storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType>>(quotient.getReachableStates(), quotient.getInitialStates(), quotient.getInitialStates().ite(quotient.getManager().getConstant(storm::utility::infinity<ValueType>()), quotient.getManager().template getAddZero<ValueType>()));
                }
            } else {
                // In the reachability probability case, we can give the answer if all initial states are prob1 states
                // in the min result or if all initial states are prob0 in the max case.
                // Furthermore, we can give the answer if there are initial states with probability > 0 in the min case
                // and the probability bound was 0 or if there are initial states with probability < 1 in the max case
                // and the probability bound was 1.
                if ((quotient.getInitialStates() && qualitativeResults.getProb1Min().getStates()) == quotient.getInitialStates()) {
                    result = std::make_unique<storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType>>(quotient.getReachableStates(), quotient.getInitialStates(), quotient.getManager().template getAddOne<ValueType>());
                } else if ((quotient.getInitialStates() && qualitativeResults.getProb0Max().getStates()) == quotient.getInitialStates()) {
                    result = std::make_unique<storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType>>(quotient.getReachableStates(), quotient.getInitialStates(), quotient.getManager().template getAddZero<ValueType>());
                } else if (checkTask.isBoundSet() && checkTask.getBoundThreshold() == storm::utility::zero<ValueType>() && (quotient.getInitialStates() && qualitativeResults.getProb0Min().getStates()) != quotient.getInitialStates()) {
                    result = std::make_unique<storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType>>(quotient.getReachableStates(), quotient.getInitialStates(), (quotient.getInitialStates() && qualitativeResults.getProb0Min().getStates()).ite(quotient.getManager().template getConstant<ValueType>(0.5), quotient.getManager().template getAddZero<ValueType>()));
                } else if (checkTask.isBoundSet() && checkTask.getBoundThreshold() == storm::utility::one<ValueType>() && (quotient.getInitialStates() && qualitativeResults.getProb1Max().getStates()) != quotient.getInitialStates()) {
                    result = std::make_unique<storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType>>(quotient.getReachableStates(), quotient.getInitialStates(), (quotient.getInitialStates() && qualitativeResults.getProb1Max().getStates()).ite(quotient.getManager().template getConstant<ValueType>(0.5), quotient.getManager().template getAddZero<ValueType>()) + qualitativeResults.getProb1Max().getStates().template toAdd<ValueType>());
                }
            }
            
            return result;
        }
        
        template<typename ModelType>
        bool PartialBisimulationMdpModelChecker<ModelType>::skipQuantitativeSolution(storm::models::symbolic::Model<DdType, ValueType> const& quotient, storm::abstraction::SymbolicQualitativeResultMinMax<DdType> const& qualitativeResults, CheckTask<storm::logic::Formula> const& checkTask) {
            
            bool isRewardFormula = checkTask.getFormula().isEventuallyFormula() && checkTask.getFormula().asEventuallyFormula().getContext() == storm::logic::FormulaContext::Reward;
            if (isRewardFormula) {
                if ((quotient.getInitialStates() && qualitativeResults.getProb1Min().getStates()) != (quotient.getInitialStates() && qualitativeResults.getProb1Max().getStates())) {
                    return true;
                }
            } else {
                if ((quotient.getInitialStates() && qualitativeResults.getProb0Min().getStates()) != (quotient.getInitialStates() && qualitativeResults.getProb0Max().getStates())) {
                    return true;
                } else if ((quotient.getInitialStates() && qualitativeResults.getProb1Min().getStates()) != (quotient.getInitialStates() && qualitativeResults.getProb1Max().getStates())) {
                    return true;
                }
            }
            return false;
        }
        
        template<typename ModelType>
        std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> PartialBisimulationMdpModelChecker<ModelType>::computeQuantitativeResult(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask, storm::dd::Bdd<DdType> const& constraintStates, storm::dd::Bdd<DdType> const& targetStates, storm::abstraction::SymbolicQualitativeResultMinMax<DdType> const& qualitativeResults) {

            std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> result;
            
            bool isRewardFormula = checkTask.getFormula().isEventuallyFormula() && checkTask.getFormula().asEventuallyFormula().getContext() == storm::logic::FormulaContext::Reward;
            if (isRewardFormula) {
                storm::dd::Bdd<DdType> maybeMin = qualitativeResults.getProb1Min().getStates() && quotient.getReachableStates();
                result.first = storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeReachabilityRewards(storm::OptimizationDirection::Minimize, quotient, quotient.getTransitionMatrix(), quotient.getTransitionMatrix().notZero(), checkTask.isRewardModelSet() ? quotient.getRewardModel(checkTask.getRewardModel()) : quotient.getRewardModel(""), maybeMin, targetStates, !qualitativeResults.getProb1Min().getStates() && quotient.getReachableStates(), storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType>(), quotient.getManager().template getAddZero<ValueType>());
                
                storm::dd::Bdd<DdType> maybeMax = qualitativeResults.getProb1Max().getStates() && quotient.getReachableStates();
                result.second = storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeReachabilityRewards(storm::OptimizationDirection::Maximize, quotient, quotient.getTransitionMatrix(), quotient.getTransitionMatrix().notZero(), checkTask.isRewardModelSet() ? quotient.getRewardModel(checkTask.getRewardModel()) : quotient.getRewardModel(""), maybeMin, targetStates, !qualitativeResults.getProb1Max().getStates() && quotient.getReachableStates(), storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType>(), maybeMax.ite(result.first->asSymbolicQuantitativeCheckResult<DdType, ValueType>().getValueVector(), quotient.getManager().template getAddZero<ValueType>()));
            } else {
                storm::dd::Bdd<DdType> maybeMin = !(qualitativeResults.getProb0Min().getStates() || qualitativeResults.getProb1Min().getStates()) && quotient.getReachableStates();
                result.first = storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeUntilProbabilities(storm::OptimizationDirection::Minimize, quotient, quotient.getTransitionMatrix(), maybeMin, qualitativeResults.getProb1Min().getStates(), storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType>(), quotient.getManager().template getAddZero<ValueType>());
                
                storm::dd::Bdd<DdType> maybeMax = !(qualitativeResults.getProb0Max().getStates() || qualitativeResults.getProb1Max().getStates()) && quotient.getReachableStates();
                result.second = storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeUntilProbabilities(storm::OptimizationDirection::Maximize, quotient, quotient.getTransitionMatrix(), maybeMax, qualitativeResults.getProb1Max().getStates(), storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType>(), maybeMax.ite(result.first->asSymbolicQuantitativeCheckResult<DdType, ValueType>().getValueVector(), quotient.getManager().template getAddZero<ValueType>()));
            }
            
            return result;
        }

        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> computeReachabilityProbabilitiesHelper(storm::models::symbolic::StochasticTwoPlayerGame<Type, ValueType> const& quotient, storm::OptimizationDirection const& player1Direction, storm::OptimizationDirection const& player2Direction,  storm::dd::Bdd<Type> const& maybeStates, storm::dd::Bdd<Type> const& prob1States) {
            
            STORM_LOG_TRACE("Performing quantative solution step. Player 1: " << player1Direction << ", player 2: " << player2Direction << ".");
            
            // Compute the ingredients of the equation system.
            storm::dd::Add<Type, ValueType> maybeStatesAdd = maybeStates.template toAdd<ValueType>();
            storm::dd::Add<Type, ValueType> submatrix = maybeStatesAdd * quotient.getTransitionMatrix();
            storm::dd::Add<Type, ValueType> prob1StatesAsColumn = prob1States.template toAdd<ValueType>().swapVariables(quotient.getRowColumnMetaVariablePairs());
            storm::dd::Add<Type, ValueType> subvector = submatrix * prob1StatesAsColumn;
            subvector = subvector.sumAbstract(quotient.getColumnVariables());
            
            // Cut away all columns targeting non-maybe states.
            submatrix *= maybeStatesAdd.swapVariables(quotient.getRowColumnMetaVariablePairs());
            
            // Initialize the starting vector.
            storm::dd::Add<Type, ValueType> startVector = quotient.getManager().template getAddZero<ValueType>();
            
            // Create the solver and solve the equation system.
            storm::solver::SymbolicGameSolverFactory<Type, ValueType> solverFactory;
            std::unique_ptr<storm::solver::SymbolicGameSolver<Type, ValueType>> solver = solverFactory.create(submatrix, maybeStates, quotient.getIllegalPlayer1Mask(), quotient.getIllegalPlayer2Mask(), quotient.getRowVariables(), quotient.getColumnVariables(), quotient.getRowColumnMetaVariablePairs(), quotient.getPlayer1Variables(), quotient.getPlayer2Variables());
            auto values = solver->solveGame(player1Direction, player2Direction, startVector, subvector);
            
            return std::make_unique<storm::modelchecker::SymbolicQuantitativeCheckResult<Type, ValueType>>(quotient.getReachableStates(), prob1States.template toAdd<ValueType>() + values);
        }
        
        template<typename ModelType>
        std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> PartialBisimulationMdpModelChecker<ModelType>::computeQuantitativeResult(storm::models::symbolic::StochasticTwoPlayerGame<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask, storm::dd::Bdd<DdType> const& constraintStates, storm::dd::Bdd<DdType> const& targetStates, storm::abstraction::SymbolicQualitativeResultMinMax<DdType> const& qualitativeResults) {
            
            std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> result;
            
            bool isRewardFormula = checkTask.getFormula().isEventuallyFormula() && checkTask.getFormula().asEventuallyFormula().getContext() == storm::logic::FormulaContext::Reward;
            if (isRewardFormula) {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Computing rewards for stochastic games is currently unsupported.");
            } else {
                storm::dd::Bdd<DdType> maybeMin = !(qualitativeResults.getProb0Min().getStates() || qualitativeResults.getProb1Min().getStates()) && quotient.getReachableStates();
                result.first = computeReachabilityProbabilitiesHelper(quotient, storm::OptimizationDirection::Minimize, checkTask.getOptimizationDirection(), maybeMin, qualitativeResults.getProb1Min().getStates());
                
                storm::dd::Bdd<DdType> maybeMax = !(qualitativeResults.getProb0Max().getStates() || qualitativeResults.getProb1Max().getStates()) && quotient.getReachableStates();
                result.second = computeReachabilityProbabilitiesHelper(quotient, storm::OptimizationDirection::Maximize, checkTask.getOptimizationDirection(), maybeMin, qualitativeResults.getProb1Max().getStates());
            }
            
            return result;
        }

        template<typename ModelType>
        std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> PartialBisimulationMdpModelChecker<ModelType>::computeBoundsPartialQuotient(storm::models::symbolic::Mdp<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask) {
            std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> result;

            // We go through two phases. In phase (1) we are solving the qualitative part and in phase (2) the quantitative part.
            
            // Preparation: determine the constraint states and the target states of the reachability objective.
            std::pair<storm::dd::Bdd<DdType>, storm::dd::Bdd<DdType>> constraintTargetStates = getConstraintAndTargetStates(quotient, checkTask);
            
            // Phase (1): solve qualitatively.
            storm::abstraction::QualitativeMdpResultMinMax<DdType> qualitativeResults = computeQualitativeResult(quotient, checkTask, constraintTargetStates.first, constraintTargetStates.second);

            // Check whether the answer can be given after the qualitative solution.
            result.first = checkForResult(quotient, qualitativeResults, checkTask);
            if (result.first) {
                return result;
            }
            
            // Check whether we should skip the quantitative solution (for example if there are initial states for which
            // the value is already known to be different at this point.
            bool doSkipQuantitativeSolution = skipQuantitativeSolution(quotient, qualitativeResults, checkTask);
            STORM_LOG_TRACE("" << (doSkipQuantitativeSolution ? "Skipping" : "Not skipping") << " quantitative solution.");
            
            // Phase (2): solve quantitatively.
            if (!doSkipQuantitativeSolution) {
                result = computeQuantitativeResult(quotient, checkTask, constraintTargetStates.first, constraintTargetStates.second, qualitativeResults);

                storm::modelchecker::SymbolicQualitativeCheckResult<DdType> initialStateFilter(quotient.getReachableStates(), quotient.getInitialStates());
                result.first->filter(initialStateFilter);
                result.second->filter(initialStateFilter);
                printBoundsInformation(result);
                
                // Check whether the answer can be given after the quantitative solution.
                if (checkForResult(quotient, true, result.first->asQuantitativeCheckResult<ValueType>(), checkTask)) {
                    result.second = nullptr;
                }
                if (checkForResult(quotient, false, result.second->asQuantitativeCheckResult<ValueType>(), checkTask)) {
                    result.first = nullptr;
                }
            }
            return result;
        }

        template<typename ModelType>
        std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> PartialBisimulationMdpModelChecker<ModelType>::computeBoundsPartialQuotient(storm::models::symbolic::StochasticTwoPlayerGame<DdType, ValueType> const& quotient, CheckTask<storm::logic::Formula> const& checkTask) {
            std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> result;
            
            // We go through two phases. In phase (1) we are solving the qualitative part and in phase (2) the quantitative part.
            
            // Preparation: determine the constraint states and the target states of the reachability objective.
            std::pair<storm::dd::Bdd<DdType>, storm::dd::Bdd<DdType>> constraintTargetStates = getConstraintAndTargetStates(quotient, checkTask);
            
            // Phase (1): solve qualitatively.
            storm::abstraction::QualitativeGameResultMinMax<DdType> qualitativeResults = computeQualitativeResult(quotient, checkTask, constraintTargetStates.first, constraintTargetStates.second, checkTask.getOptimizationDirection());
            
            // Check whether the answer can be given after the qualitative solution.
            result.first = checkForResult(quotient, qualitativeResults, checkTask);
            if (result.first) {
                return result;
            }

            // Check whether we should skip the quantitative solution (for example if there are initial states for which
            // the value is already known to be different at this point.
            bool doSkipQuantitativeSolution = skipQuantitativeSolution(quotient, qualitativeResults, checkTask);
            STORM_LOG_TRACE("" << (doSkipQuantitativeSolution ? "Skipping" : "Not skipping") << " quantitative solution.");

            // Phase (2): solve quantitatively.
            if (!doSkipQuantitativeSolution) {
                result = computeQuantitativeResult(quotient, checkTask, constraintTargetStates.first, constraintTargetStates.second, qualitativeResults);

                storm::modelchecker::SymbolicQualitativeCheckResult<DdType> initialStateFilter(quotient.getReachableStates(), quotient.getInitialStates());
                result.first->filter(initialStateFilter);
                result.second->filter(initialStateFilter);
                printBoundsInformation(result);
                
                // Check whether the answer can be given after the quantitative solution.
                if (checkForResult(quotient, true, result.first->asQuantitativeCheckResult<ValueType>(), checkTask)) {
                    result.second = nullptr;
                } else if (checkForResult(quotient, false, result.second->asQuantitativeCheckResult<ValueType>(), checkTask)) {
                    result.first = nullptr;
                }
            }
            return result;
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
