#ifndef STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_
#define STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_

#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"

#include "src/models/sparse/MarkovAutomaton.h"

#include "src/utility/solver.h"

namespace storm {
    namespace modelchecker {
        
        template<typename SparseMarkovAutomatonModelType>
        class SparseMarkovAutomatonCslModelChecker : public SparsePropositionalModelChecker<SparseMarkovAutomatonModelType>  {
        public:
            typedef typename SparseMarkovAutomatonModelType::ValueType ValueType;
            typedef typename SparseMarkovAutomatonModelType::RewardModelType RewardModelType;
            
            explicit SparseMarkovAutomatonCslModelChecker(SparseMarkovAutomatonModelType const& model, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType>>&& minMaxLinearEquationSolver);
            explicit SparseMarkovAutomatonCslModelChecker(SparseMarkovAutomatonModelType const& model);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(CheckTask<storm::logic::ReachabilityRewardFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeExpectedTimes(CheckTask<storm::logic::EventuallyFormula> const& checkTask) override;
            
        private:
            // An object that is used for retrieving solvers for systems of linear equations that are the result of nondeterministic choices.
            std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType>> minMaxLinearEquationSolverFactory;
        };
    }
}

#endif /* STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_ */
