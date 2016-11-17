#include "src/modelchecker/prctl/SymbolicMdpPrctlModelChecker.h"

#include "src/modelchecker/prctl/helper/SymbolicMdpPrctlHelper.h"

#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQuantitativeCheckResult.h"

#include "src/logic/FragmentSpecification.h"

#include "src/models/symbolic/StandardRewardModel.h"

#include "src/utility/macros.h"
#include "src/utility/graph.h"

#include "src/settings/modules/GeneralSettings.h"

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidPropertyException.h"

#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace modelchecker {
        template<typename ModelType>
        SymbolicMdpPrctlModelChecker<ModelType>::SymbolicMdpPrctlModelChecker(ModelType const& model, std::unique_ptr<storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType>>&& linearEquationSolverFactory) : SymbolicPropositionalModelChecker<ModelType>(model), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename ModelType>
        SymbolicMdpPrctlModelChecker<ModelType>::SymbolicMdpPrctlModelChecker(ModelType const& model) : SymbolicPropositionalModelChecker<ModelType>(model), linearEquationSolverFactory(new storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType>()) {
            // Intentionally left empty.
        }
        
        template<typename ModelType>
        bool SymbolicMdpPrctlModelChecker<ModelType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            return formula.isInFragment(storm::logic::prctl().setLongRunAverageRewardFormulasAllowed(false));
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicMdpPrctlModelChecker<ModelType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeUntilProbabilities(checkTask.getOptimizationDirection(), this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicMdpPrctlModelChecker<ModelType>::computeGloballyProbabilities(CheckTask<storm::logic::GloballyFormula, ValueType> const& checkTask) {
            storm::logic::GloballyFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeGloballyProbabilities(checkTask.getOptimizationDirection(), this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicMdpPrctlModelChecker<ModelType>::computeNextProbabilities(CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) {
            storm::logic::NextFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeNextProbabilities(checkTask.getOptimizationDirection(), this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector());
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicMdpPrctlModelChecker<ModelType>::computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(pathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeBoundedUntilProbabilities(checkTask.getOptimizationDirection(), this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicMdpPrctlModelChecker<ModelType>::computeCumulativeRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) {
            storm::logic::CumulativeRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            return storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeCumulativeRewards(checkTask.getOptimizationDirection(), this->getModel(), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicMdpPrctlModelChecker<ModelType>::computeInstantaneousRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) {
            storm::logic::InstantaneousRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            return storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeInstantaneousRewards(checkTask.getOptimizationDirection(), this->getModel(), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> SymbolicMdpPrctlModelChecker<ModelType>::computeReachabilityRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(eventuallyFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeReachabilityRewards(checkTask.getOptimizationDirection(), this->getModel(), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
        }

        template class SymbolicMdpPrctlModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double>>;
        template class SymbolicMdpPrctlModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double>>;
    }
}
