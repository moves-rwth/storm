#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"

#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/logic/FragmentSpecification.h"

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
        bool SparseMdpPrctlModelChecker<SparseMdpModelType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            return formula.isInFragment(storm::logic::prctl().setLongRunAverageRewardFormulasAllowed(false).setLongRunAverageProbabilitiesAllowed(true).setConditionalProbabilityFormulasAllowed(true).setOnlyEventuallyFormuluasInConditionalFormulasAllowed(true));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeBoundedUntilProbabilities(checkTask.getOptimizationDirection(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getDiscreteTimeBound(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeNextProbabilities(CheckTask<storm::logic::NextFormula> const& checkTask) {
            storm::logic::NextFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeNextProbabilities(checkTask.getOptimizationDirection(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            auto ret = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeUntilProbabilities(checkTask.getOptimizationDirection(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet(), checkTask.isProduceSchedulersSet(), *minMaxLinearEquationSolverFactory);
            std::unique_ptr<CheckResult> result(new ExplicitQuantitativeCheckResult<ValueType>(std::move(ret.values)));
            if (checkTask.isProduceSchedulersSet() && ret.scheduler) {
                result->asExplicitQuantitativeCheckResult<ValueType>().setScheduler(std::move(ret.scheduler));
            }
            return result;
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeGloballyProbabilities(CheckTask<storm::logic::GloballyFormula> const& checkTask) {
            storm::logic::GloballyFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            auto ret = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeGloballyProbabilities(checkTask.getOptimizationDirection(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(ret)));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeConditionalProbabilities(CheckTask<storm::logic::ConditionalFormula> const& checkTask) {
            storm::logic::ConditionalFormula const& conditionalFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(this->getModel().getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::InvalidPropertyException, "Cannot compute conditional probabilities on MDPs with more than one initial state.");
            STORM_LOG_THROW(conditionalFormula.getSubformula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException, "Illegal conditional probability formula.");
            STORM_LOG_THROW(conditionalFormula.getConditionFormula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException, "Illegal conditional probability formula.");

            std::unique_ptr<CheckResult> leftResultPointer = this->check(conditionalFormula.getSubformula().asEventuallyFormula().getSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(conditionalFormula.getConditionFormula().asEventuallyFormula().getSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();

            return storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeConditionalProbabilities(checkTask.getOptimizationDirection(), *this->getModel().getInitialStates().begin(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *minMaxLinearEquationSolverFactory);
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeCumulativeRewards(CheckTask<storm::logic::CumulativeRewardFormula> const& checkTask) {
            storm::logic::CumulativeRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidArgumentException, "Formula needs to have a discrete time bound.");
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeCumulativeRewards(checkTask.getOptimizationDirection(), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getDiscreteTimeBound(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeInstantaneousRewards(CheckTask<storm::logic::InstantaneousRewardFormula> const& checkTask) {
            storm::logic::InstantaneousRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidArgumentException, "Formula needs to have a discrete time bound.");
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeInstantaneousRewards(checkTask.getOptimizationDirection(), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getDiscreteTimeBound(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
                
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeReachabilityRewards(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(eventuallyFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeReachabilityRewards(checkTask.getOptimizationDirection(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }

		template<typename SparseMdpModelType>
		std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula> const& checkTask) {
            storm::logic::StateFormula const& stateFormula = checkTask.getFormula();
			STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
			std::unique_ptr<CheckResult> subResultPointer = this->check(stateFormula);
			ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeLongRunAverageProbabilities(checkTask.getOptimizationDirection(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(),  subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
		}
                
        template class SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>>;
    }
}