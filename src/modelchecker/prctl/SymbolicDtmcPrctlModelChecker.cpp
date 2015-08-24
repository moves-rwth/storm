#include "src/modelchecker/prctl/SymbolicDtmcPrctlModelChecker.h"

#include "src/modelchecker/prctl/helper/SymbolicDtmcPrctlHelper.h"

#include "src/storage/dd/Add.h"

#include "src/utility/macros.h"

#include "src/models/symbolic/StandardRewardModel.h"

#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQuantitativeCheckResult.h"

#include "src/settings/modules/GeneralSettings.h"

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicDtmcPrctlModelChecker<DdType, ValueType>::SymbolicDtmcPrctlModelChecker(storm::models::symbolic::Dtmc<DdType> const& model, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType>>&& linearEquationSolverFactory) : SymbolicPropositionalModelChecker<DdType>(model), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicDtmcPrctlModelChecker<DdType, ValueType>::SymbolicDtmcPrctlModelChecker(storm::models::symbolic::Dtmc<DdType> const& model) : SymbolicPropositionalModelChecker<DdType>(model), linearEquationSolverFactory(new storm::utility::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType>()) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        bool SymbolicDtmcPrctlModelChecker<DdType, ValueType>::canHandle(storm::logic::Formula const& formula) const {
            return formula.isPctlStateFormula() || formula.isPctlPathFormula() || formula.isRewardPathFormula();
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<DdType, ValueType>::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            storm::dd::Add<DdType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeUntilProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative, *this->linearEquationSolverFactory);
            return std::unique_ptr<SymbolicQuantitativeCheckResult<DdType>>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), numericResult));
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<DdType, ValueType>::computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            storm::dd::Add<DdType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeNextProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector());
            return std::unique_ptr<SymbolicQuantitativeCheckResult<DdType>>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), numericResult));
        }
                
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<DdType, ValueType>::computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(pathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            storm::dd::Add<DdType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeBoundedUntilProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
            return std::unique_ptr<SymbolicQuantitativeCheckResult<DdType>>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), numericResult));
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<DdType, ValueType>::computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            storm::dd::Add<DdType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeCumulativeRewards(this->getModel(), this->getModel().getTransitionMatrix(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), rewardPathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
            return std::unique_ptr<SymbolicQuantitativeCheckResult<DdType>>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), numericResult));
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<DdType, ValueType>::computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            storm::dd::Add<DdType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeInstantaneousRewards(this->getModel(), this->getModel().getTransitionMatrix(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), rewardPathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
            return std::unique_ptr<SymbolicQuantitativeCheckResult<DdType>>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), numericResult));
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicDtmcPrctlModelChecker<DdType, ValueType>::computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(rewardPathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            storm::dd::Add<DdType> numericResult = storm::modelchecker::helper::SymbolicDtmcPrctlHelper<DdType, ValueType>::computeReachabilityRewards(this->getModel(), this->getModel().getTransitionMatrix(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), qualitative, *this->linearEquationSolverFactory);
            return std::unique_ptr<SymbolicQuantitativeCheckResult<DdType>>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), numericResult));
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::models::symbolic::Dtmc<DdType> const& SymbolicDtmcPrctlModelChecker<DdType, ValueType>::getModel() const {
            return this->template getModelAs<storm::models::symbolic::Dtmc<DdType>>();
        }
        
        template class SymbolicDtmcPrctlModelChecker<storm::dd::DdType::CUDD, double>;
    }
}
