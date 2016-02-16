#include "src/modelchecker/csl/HybridCtmcCslModelChecker.h"

#include "src/models/symbolic/StandardRewardModel.h"

#include "src/modelchecker/csl/helper/SparseCtmcCslHelper.h"
#include "src/modelchecker/csl/helper/HybridCtmcCslHelper.h"

#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"

#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType DdType, class ValueType>
        HybridCtmcCslModelChecker<DdType, ValueType>::HybridCtmcCslModelChecker(storm::models::symbolic::Ctmc<DdType, ValueType> const& model) : SymbolicPropositionalModelChecker<DdType, ValueType>(model), linearEquationSolverFactory(new storm::utility::solver::LinearEquationSolverFactory<ValueType>()) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        HybridCtmcCslModelChecker<DdType, ValueType>::HybridCtmcCslModelChecker(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory) : SymbolicPropositionalModelChecker<DdType, ValueType>(model), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        bool HybridCtmcCslModelChecker<DdType, ValueType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            return formula.isCslStateFormula() || formula.isCslPathFormula();
        }
                
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            
            return storm::modelchecker::helper::HybridCtmcCslHelper<DdType, ValueType>::computeUntilProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeNextProbabilities(CheckTask<storm::logic::NextFormula> const& checkTask) {
            storm::logic::NextFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            
            return storm::modelchecker::helper::HybridCtmcCslHelper<DdType, ValueType>::computeNextProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), subResult.getTruthValuesVector());
        }

        template<storm::dd::DdType DdType, class ValueType>
        storm::models::symbolic::Ctmc<DdType, ValueType> const& HybridCtmcCslModelChecker<DdType, ValueType>::getModel() const {
            return this->template getModelAs<storm::models::symbolic::Ctmc<DdType, ValueType>>();
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeReachabilityRewards(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(eventuallyFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();

            return storm::modelchecker::helper::HybridCtmcCslHelper<DdType, ValueType>::computeReachabilityRewards(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
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
            
            return storm::modelchecker::helper::HybridCtmcCslHelper<DdType, ValueType>::computeBoundedUntilProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet(), lowerBound, upperBound, *linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeInstantaneousRewards(CheckTask<storm::logic::InstantaneousRewardFormula> const& checkTask) {
            storm::logic::InstantaneousRewardFormula const& rewardPathFormula = checkTask.getFormula();
            return storm::modelchecker::helper::HybridCtmcCslHelper<DdType, ValueType>::computeInstantaneousRewards(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getContinuousTimeBound(), *linearEquationSolverFactory);
        }

        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeCumulativeRewards(CheckTask<storm::logic::CumulativeRewardFormula> const& checkTask) {
            storm::logic::CumulativeRewardFormula const& rewardPathFormula = checkTask.getFormula();
            return storm::modelchecker::helper::HybridCtmcCslHelper<DdType, ValueType>::computeCumulativeRewards(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getContinuousTimeBound(), *linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, class ValueType>
        std::unique_ptr<CheckResult> HybridCtmcCslModelChecker<DdType, ValueType>::computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula> const& checkTask) {
            storm::logic::StateFormula const& stateFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(stateFormula);
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            
            return storm::modelchecker::helper::HybridCtmcCslHelper<DdType, ValueType>::computeLongRunAverageProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *linearEquationSolverFactory);
        }
        
        // Explicitly instantiate the model checker.
        template class HybridCtmcCslModelChecker<storm::dd::DdType::CUDD, double>;
        template class HybridCtmcCslModelChecker<storm::dd::DdType::Sylvan, double>;
        
    } // namespace modelchecker
} // namespace storm