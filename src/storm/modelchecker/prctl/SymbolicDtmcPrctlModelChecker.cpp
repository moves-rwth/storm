#include "storm/modelchecker/prctl/SymbolicDtmcPrctlModelChecker.h"

#include "storm/modelchecker/prctl/helper/SymbolicDtmcPrctlHelper.h"

#include "storm/storage/dd/Add.h"

#include "storm/utility/macros.h"

#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQuantitativeCheckResult.h"

#include "storm/logic/FragmentSpecification.h"

#include "storm/solver/SymbolicLinearEquationSolver.h"

#include "storm/settings/modules/GeneralSettings.h"

#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        template<typename ModelType>
        SymbolicDtmcPrctlModelChecker<ModelType>::SymbolicDtmcPrctlModelChecker(ModelType const& model, std::unique_ptr<storm::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType>>&& linearEquationSolverFactory) : SymbolicPropositionalModelChecker<ModelType>(model), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename ModelType>
        SymbolicDtmcPrctlModelChecker<ModelType>::SymbolicDtmcPrctlModelChecker(ModelType const& model) : SymbolicPropositionalModelChecker<ModelType>(model), linearEquationSolverFactory(std::make_unique<storm::solver::GeneralSymbolicLinearEquationSolverFactory<DdType, ValueType>>()) {
            // Intentionally left empty.
        }
        
        template<typename ModelType>
        bool SymbolicDtmcPrctlModelChecker<ModelType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            return formula.isInFragment(storm::logic::prctl().setLongRunAverageRewardFormulasAllowed(false));
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<ModelType>::computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            storm::dd::Add<DdType, ValueType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeUntilProbabilities(env, this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
            return std::make_unique<SymbolicQuantitativeCheckResult<DdType, ValueType>>(this->getModel().getReachableStates(), numericResult);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<ModelType>::computeGloballyProbabilities(Environment const& env, CheckTask<storm::logic::GloballyFormula, ValueType> const& checkTask) {
            storm::logic::GloballyFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            storm::dd::Add<DdType, ValueType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeGloballyProbabilities(env, this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
            return std::make_unique<SymbolicQuantitativeCheckResult<DdType, ValueType>>(this->getModel().getReachableStates(), numericResult);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<ModelType>::computeNextProbabilities(Environment const& env, CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) {
            storm::logic::NextFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            storm::dd::Add<DdType, ValueType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeNextProbabilities(env, this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector());
            return std::make_unique<SymbolicQuantitativeCheckResult<DdType, ValueType>>(this->getModel().getReachableStates(), numericResult);
        }
                
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<ModelType>::computeBoundedUntilProbabilities(Environment const& env, CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(!pathFormula.hasLowerBound() && pathFormula.hasUpperBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have single upper time bound.");
            STORM_LOG_THROW(pathFormula.hasIntegerUpperBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have discrete upper time bound.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            storm::dd::Add<DdType, ValueType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeBoundedUntilProbabilities(env, this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getNonStrictUpperBound<uint64_t>(), *this->linearEquationSolverFactory);
            return std::unique_ptr<SymbolicQuantitativeCheckResult<DdType, ValueType>>(new SymbolicQuantitativeCheckResult<DdType, ValueType>(this->getModel().getReachableStates(), numericResult));
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<ModelType>::computeCumulativeRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) {
            storm::logic::CumulativeRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(rewardPathFormula.hasIntegerBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            storm::dd::Add<DdType, ValueType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeCumulativeRewards(env, this->getModel(), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getNonStrictBound<uint64_t>(), *this->linearEquationSolverFactory);
            return std::unique_ptr<SymbolicQuantitativeCheckResult<DdType, ValueType>>(new SymbolicQuantitativeCheckResult<DdType, ValueType>(this->getModel().getReachableStates(), numericResult));
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<ModelType>::computeInstantaneousRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) {
            storm::logic::InstantaneousRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(rewardPathFormula.hasIntegerBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            storm::dd::Add<DdType, ValueType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeInstantaneousRewards(env, this->getModel(), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getBound<uint64_t>(), *this->linearEquationSolverFactory);
            return std::make_unique<SymbolicQuantitativeCheckResult<DdType, ValueType>>(this->getModel().getReachableStates(), numericResult);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<ModelType>::computeReachabilityRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, eventuallyFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            storm::dd::Add<DdType, ValueType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeReachabilityRewards(env, this->getModel(), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
            return std::make_unique<SymbolicQuantitativeCheckResult<DdType, ValueType>>(this->getModel().getReachableStates(), numericResult);
        }


        template class SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>>;
        template class SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>>;

        template class SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, storm::RationalNumber>>;
        template class SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, storm::RationalFunction>>;
    }
}
