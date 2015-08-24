#include "src/modelchecker/csl/HybridCtmcCslModelChecker.h"

#include "src/models/symbolic/StandardRewardModel.h"

#include "src/modelchecker/csl/helper/SparseCtmcCslHelper.h"
#include "src/modelchecker/csl/helper/HybridCtmcCslHelper.h"

#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"

#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddBdd.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType DdType, class ValueType>
        HybridCtmcCslModelChecker<DdType, ValueType>::HybridCtmcCslModelChecker(storm::models::symbolic::Ctmc<DdType> const& model) : SymbolicPropositionalModelChecker<DdType>(model), linearEquationSolverFactory(new storm::utility::solver::LinearEquationSolverFactory<ValueType>()) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        HybridCtmcCslModelChecker<DdType, ValueType>::HybridCtmcCslModelChecker(storm::models::symbolic::Ctmc<DdType> const& model, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory) : SymbolicPropositionalModelChecker<DdType>(model), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        bool HybridCtmcCslModelChecker<DdType, ValueType>::canHandle(storm::logic::Formula const& formula) const {
            return formula.isCslStateFormula() || formula.isCslPathFormula() || formula.isRewardPathFormula();
        }
                
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            
            return storm::modelchecker::helper::HybridCtmcCslHelper<DdType, ValueType>::computeUntilProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative, *this->linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            
            return storm::modelchecker::helper::HybridCtmcCslHelper<DdType, ValueType>::computeNextProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), subResult.getTruthValuesVector());
        }

        template<storm::dd::DdType DdType, class ValueType>
        storm::models::symbolic::Ctmc<DdType> const& HybridCtmcCslModelChecker<DdType, ValueType>::getModel() const {
            return this->template getModelAs<storm::models::symbolic::Ctmc<DdType>>();
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(rewardPathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();

            return storm::modelchecker::helper::HybridCtmcCslHelper<DdType, ValueType>::computeReachabilityRewards(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), qualitative, *linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            double lowerBound = 0;
            double upperBound = 0;
            if (!pathFormula.hasDiscreteTimeBound()) {
                std::pair<double, double> const& intervalBounds =  pathFormula.getIntervalBounds();
                lowerBound = intervalBounds.first;
                upperBound = intervalBounds.second;
            } else {
                upperBound = pathFormula.getDiscreteTimeBound();
            }
            
            return storm::modelchecker::helper::HybridCtmcCslHelper<DdType, ValueType>::computeBoundedUntilProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative, lowerBound, upperBound, *linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            return storm::modelchecker::helper::HybridCtmcCslHelper<DdType, ValueType>::computeInstantaneousRewards(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), rewardPathFormula.getContinuousTimeBound(), *linearEquationSolverFactory);
        }

        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            return storm::modelchecker::helper::HybridCtmcCslHelper<DdType, ValueType>::computeCumulativeRewards(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), rewardPathFormula.getContinuousTimeBound(), *linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeLongRunAverage(storm::logic::StateFormula const& stateFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(stateFormula);
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            
            return storm::modelchecker::helper::HybridCtmcCslHelper<DdType, ValueType>::computeLongRunAverage(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), subResult.getTruthValuesVector(), qualitative, *linearEquationSolverFactory);
        }
        
        // Explicitly instantiate the model checker.
        template class HybridCtmcCslModelChecker<storm::dd::DdType::CUDD, double>;
        
    } // namespace modelchecker
} // namespace storm