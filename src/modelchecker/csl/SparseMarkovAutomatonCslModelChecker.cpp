#include "src/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"

#include "src/modelchecker/csl/helper/SparseMarkovAutomatonCslHelper.h"

#include "src/models/sparse/StandardRewardModel.h"

#include "src/utility/macros.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        template<typename SparseMarkovAutomatonModelType>
        SparseMarkovAutomatonCslModelChecker<SparseMarkovAutomatonModelType>::SparseMarkovAutomatonCslModelChecker(SparseMarkovAutomatonModelType const& model, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType>>&& minMaxLinearEquationSolverFactory) : SparsePropositionalModelChecker<SparseMarkovAutomatonModelType>(model), minMaxLinearEquationSolverFactory(std::move(minMaxLinearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename SparseMarkovAutomatonModelType>
        SparseMarkovAutomatonCslModelChecker<SparseMarkovAutomatonModelType>::SparseMarkovAutomatonCslModelChecker(SparseMarkovAutomatonModelType const& model) : SparsePropositionalModelChecker<SparseMarkovAutomatonModelType>(model), minMaxLinearEquationSolverFactory(new storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType>()) {
            // Intentionally left empty.
        }
        
        template<typename SparseMarkovAutomatonModelType>
        bool SparseMarkovAutomatonCslModelChecker<SparseMarkovAutomatonModelType>::canHandle(storm::logic::Formula const& formula) const {
            return formula.isCslStateFormula() || formula.isCslPathFormula() || (formula.isRewardPathFormula() && formula.isReachabilityRewardFormula());
        }
        
        template<typename SparseMarkovAutomatonModelType>
        std::unique_ptr<CheckResult> SparseMarkovAutomatonCslModelChecker<SparseMarkovAutomatonModelType>::computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula> const& checkTask) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(pathFormula.getLeftSubformula().isTrueFormula(), storm::exceptions::NotImplementedException, "Only bounded properties of the form 'true U[t1, t2] phi' are currently supported.");
            STORM_LOG_THROW(this->getModel().isClosed(), storm::exceptions::InvalidPropertyException, "Unable to compute time-bounded reachability probabilities in non-closed Markov automaton.");
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();

            std::vector<ValueType> result = storm::modelchecker::helper::SparseMarkovAutomatonCslHelper<ValueType>::computeBoundedUntilProbabilities(optimalityType.get(), this->getModel().getTransitionMatrix(), this->getModel().getExitRates(), this->getModel().getMarkovianStates(), rightResult.getTruthValuesVector(), pathFormula.getIntervalBounds(), *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(result)));
        }
                
        template<typename SparseMarkovAutomatonModelType>
        std::unique_ptr<CheckResult> SparseMarkovAutomatonCslModelChecker<SparseMarkovAutomatonModelType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> result = storm::modelchecker::helper::SparseMarkovAutomatonCslHelper<ValueType>::computeUntilProbabilities(optimalityType.get(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative, *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(result)));
        }
                
        template<typename SparseMarkovAutomatonModelType>
        std::unique_ptr<CheckResult> SparseMarkovAutomatonCslModelChecker<SparseMarkovAutomatonModelType>::computeReachabilityRewards(CheckTask<storm::logic::ReachabilityRewardFormula> const& checkTask) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(this->getModel().isClosed(), storm::exceptions::InvalidPropertyException, "Unable to compute reachability rewards in non-closed Markov automaton.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(rewardPathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();

            std::vector<ValueType> result = storm::modelchecker::helper::SparseMarkovAutomatonCslHelper<ValueType>::computeReachabilityRewards(optimalityType.get(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRates(), this->getModel().getMarkovianStates(), rewardModelName ? this->getModel().getRewardModel(rewardModelName.get()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), qualitative, *minMaxLinearEquationSolverFactory);

            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(result)));
        }
                
        template<typename SparseMarkovAutomatonModelType>
        std::unique_ptr<CheckResult> SparseMarkovAutomatonCslModelChecker<SparseMarkovAutomatonModelType>::computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula> const& checkTask) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(this->getModel().isClosed(), storm::exceptions::InvalidPropertyException, "Unable to compute long-run average in non-closed Markov automaton.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(stateFormula);
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();

            std::vector<ValueType> result = storm::modelchecker::helper::SparseMarkovAutomatonCslHelper<ValueType>::computeLongRunAverageProbabilities(optimalityType.get(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRates(), this->getModel().getMarkovianStates(), subResult.getTruthValuesVector(), qualitative, *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(result)));
        }
                
        template<typename SparseMarkovAutomatonModelType>
        std::unique_ptr<CheckResult> SparseMarkovAutomatonCslModelChecker<SparseMarkovAutomatonModelType>::computeExpectedTimes(storm::logic::EventuallyFormula const& eventuallyFormula, CheckSettings<double> const& checkTask) {
            STORM_LOG_THROW(optimalityType, storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
            STORM_LOG_THROW(this->getModel().isClosed(), storm::exceptions::InvalidPropertyException, "Unable to compute expected times in non-closed Markov automaton.");
            std::unique_ptr<CheckResult> subResultPointer = this->check(eventuallyFormula.getSubformula());
            ExplicitQualitativeCheckResult& subResult = subResultPointer->asExplicitQualitativeCheckResult();

            std::vector<ValueType> result = storm::modelchecker::helper::SparseMarkovAutomatonCslHelper<ValueType>::computeExpectedTimes(optimalityType.get(), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRates(), this->getModel().getMarkovianStates(), subResult.getTruthValuesVector(), qualitative, *minMaxLinearEquationSolverFactory);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(result)));
        }
                
        template class SparseMarkovAutomatonCslModelChecker<storm::models::sparse::MarkovAutomaton<double>>;
    }
}