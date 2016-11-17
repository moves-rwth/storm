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
        SymbolicDtmcPrctlModelChecker<ModelType>::SymbolicDtmcPrctlModelChecker(ModelType const& model, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType>>&& linearEquationSolverFactory) : SymbolicPropositionalModelChecker<ModelType>(model), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename ModelType>
        SymbolicDtmcPrctlModelChecker<ModelType>::SymbolicDtmcPrctlModelChecker(ModelType const& model) : SymbolicPropositionalModelChecker<ModelType>(model), linearEquationSolverFactory(new storm::utility::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType>()) {
            // Intentionally left empty.
        }
        
        template<typename ModelType>
        bool SymbolicDtmcPrctlModelChecker<ModelType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            return formula.isInFragment(storm::logic::prctl().setLongRunAverageRewardFormulasAllowed(false));
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<ModelType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            storm::dd::Add<DdType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeUntilProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
            return std::unique_ptr<SymbolicQuantitativeCheckResult<DdType>>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), numericResult));
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<ModelType>::computeGloballyProbabilities(CheckTask<storm::logic::GloballyFormula, ValueType> const& checkTask) {
            storm::logic::GloballyFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            storm::dd::Add<DdType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeGloballyProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
            return std::unique_ptr<SymbolicQuantitativeCheckResult<DdType>>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), numericResult));
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<ModelType>::computeNextProbabilities(CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) {
            storm::logic::NextFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            storm::dd::Add<DdType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeNextProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector());
            return std::unique_ptr<SymbolicQuantitativeCheckResult<DdType>>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), numericResult));
        }
                
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<ModelType>::computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(pathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            storm::dd::Add<DdType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeBoundedUntilProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
            return std::unique_ptr<SymbolicQuantitativeCheckResult<DdType>>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), numericResult));
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<ModelType>::computeCumulativeRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) {
            storm::logic::CumulativeRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            storm::dd::Add<DdType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeCumulativeRewards(this->getModel(), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
            return std::unique_ptr<SymbolicQuantitativeCheckResult<DdType>>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), numericResult));
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<ModelType>::computeInstantaneousRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) {
            storm::logic::InstantaneousRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            storm::dd::Add<DdType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeInstantaneousRewards(this->getModel(), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
            return std::unique_ptr<SymbolicQuantitativeCheckResult<DdType>>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), numericResult));
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<ModelType>::computeReachabilityRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(eventuallyFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            storm::dd::Add<DdType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeReachabilityRewards(this->getModel(), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
            return std::unique_ptr<SymbolicQuantitativeCheckResult<DdType>>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), numericResult));
        }


        template class SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>>;
        template class SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>>;
    }
}
