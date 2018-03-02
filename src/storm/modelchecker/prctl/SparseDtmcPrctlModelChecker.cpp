#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"

#include <vector>
#include <memory>

#include "storm/utility/macros.h"

#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "storm/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"
#include "storm/modelchecker/csl/helper/SparseCtmcCslHelper.h"

#include "storm/logic/FragmentSpecification.h"

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/settings/modules/GeneralSettings.h"

#include "storm/exceptions/InvalidStateException.h"

#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        template<typename SparseDtmcModelType>
        SparseDtmcPrctlModelChecker<SparseDtmcModelType>::SparseDtmcPrctlModelChecker(SparseDtmcModelType const& model) : SparsePropositionalModelChecker<SparseDtmcModelType>(model) {
            // Intentionally left empty.
        }
        
        template<typename SparseDtmcModelType>
        bool SparseDtmcPrctlModelChecker<SparseDtmcModelType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            return formula.isInFragment(storm::logic::prctl().setLongRunAverageRewardFormulasAllowed(true).setLongRunAverageProbabilitiesAllowed(true).setConditionalProbabilityFormulasAllowed(true).setConditionalRewardFormulasAllowed(true).setOnlyEventuallyFormuluasInConditionalFormulasAllowed(true).setRewardBoundedUntilFormulasAllowed(true).setRewardBoundedCumulativeRewardFormulasAllowed(true).setMultiDimensionalBoundedUntilFormulasAllowed(true).setMultiDimensionalCumulativeRewardFormulasAllowed(true));
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeBoundedUntilProbabilities(Environment const& env, CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
            if (pathFormula.isMultiDimensional() || pathFormula.getTimeBoundReference().isRewardBound()) {
                STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::InvalidOperationException, "Checking non-trivial bounded until probabilities can only be computed for the initial states of the model.");
                storm::logic::OperatorInformation opInfo;
                if (checkTask.isBoundSet()) {
                    opInfo.bound = checkTask.getBound();
                }
                auto formula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(checkTask.getFormula().asSharedPointer(), opInfo);
                auto numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeRewardBoundedValues(env, this->getModel(), formula);
                return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
            } else {
                STORM_LOG_THROW(!pathFormula.hasLowerBound() && pathFormula.hasUpperBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have single upper time bound.");
                STORM_LOG_THROW(pathFormula.hasIntegerUpperBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have discrete upper time bound.");
                std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
                std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
                ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
                ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
                std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeStepBoundedUntilProbabilities(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getNonStrictUpperBound<uint64_t>(), checkTask.getHint());
                std::unique_ptr<CheckResult> result = std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
               return result;
            }
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeNextProbabilities(Environment const& env, CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) {
            storm::logic::NextFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeNextProbabilities(env, this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeUntilProbabilities(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet(), checkTask.getHint());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeGloballyProbabilities(Environment const& env, CheckTask<storm::logic::GloballyFormula, ValueType> const& checkTask) {
            storm::logic::GloballyFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeGloballyProbabilities(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeCumulativeRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) {
            storm::logic::CumulativeRewardFormula const& rewardPathFormula = checkTask.getFormula();
            if (rewardPathFormula.isMultiDimensional() || rewardPathFormula.getTimeBoundReference().isRewardBound()) {
                STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::InvalidOperationException, "Checking non-trivial bounded until probabilities can only be computed for the initial states of the model.");
                storm::logic::OperatorInformation opInfo;
                if (checkTask.isBoundSet()) {
                    opInfo.bound = checkTask.getBound();
                }
                auto formula = std::make_shared<storm::logic::RewardOperatorFormula>(checkTask.getFormula().asSharedPointer(), checkTask.getRewardModel(), opInfo);
                auto numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeRewardBoundedValues(env, this->getModel(), formula);
                return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
            } else {
                STORM_LOG_THROW(rewardPathFormula.hasIntegerBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
                std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeCumulativeRewards(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getNonStrictBound<uint64_t>());
                return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
            }
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeInstantaneousRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) {
            storm::logic::InstantaneousRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(rewardPathFormula.hasIntegerBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeInstantaneousRewards(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getBound<uint64_t>());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeReachabilityRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, eventuallyFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeReachabilityRewards(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), checkTask.getHint());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }

        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeLongRunAverageProbabilities(Environment const& env, CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) {
            storm::logic::StateFormula const& stateFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, stateFormula);
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeLongRunAverageProbabilities<ValueType>(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector(), nullptr);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeLongRunAverageRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) {
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeLongRunAverageRewards<ValueType>(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), nullptr);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));

        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeConditionalProbabilities(Environment const& env, CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask) {
            storm::logic::ConditionalFormula const& conditionalFormula = checkTask.getFormula();
            STORM_LOG_THROW(conditionalFormula.getSubformula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException, "Illegal conditional probability formula.");
            STORM_LOG_THROW(conditionalFormula.getConditionFormula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException, "Illegal conditional probability formula.");

            std::unique_ptr<CheckResult> leftResultPointer = this->check(env, conditionalFormula.getSubformula().asEventuallyFormula().getSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(env, conditionalFormula.getConditionFormula().asEventuallyFormula().getSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();

            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeConditionalProbabilities(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeConditionalRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask) {
            storm::logic::ConditionalFormula const& conditionalFormula = checkTask.getFormula();
            STORM_LOG_THROW(conditionalFormula.getSubformula().isReachabilityRewardFormula(), storm::exceptions::InvalidPropertyException, "Illegal conditional probability formula.");
            STORM_LOG_THROW(conditionalFormula.getConditionFormula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException, "Illegal conditional probability formula.");
            
            std::unique_ptr<CheckResult> leftResultPointer = this->check(env, conditionalFormula.getSubformula().asReachabilityRewardFormula().getSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(env, conditionalFormula.getConditionFormula().asEventuallyFormula().getSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeConditionalRewards(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template class SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<double>>;

#ifdef STORM_HAVE_CARL
        template class SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<storm::RationalNumber>>;
        template class SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>>;
#endif
    }
}
