#ifndef STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_
#define STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_

#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"

#include "storm/models/sparse/MarkovAutomaton.h"

#include "storm/solver/MinMaxLinearEquationSolver.h"

namespace storm {
    namespace modelchecker {
        
        template<typename SparseMarkovAutomatonModelType>
        class SparseMarkovAutomatonCslModelChecker : public SparsePropositionalModelChecker<SparseMarkovAutomatonModelType>  {
        public:
            typedef typename SparseMarkovAutomatonModelType::ValueType ValueType;
            typedef typename SparseMarkovAutomatonModelType::RewardModelType RewardModelType;
            
            explicit SparseMarkovAutomatonCslModelChecker(SparseMarkovAutomatonModelType const& model, std::unique_ptr<storm::solver::MinMaxLinearEquationSolverFactory<ValueType>>&& minMaxLinearEquationSolver);
            explicit SparseMarkovAutomatonCslModelChecker(SparseMarkovAutomatonModelType const& model);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityTimes(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> checkMultiObjectiveFormula(CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask) override;
            
        private:
            // An object that is used for retrieving solvers for systems of linear equations that are the result of nondeterministic choices.
            std::unique_ptr<storm::solver::MinMaxLinearEquationSolverFactory<ValueType>> minMaxLinearEquationSolverFactory;
        };
    }
}

#endif /* STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_ */
