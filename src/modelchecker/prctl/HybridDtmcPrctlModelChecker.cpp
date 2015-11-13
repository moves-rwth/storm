#include "src/modelchecker/prctl/HybridDtmcPrctlModelChecker.h"

#include "src/modelchecker/prctl/helper/HybridDtmcPrctlHelper.h"
#include "src/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"

#include "src/storage/dd/cudd/CuddOdd.h"
#include "src/storage/dd/DdManager.h"

#include "src/utility/macros.h"
#include "src/utility/graph.h"

#include "src/models/symbolic/StandardRewardModel.h"

#include "src/settings/modules/GeneralSettings.h"

#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "src/modelchecker/results/HybridQuantitativeCheckResult.h"

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType DdType, typename ValueType>
        HybridDtmcPrctlModelChecker<DdType, ValueType>::HybridDtmcPrctlModelChecker(storm::models::symbolic::Dtmc<DdType> const& model, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory) : SymbolicPropositionalModelChecker<DdType>(model), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        HybridDtmcPrctlModelChecker<DdType, ValueType>::HybridDtmcPrctlModelChecker(storm::models::symbolic::Dtmc<DdType> const& model) : SymbolicPropositionalModelChecker<DdType>(model), linearEquationSolverFactory(new storm::utility::solver::LinearEquationSolverFactory<ValueType>()) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        bool HybridDtmcPrctlModelChecker<DdType, ValueType>::canHandle(storm::logic::Formula const& formula) const {
            return formula.isPctlStateFormula() || formula.isPctlPathFormula() || formula.isRewardPathFormula();
        }
                
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<OptimizationDirection> const& optimalityType) {
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::HybridDtmcPrctlHelper<DdType, ValueType>::computeUntilProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative, *this->linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative, boost::optional<OptimizationDirection> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::HybridDtmcPrctlHelper<DdType, ValueType>::computeNextProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector());
        }
                
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative, boost::optional<OptimizationDirection> const& optimalityType) {
            STORM_LOG_THROW(pathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidArgumentException, "Formula needs to have a discrete time bound.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::HybridDtmcPrctlHelper<DdType, ValueType>::computeBoundedUntilProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
        }
                
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName, bool qualitative, boost::optional<OptimizationDirection> const& optimalityType) {
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidArgumentException, "Formula needs to have a discrete time bound.");
            return storm::modelchecker::helper::HybridDtmcPrctlHelper<DdType, ValueType>::computeCumulativeRewards(this->getModel(), this->getModel().getTransitionMatrix(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), rewardPathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
        }
                
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName, bool qualitative, boost::optional<OptimizationDirection> const& optimalityType) {
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidArgumentException, "Formula needs to have a discrete time bound.");
            return storm::modelchecker::helper::HybridDtmcPrctlHelper<DdType, ValueType>::computeInstantaneousRewards(this->getModel(), this->getModel().getTransitionMatrix(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), rewardPathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName, bool qualitative, boost::optional<OptimizationDirection> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(rewardPathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::HybridDtmcPrctlHelper<DdType, ValueType>::computeReachabilityRewards(this->getModel(), this->getModel().getTransitionMatrix(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), qualitative, *this->linearEquationSolverFactory);
        }
                
        template<storm::dd::DdType DdType, typename ValueType>
        storm::models::symbolic::Dtmc<DdType> const& HybridDtmcPrctlModelChecker<DdType, ValueType>::getModel() const {
            return this->template getModelAs<storm::models::symbolic::Dtmc<DdType>>();
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeLongRunAverage(storm::logic::StateFormula const& stateFormula, bool qualitative, boost::optional<OptimizationDirection> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(stateFormula);
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            
            // Create ODD for the translation.
            storm::dd::Odd<DdType> odd(this->getModel().getReachableStates());
            
            storm::storage::SparseMatrix<ValueType> explicitProbabilityMatrix = this->getModel().getTransitionMatrix().toMatrix(odd, odd);
            
            std::vector<ValueType> result = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeLongRunAverage(explicitProbabilityMatrix, subResult.getTruthValuesVector().toVector(odd), qualitative, *this->linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), this->getModel().getManager().getBddZero(), this->getModel().getManager().getAddZero(), this->getModel().getReachableStates(), std::move(odd), std::move(result)));
        }
        
        template class HybridDtmcPrctlModelChecker<storm::dd::DdType::CUDD, double>;
    }
}