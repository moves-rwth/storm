#include "storm/modelchecker/csl/HybridCtmcCslModelChecker.h"

#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/modelchecker/csl/helper/SparseCtmcCslHelper.h"
#include "storm/modelchecker/csl/helper/HybridCtmcCslHelper.h"

#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"

#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

#include "storm/logic/FragmentSpecification.h"

#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        template<typename ModelType>
        HybridCtmcCslModelChecker<ModelType>::HybridCtmcCslModelChecker(ModelType const& model) : SymbolicPropositionalModelChecker<ModelType>(model), linearEquationSolverFactory(std::make_unique<storm::solver::GeneralLinearEquationSolverFactory<ValueType>>()) {
            // Intentionally left empty.
        }
        
        template<typename ModelType>
        HybridCtmcCslModelChecker<ModelType>::HybridCtmcCslModelChecker(ModelType const& model, std::unique_ptr<storm::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory) : SymbolicPropositionalModelChecker<ModelType>(model), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template <typename ModelType>
        bool HybridCtmcCslModelChecker<ModelType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
            return HybridCtmcCslModelChecker<ModelType>::canHandleImplementation<ValueType>(checkTask);
        }
        
        template <typename ModelType>
        template<typename CValueType, typename std::enable_if<storm::NumberTraits<CValueType>::SupportsExponential, int>::type>
        bool HybridCtmcCslModelChecker<ModelType>::canHandleImplementation(CheckTask<storm::logic::Formula, CValueType> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            return formula.isInFragment(storm::logic::csrl().setGloballyFormulasAllowed(false).setLongRunAverageRewardFormulasAllowed(true).setLongRunAverageProbabilitiesAllowed(true));
        }
        
        template <typename ModelType>
        template<typename CValueType, typename std::enable_if<!storm::NumberTraits<CValueType>::SupportsExponential, int>::type>
        bool HybridCtmcCslModelChecker<ModelType>::canHandleImplementation(CheckTask<storm::logic::Formula, CValueType> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            return formula.isInFragment(storm::logic::prctl().setGloballyFormulasAllowed(false).setLongRunAverageRewardFormulasAllowed(true).setLongRunAverageProbabilitiesAllowed(true));
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<ModelType>::computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            
            return storm::modelchecker::helper::HybridCtmcCslHelper::computeUntilProbabilities<DdType, ValueType>(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<ModelType>::computeNextProbabilities(Environment const& env, CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) {
            storm::logic::NextFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            
            return storm::modelchecker::helper::HybridCtmcCslHelper::computeNextProbabilities<DdType, ValueType>(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), subResult.getTruthValuesVector());
        }


        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<ModelType>::computeReachabilityRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, eventuallyFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();

            return storm::modelchecker::helper::HybridCtmcCslHelper::computeReachabilityRewards<DdType, ValueType>(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *linearEquationSolverFactory);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<ModelType>::computeBoundedUntilProbabilities(Environment const& env, CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            
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
            
            return storm::modelchecker::helper::HybridCtmcCslHelper::computeBoundedUntilProbabilities<DdType, ValueType>(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet(), lowerBound, upperBound, *linearEquationSolverFactory);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<ModelType>::computeInstantaneousRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) {
            storm::logic::InstantaneousRewardFormula const& rewardPathFormula = checkTask.getFormula();
            
            STORM_LOG_THROW(!rewardPathFormula.isStepBounded(), storm::exceptions::NotImplementedException, "Currently step-bounded properties on CTMCs are not supported.");
            return storm::modelchecker::helper::HybridCtmcCslHelper::computeInstantaneousRewards<DdType, ValueType>(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getBound<double>(), *linearEquationSolverFactory);
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<ModelType>::computeCumulativeRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) {
            storm::logic::CumulativeRewardFormula const& rewardPathFormula = checkTask.getFormula();

            STORM_LOG_THROW(!rewardPathFormula.isStepBounded(), storm::exceptions::NotImplementedException, "Currently step-bounded properties on CTMCs are not supported.");
            return storm::modelchecker::helper::HybridCtmcCslHelper::computeCumulativeRewards<DdType, ValueType>(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getNonStrictBound<double>(), *linearEquationSolverFactory);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<ModelType>::computeLongRunAverageProbabilities(Environment const& env, CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) {
            storm::logic::StateFormula const& stateFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, stateFormula);
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            
            return storm::modelchecker::helper::HybridCtmcCslHelper::computeLongRunAverageProbabilities<DdType, ValueType>(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), subResult.getTruthValuesVector(), *linearEquationSolverFactory);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<ModelType>::computeLongRunAverageRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) {
            return storm::modelchecker::helper::HybridCtmcCslHelper::computeLongRunAverageRewards<DdType, ValueType>(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), *linearEquationSolverFactory);
        }
        
        // Explicitly instantiate the model checker.
        template class HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD, double>>;
        template class HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan, double>>;

        template class HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan, storm::RationalNumber>>;
        template class HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan, storm::RationalFunction>>;

    } // namespace modelchecker
} // namespace storm
