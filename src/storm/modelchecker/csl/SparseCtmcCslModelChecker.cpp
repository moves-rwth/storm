#include "storm/modelchecker/csl/SparseCtmcCslModelChecker.h"

#include "storm/modelchecker/csl/helper/SparseCtmcCslHelper.h"
#include "storm/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"
#include "storm/utility/solver.h"

#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "storm/logic/FragmentSpecification.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        template <typename SparseCtmcModelType>
        SparseCtmcCslModelChecker<SparseCtmcModelType>::SparseCtmcCslModelChecker(SparseCtmcModelType const& model) : SparsePropositionalModelChecker<SparseCtmcModelType>(model), linearEquationSolverFactory(std::make_unique<storm::solver::GeneralLinearEquationSolverFactory<ValueType>>()) {
            // Intentionally left empty.
        }
        
        template <typename SparseCtmcModelType>
        SparseCtmcCslModelChecker<SparseCtmcModelType>::SparseCtmcCslModelChecker(SparseCtmcModelType const& model, std::unique_ptr<storm::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory) : SparsePropositionalModelChecker<SparseCtmcModelType>(model), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template <typename SparseCtmcModelType>
        bool SparseCtmcCslModelChecker<SparseCtmcModelType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
            return SparseCtmcCslModelChecker<SparseCtmcModelType>::canHandleImplementation<ValueType>(checkTask);
        }
        
        template <typename SparseCtmcModelType>
        template<typename CValueType, typename std::enable_if<storm::NumberTraits<CValueType>::SupportsExponential, int>::type>
        bool SparseCtmcCslModelChecker<SparseCtmcModelType>::canHandleImplementation(CheckTask<storm::logic::Formula, CValueType> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            return formula.isInFragment(storm::logic::csrl().setGloballyFormulasAllowed(false).setLongRunAverageRewardFormulasAllowed(true).setLongRunAverageProbabilitiesAllowed(true).setTimeAllowed(true));
        }
        
        template <typename SparseCtmcModelType>
        template<typename CValueType, typename std::enable_if<!storm::NumberTraits<CValueType>::SupportsExponential, int>::type>
        bool SparseCtmcCslModelChecker<SparseCtmcModelType>::canHandleImplementation(CheckTask<storm::logic::Formula, CValueType> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            return formula.isInFragment(storm::logic::prctl().setGloballyFormulasAllowed(false).setLongRunAverageRewardFormulasAllowed(true).setLongRunAverageProbabilitiesAllowed(true).setTimeAllowed(true));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeBoundedUntilProbabilities(Environment const& env, CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();;
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();

            STORM_LOG_THROW(!pathFormula.getTimeBoundReference().isStepBound(), storm::exceptions::NotImplementedException, "Currently step-bounded properties on CTMCs are not supported.");
            double lowerBound = 0;
            double upperBound = 0;
            if (pathFormula.hasLowerBound()) {
                lowerBound = pathFormula.getLowerBound<double>();
            }
            if (pathFormula.hasUpperBound()) {
                upperBound = pathFormula.getNonStrictUpperBound<double>();
            } else {
                upperBound = storm::utility::infinity<double>();
            }

            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeBoundedUntilProbabilities(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), this->getModel().getExitRateVector(), checkTask.isQualitativeSet(), lowerBound, upperBound, *linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeNextProbabilities(Environment const& env, CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) {
            storm::logic::NextFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeNextProbabilities(this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), subResult.getTruthValuesVector(), *linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeUntilProbabilities(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRateVector(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeInstantaneousRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) {
            storm::logic::InstantaneousRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(!rewardPathFormula.isStepBounded(), storm::exceptions::NotImplementedException, "Currently step-bounded properties on CTMCs are not supported.");
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeInstantaneousRewards(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getBound<double>(), *linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
                
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeCumulativeRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) {
            storm::logic::CumulativeRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(!rewardPathFormula.isStepBounded(), storm::exceptions::NotImplementedException, "Currently step-bounded properties on CTMCs are not supported.");
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeCumulativeRewards(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getNonStrictBound<double>(), *linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
                
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeReachabilityRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, eventuallyFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeReachabilityRewards(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRateVector(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeLongRunAverageProbabilities(Environment const& env, CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) {
            storm::logic::StateFormula const& stateFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, stateFormula);
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            
            storm::storage::SparseMatrix<ValueType> probabilityMatrix = storm::modelchecker::helper::SparseCtmcCslHelper::computeProbabilityMatrix(this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector());
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeLongRunAverageProbabilities(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), probabilityMatrix, subResult.getTruthValuesVector(), &this->getModel().getExitRateVector(), *linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }

        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeLongRunAverageRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) {
            storm::storage::SparseMatrix<ValueType> probabilityMatrix = storm::modelchecker::helper::SparseCtmcCslHelper::computeProbabilityMatrix(this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector());
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeLongRunAverageRewards(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), probabilityMatrix, checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), &this->getModel().getExitRateVector(), *linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeReachabilityTimes(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, eventuallyFormula.getSubformula());
            ExplicitQualitativeCheckResult& subResult = subResultPointer->asExplicitQualitativeCheckResult();

            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeReachabilityTimes(storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRateVector(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }

        // Explicitly instantiate the model checker.
        template class SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<double>>;

#ifdef STORM_HAVE_CARL
        template class SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<storm::RationalNumber>>;
        template class SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<storm::RationalFunction>>;
#endif

    } // namespace modelchecker
} // namespace storm
