#include "src/modelchecker/prctl/SymbolicMdpPrctlModelChecker.h"

#include "src/modelchecker/prctl/helper/SymbolicMdpPrctlHelper.h"

#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQuantitativeCheckResult.h"

#include "src/models/symbolic/StandardRewardModel.h"

#include "src/utility/macros.h"
#include "src/utility/graph.h"

#include "src/settings/modules/GeneralSettings.h"

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidPropertyException.h"

#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicMdpPrctlModelChecker<DdType, ValueType>::SymbolicMdpPrctlModelChecker(storm::models::symbolic::Mdp<DdType, ValueType> const& model, std::unique_ptr<storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType>>&& linearEquationSolverFactory) : SymbolicPropositionalModelChecker<DdType, ValueType>(model), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicMdpPrctlModelChecker<DdType, ValueType>::SymbolicMdpPrctlModelChecker(storm::models::symbolic::Mdp<DdType, ValueType> const& model) : SymbolicPropositionalModelChecker<DdType, ValueType>(model), linearEquationSolverFactory(new storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType>()) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        bool SymbolicMdpPrctlModelChecker<DdType, ValueType>::canHandle(storm::logic::Formula const& formula) const {
            if (formula.isPctlStateFormula() || formula.isPctlPathFormula() || formula.isRewardPathFormula()) {
                return true;
            }
            if (formula.isProbabilityOperatorFormula()) {
                return this->canHandle(formula.asProbabilityOperatorFormula().getSubformula());
            }
            if (formula.isGloballyFormula()) {
                return true;
            }
            return false;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicMdpPrctlModelChecker<DdType, ValueType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeUntilProbabilities(optimalityType.get() == OptimizationDirection::Minimize, this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative, *this->linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicMdpPrctlModelChecker<DdType, ValueType>::computeGloballyProbabilities(CheckTask<storm::logic::GloballyFormula> const& checkTask) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeGloballyProbabilities(optimalityType.get() == OptimizationDirection::Minimize, this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector(), qualitative, *this->linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicMdpPrctlModelChecker<DdType, ValueType>::computeNextProbabilities(CheckTask<storm::logic::NextFormula> const& checkTask) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeNextProbabilities(optimalityType.get() == OptimizationDirection::Minimize, this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector());
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicMdpPrctlModelChecker<DdType, ValueType>::computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula> const& checkTask) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(pathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeBoundedUntilProbabilities(optimalityType.get() == OptimizationDirection::Minimize, this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicMdpPrctlModelChecker<DdType, ValueType>::computeCumulativeRewards(CheckTask<storm::logic::CumulativeRewardFormula> const& checkTask) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            return storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeCumulativeRewards(optimalityType.get() == OptimizationDirection::Minimize, this->getModel(), this->getModel().getTransitionMatrix(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), rewardPathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicMdpPrctlModelChecker<DdType, ValueType>::computeInstantaneousRewards(CheckTask<storm::logic::InstantaneousRewardFormula> const& checkTask) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidArgumentException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            return storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeInstantaneousRewards(optimalityType.get() == OptimizationDirection::Minimize, this->getModel(), this->getModel().getTransitionMatrix(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), rewardPathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicMdpPrctlModelChecker<DdType, ValueType>::computeReachabilityRewards(CheckTask<storm::logic::ReachabilityRewardFormula> const& checkTask) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(rewardPathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::SymbolicMdpPrctlHelper<DdType, ValueType>::computeReachabilityRewards(optimalityType.get() == OptimizationDirection::Minimize, this->getModel(), this->getModel().getTransitionMatrix(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), qualitative, *this->linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::models::symbolic::Mdp<DdType, ValueType> const& SymbolicMdpPrctlModelChecker<DdType, ValueType>::getModel() const {
            return this->template getModelAs<storm::models::symbolic::Mdp<DdType>>();
        }
        
        template class SymbolicMdpPrctlModelChecker<storm::dd::DdType::CUDD, double>;
        template class SymbolicMdpPrctlModelChecker<storm::dd::DdType::Sylvan, double>;
    }
}
