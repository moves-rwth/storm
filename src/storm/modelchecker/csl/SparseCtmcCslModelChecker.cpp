#include "storm/modelchecker/csl/SparseCtmcCslModelChecker.h"

#include "storm/modelchecker/csl/helper/SparseCtmcCslHelper.h"
#include "storm/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"
#include "storm/utility/solver.h"
#include "storm/utility/FilteredRewardModel.h"

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
        SparseCtmcCslModelChecker<SparseCtmcModelType>::SparseCtmcCslModelChecker(SparseCtmcModelType const& model) : SparsePropositionalModelChecker<SparseCtmcModelType>(model) {
            // Intentionally left empty.
        }
        
        template <typename ModelType>
        bool SparseCtmcCslModelChecker<ModelType>::canHandleStatic(CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
            auto fragment = storm::logic::csrl().setGloballyFormulasAllowed(false).setLongRunAverageRewardFormulasAllowed(true).setLongRunAverageProbabilitiesAllowed(true).setTimeAllowed(true).setTimeOperatorsAllowed(true).setTotalRewardFormulasAllowed(true).setRewardAccumulationAllowed(true);
            if (!storm::NumberTraits<ValueType>::SupportsExponential) {
                fragment.setBoundedUntilFormulasAllowed(false).setCumulativeRewardFormulasAllowed(false).setInstantaneousFormulasAllowed(false);
            }
            return checkTask.getFormula().isInFragment(fragment);
        }
        
        template <typename SparseCtmcModelType>
        bool SparseCtmcCslModelChecker<SparseCtmcModelType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
            return canHandleStatic(checkTask);
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeBoundedUntilProbabilities(Environment const& env, CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();;
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();

            STORM_LOG_THROW(pathFormula.getTimeBoundReference().isTimeBound(), storm::exceptions::NotImplementedException, "Currently step-bounded or reward-bounded properties on CTMCs are not supported.");
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

            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeBoundedUntilProbabilities(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), this->getModel().getExitRateVector(), checkTask.isQualitativeSet(), lowerBound, upperBound);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeNextProbabilities(Environment const& env, CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) {
            storm::logic::NextFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeNextProbabilities(env, this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), subResult.getTruthValuesVector());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeUntilProbabilities(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRateVector(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeInstantaneousRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) {
            storm::logic::InstantaneousRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(!rewardPathFormula.isStepBounded(), storm::exceptions::NotImplementedException, "Currently step-bounded properties on CTMCs are not supported.");
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeInstantaneousRewards(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getBound<double>());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
                
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeCumulativeRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) {
            storm::logic::CumulativeRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(rewardPathFormula.getTimeBoundReference().isTimeBound(), storm::exceptions::NotImplementedException, "Currently step-bounded and reward-bounded properties on CTMCs are not supported.");
            auto rewardModel = storm::utility::createFilteredRewardModel(this->getModel(), checkTask);
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeCumulativeRewards(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), rewardModel.get(), rewardPathFormula.getNonStrictBound<double>());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
                
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeReachabilityRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, eventuallyFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            auto rewardModel = storm::utility::createFilteredRewardModel(this->getModel(), checkTask);
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeReachabilityRewards(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRateVector(), rewardModel.get(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeTotalRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::TotalRewardFormula, ValueType> const& checkTask) {
            auto rewardModel = storm::utility::createFilteredRewardModel(this->getModel(), checkTask);
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeTotalRewards(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRateVector(), rewardModel.get(), checkTask.isQualitativeSet());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeLongRunAverageProbabilities(Environment const& env, CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) {
            storm::logic::StateFormula const& stateFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, stateFormula);
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeLongRunAverageProbabilities(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector(), &this->getModel().getExitRateVector());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }

        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeLongRunAverageRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) {
            auto rewardModel = storm::utility::createFilteredRewardModel(this->getModel(), checkTask);
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeLongRunAverageRewards(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), rewardModel.get(), &this->getModel().getExitRateVector());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeReachabilityTimes(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, eventuallyFormula.getSubformula());
            ExplicitQualitativeCheckResult& subResult = subResultPointer->asExplicitQualitativeCheckResult();

            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeReachabilityTimes(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRateVector(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }

        template <typename SparseCtmcModelType>
        std::vector<typename SparseCtmcModelType::ValueType> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeAllTransientProbabilities(Environment const& env, CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(pathFormula.getTimeBoundReference().isTimeBound(), storm::exceptions::NotImplementedException, "Currently step-bounded or reward-bounded properties on CTMCs are not supported.");
            STORM_LOG_THROW(pathFormula.hasUpperBound(), storm::exceptions::NotImplementedException, "Computation needs upper limit for time bound.");
            double upperBound = pathFormula.getNonStrictUpperBound<double>();

            std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();;
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();

            std::vector<ValueType> result = storm::modelchecker::helper::SparseCtmcCslHelper::computeAllTransientProbabilities(env, this->getModel().getTransitionMatrix(), this->getModel().getInitialStates(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), this->getModel().getExitRateVector(), upperBound);
            return result;
        }

        // Explicitly instantiate the model checker.
        template class SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<double>>;

#ifdef STORM_HAVE_CARL
        template class SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<storm::RationalNumber>>;
        template class SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<storm::RationalFunction>>;
#endif

    } // namespace modelchecker
} // namespace storm
