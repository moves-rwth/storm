#include "src/modelchecker/csl/SparseCtmcCslModelChecker.h"

#include "src/modelchecker/csl/helper/SparseCtmcCslHelper.h"
#include "src/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"

#include "src/models/sparse/StandardRewardModel.h"

#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"
#include "src/utility/solver.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/logic/FragmentSpecification.h"

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        template <typename SparseCtmcModelType>
        SparseCtmcCslModelChecker<SparseCtmcModelType>::SparseCtmcCslModelChecker(SparseCtmcModelType const& model) : SparsePropositionalModelChecker<SparseCtmcModelType>(model), linearEquationSolverFactory(new storm::utility::solver::LinearEquationSolverFactory<ValueType>()) {
            // Intentionally left empty.
        }
        
        template <typename SparseCtmcModelType>
        SparseCtmcCslModelChecker<SparseCtmcModelType>::SparseCtmcCslModelChecker(SparseCtmcModelType const& model, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory) : SparsePropositionalModelChecker<SparseCtmcModelType>(model), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template <typename SparseCtmcModelType>
        bool SparseCtmcCslModelChecker<SparseCtmcModelType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            return formula.isInFragment(storm::logic::csrl().setGloballyFormulasAllowed(false).setLongRunAverageRewardFormulasAllowed(false).setLongRunAverageProbabilitiesAllowed(true).setExpectedTimeAllowed(true));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();;
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            double lowerBound = 0;
            double upperBound = 0;
            if (!pathFormula.hasDiscreteTimeBound()) {
                std::pair<double, double> const& intervalBounds =  pathFormula.getIntervalBounds();
                lowerBound = intervalBounds.first;
                upperBound = intervalBounds.second;
            } else {
                upperBound = pathFormula.getDiscreteTimeBound();
            }
            
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeBoundedUntilProbabilities(this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), this->getModel().getExitRateVector(), checkTask.isQualitativeSet(), lowerBound, upperBound, *linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeNextProbabilities(CheckTask<storm::logic::NextFormula> const& checkTask) {
            storm::logic::NextFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeNextProbabilities(this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), subResult.getTruthValuesVector(), *linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeUntilProbabilities(this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRateVector(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeInstantaneousRewards(CheckTask<storm::logic::InstantaneousRewardFormula> const& checkTask) {
            storm::logic::InstantaneousRewardFormula const& rewardPathFormula = checkTask.getFormula();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeInstantaneousRewards(this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getContinuousTimeBound(), *linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
                
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeCumulativeRewards(CheckTask<storm::logic::CumulativeRewardFormula> const& checkTask) {
            storm::logic::CumulativeRewardFormula const& rewardPathFormula = checkTask.getFormula();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeCumulativeRewards(this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getContinuousTimeBound(), *linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
                
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeReachabilityRewards(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(eventuallyFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeReachabilityRewards(this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRateVector(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula> const& checkTask) {
            storm::logic::StateFormula const& stateFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(stateFormula);
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            
            storm::storage::SparseMatrix<ValueType> probabilityMatrix = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeProbabilityMatrix(this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector());
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeLongRunAverageProbabilities(probabilityMatrix, subResult.getTruthValuesVector(), &this->getModel().getExitRateVector(), checkTask.isQualitativeSet(), *linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }

        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeExpectedTimes(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(eventuallyFormula.getSubformula());
            ExplicitQualitativeCheckResult& subResult = subResultPointer->asExplicitQualitativeCheckResult();

            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeExpectedTimes(this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRateVector(), this->getModel().getInitialStates(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *linearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }

        // Explicitly instantiate the model checker.
        template class SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<double>>;
        
    } // namespace modelchecker
} // namespace storm
