#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"

#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "storm/logic/FragmentSpecification.h"

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/modelchecker/prctl/helper/SparseMdpPrctlHelper.h"

#include "storm/modelchecker/multiobjective/multiObjectiveModelChecking.h"

#include "storm/solver/SolveGoal.h"

#include "storm/settings/modules/GeneralSettings.h"

#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/storage/expressions/Expressions.h"

#include "storm/storage/MaximalEndComponentDecomposition.h"

#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        template<typename SparseMdpModelType>
        SparseMdpPrctlModelChecker<SparseMdpModelType>::SparseMdpPrctlModelChecker(SparseMdpModelType const& model) : SparsePropositionalModelChecker<SparseMdpModelType>(model), minMaxLinearEquationSolverFactory(std::make_unique<storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType>>()) {
            // Intentionally left empty.
        }
        
        template<typename SparseMdpModelType>
        SparseMdpPrctlModelChecker<SparseMdpModelType>::SparseMdpPrctlModelChecker(SparseMdpModelType const& model, std::unique_ptr<storm::solver::MinMaxLinearEquationSolverFactory<ValueType>>&& minMaxLinearEquationSolverFactory) : SparsePropositionalModelChecker<SparseMdpModelType>(model), minMaxLinearEquationSolverFactory(std::move(minMaxLinearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename SparseMdpModelType>
        bool SparseMdpPrctlModelChecker<SparseMdpModelType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            if(formula.isInFragment(storm::logic::prctl().setLongRunAverageRewardFormulasAllowed(true).setLongRunAverageProbabilitiesAllowed(true).setConditionalProbabilityFormulasAllowed(true).setOnlyEventuallyFormuluasInConditionalFormulasAllowed(true).setRewardBoundedUntilFormulasAllowed(true).setMultiDimensionalBoundedUntilFormulasAllowed(true))) {
                return true;
            } else {
                // Check whether we consider a multi-objective formula
                // For multi-objective model checking, each initial state requires an individual scheduler (in contrast to single-objective model checking). Let's exclude multiple initial states.
                if (this->getModel().getInitialStates().getNumberOfSetBits() > 1) return false;
                if (!checkTask.isOnlyInitialStatesRelevantSet()) return false;
                return formula.isInFragment(storm::logic::multiObjective().setCumulativeRewardFormulasAllowed(true).setRewardBoundedUntilFormulasAllowed(true).setMultiDimensionalBoundedUntilFormulasAllowed(true));
            }
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            if (pathFormula.isMultiDimensional() || pathFormula.getTimeBoundReference().isRewardBound()) {
                STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::InvalidOperationException, "Checking non-trivial bounded until probabilities can only be computed for the initial states of the model.");
                STORM_LOG_WARN_COND(!checkTask.isQualitativeSet(), "Checking non-trivial bounded until formulas is not optimized w.r.t. qualitative queries");
                storm::logic::OperatorInformation opInfo(checkTask.getOptimizationDirection());
                if (checkTask.isBoundSet()) {
                    opInfo.bound = checkTask.getBound();
                }
                auto formula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(checkTask.getFormula().asSharedPointer(), opInfo);
                storm::modelchecker::multiobjective::MultiDimensionalRewardUnfolding<ValueType, true> rewardUnfolding(this->getModel(), formula);
                auto numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeNonTrivialBoundedUntilProbabilities(checkTask.getOptimizationDirection(), rewardUnfolding, this->getModel().getInitialStates(), *minMaxLinearEquationSolverFactory);
                return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
            } else {
                STORM_LOG_THROW(!pathFormula.hasLowerBound() && pathFormula.hasUpperBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have single upper time bound.");
                STORM_LOG_THROW(pathFormula.hasIntegerUpperBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have discrete upper time bound.");
                std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
                std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
                ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
                ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
                std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeStepBoundedUntilProbabilities(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getNonStrictUpperBound<uint64_t>(), *minMaxLinearEquationSolverFactory, checkTask.getHint());
                return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
            }
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeNextProbabilities(CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) {
            storm::logic::NextFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeNextProbabilities(checkTask.getOptimizationDirection(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            auto ret = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeUntilProbabilities(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet(), checkTask.isProduceSchedulersSet(), *minMaxLinearEquationSolverFactory, checkTask.getHint());
            std::unique_ptr<CheckResult> result(new ExplicitQuantitativeCheckResult<ValueType>(std::move(ret.values)));
            if (checkTask.isProduceSchedulersSet() && ret.scheduler) {
                result->asExplicitQuantitativeCheckResult<ValueType>().setScheduler(std::move(ret.scheduler));
            }
            return result;
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeGloballyProbabilities(CheckTask<storm::logic::GloballyFormula, ValueType> const& checkTask) {
            storm::logic::GloballyFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            auto ret = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeGloballyProbabilities(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(ret)));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeConditionalProbabilities(CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask) {
            storm::logic::ConditionalFormula const& conditionalFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(this->getModel().getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::InvalidPropertyException, "Cannot compute conditional probabilities on MDPs with more than one initial state.");
            STORM_LOG_THROW(conditionalFormula.getSubformula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException, "Illegal conditional probability formula.");
            STORM_LOG_THROW(conditionalFormula.getConditionFormula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException, "Illegal conditional probability formula.");

            std::unique_ptr<CheckResult> leftResultPointer = this->check(conditionalFormula.getSubformula().asEventuallyFormula().getSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(conditionalFormula.getConditionFormula().asEventuallyFormula().getSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();

            return storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeConditionalProbabilities(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), *minMaxLinearEquationSolverFactory);
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeCumulativeRewards(storm::logic::RewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) {
            storm::logic::CumulativeRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(rewardPathFormula.hasIntegerBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeCumulativeRewards(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getNonStrictBound<uint64_t>(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeInstantaneousRewards(storm::logic::RewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) {
            storm::logic::InstantaneousRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(rewardPathFormula.hasIntegerBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeInstantaneousRewards(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getBound<uint64_t>(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
                
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeReachabilityRewards(storm::logic::RewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(eventuallyFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            auto ret = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeReachabilityRewards(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), checkTask.isProduceSchedulersSet(), *minMaxLinearEquationSolverFactory, checkTask.getHint());
            std::unique_ptr<CheckResult> result(new ExplicitQuantitativeCheckResult<ValueType>(std::move(ret.values)));
            if (checkTask.isProduceSchedulersSet() && ret.scheduler) {
                result->asExplicitQuantitativeCheckResult<ValueType>().setScheduler(std::move(ret.scheduler));
            }
            return result;
        }

		template<typename SparseMdpModelType>
		std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) {
            storm::logic::StateFormula const& stateFormula = checkTask.getFormula();
			STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
			std::unique_ptr<CheckResult> subResultPointer = this->check(stateFormula);
			ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeLongRunAverageProbabilities(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(),  subResult.getTruthValuesVector(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
		}
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::computeLongRunAverageRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::vector<ValueType> result = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeLongRunAverageRewards(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getUniqueRewardModel(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(result)));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpPrctlModelChecker<SparseMdpModelType>::checkMultiObjectiveFormula(CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask) {
            return multiobjective::performMultiObjectiveModelChecking(this->getModel(), checkTask.getFormula());
        }
        
        template class SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>>;

#ifdef STORM_HAVE_CARL
        template class SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
#endif
    }
}
