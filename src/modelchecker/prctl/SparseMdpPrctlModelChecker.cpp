#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"

#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/models/sparse/StandardRewardModel.h"

#include "src/modelchecker/prctl/helper/SparseMdpPrctlHelper.h"

#include "src/solver/LpSolver.h"

#include "src/settings/modules/GeneralSettings.h"

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidPropertyException.h"
#include "src/storage/expressions/Expressions.h"

#include "src/storage/MaximalEndComponentDecomposition.h"

#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace modelchecker {
        template<typename SparseMdpModelType>
        SparseMdpPrctlModelChecker<SparseMdpModelType>::SparseMdpPrctlModelChecker(SparseMdpModelType const& model) : SparsePropositionalModelChecker<SparseMdpModelType>(model), minMaxLinearEquationSolverFactory(new storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType>()) {
            // Intentionally left empty.
        }
        
        template<typename SparseMdpModelType>
        SparseMdpPrctlModelChecker<SparseMdpModelType>::SparseMdpPrctlModelChecker(SparseMdpModelType const& model, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType>>&& minMaxLinearEquationSolverFactory) : SparsePropositionalModelChecker<SparseMdpModelType>(model), minMaxLinearEquationSolverFactory(std::move(minMaxLinearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename SparseMdpModelType>
        bool SparseMdpPrctlModelChecker<SparseMdpModelType>::canHandle(storm::logic::Formula const& formula) const {
            if (formula.isPctlStateFormula() || formula.isPctlPathFormula() || formula.isRewardPathFormula()) {
                return true;
            }
            if (formula.isProbabilityOperatorFormula()) {
                return this->canHandle(formula.asProbabilityOperatorFormula().getSubformula());
            }
            if (formula.isGloballyFormula()) {
                return true;
            }
            if (formula.isConditionalPathFormula()) {
                storm::logic::ConditionalPathFormula const& conditionalPathFormula = formula.asConditionalPathFormula();
                if (conditionalPathFormula.getLeftSubformula().isEventuallyFormula() && conditionalPathFormula.getRightSubformula().isEventuallyFormula()) {
                    return this->canHandle(conditionalPathFormula.getLeftSubformula()) && this->canHandle(conditionalPathFormula.getRightSubformula());
                }
            }
            return false;
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeBoundedUntilProbabilities(optimalityType.get(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getDiscreteTimeBound(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeNextProbabilities(storm::logic::NextFormula const& pathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeNextProbabilities(optimalityType.get(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            auto ret = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeUntilProbabilities(optimalityType.get(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative, false, *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(ret.result)));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeGloballyProbabilities(storm::logic::GloballyFormula const& pathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            auto ret = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeGloballyProbabilities(optimalityType.get(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), subResult.getTruthValuesVector(), qualitative, *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(ret)));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeConditionalProbabilities(storm::logic::ConditionalPathFormula const& pathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(this->getModel().getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::InvalidPropertyException, "Cannot compute conditional probabilities on MDPs with more than one initial state.");
            STORM_LOG_THROW(pathFormula.getLeftSubformula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException, "Illegal conditional probability formula.");
            STORM_LOG_THROW(pathFormula.getRightSubformula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException, "Illegal conditional probability formula.");

            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula().asEventuallyFormula().getSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula().asEventuallyFormula().getSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();

            return storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeConditionalProbabilities(optimalityType.get(), *this->getModel().getInitialStates().begin(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative, *minMaxLinearEquationSolverFactory);
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeUntilProbabilitiesForInitialStates(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<OptimizationDirection>  const& optimalityType, boost::optional<storm::logic::BoundInfo<ValueType>> const& bound) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            if(qualitative | !bound) {
                // For qualitative checks, or if the , we use the standard approach.
                 auto ret = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeUntilProbabilities(optimalityType.get(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative, false, *minMaxLinearEquationSolverFactory);
                return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(ret.result)));
            } else {
                // With a given bound, we only iterate until we pass the bound.
                storm::solver::BoundedGoal<ValueType> boundedGoal(optimalityType.get(), bound.get(), this->getModel().getInitialStates() );
                auto ret = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeUntilProbabilities(boundedGoal, this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative,  false, *minMaxLinearEquationSolverFactory);
                return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(ret.result)));
            }
           
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidArgumentException, "Formula needs to have a discrete time bound.");
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeCumulativeRewards(optimalityType.get(), this->getModel().getTransitionMatrix(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), rewardPathFormula.getDiscreteTimeBound(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidArgumentException, "Formula needs to have a discrete time bound.");
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeInstantaneousRewards(optimalityType.get(), this->getModel().getTransitionMatrix(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), rewardPathFormula.getDiscreteTimeBound(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
                
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(rewardPathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeReachabilityRewards(optimalityType.get(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), qualitative, *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }

		template<typename SparseMdpModelType>
		std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeLongRunAverageProbabilities(storm::logic::StateFormula const& stateFormula, CheckSettings<double> const& checkSettings) {
			STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
			std::unique_ptr<CheckResult> subResultPointer = this->check(stateFormula);
			ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeLongRunAverageProbabilities(optimalityType.get(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(),  subResult.getTruthValuesVector(), qualitative, *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
		}
                
        template class SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>>;
    }
}