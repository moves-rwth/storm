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
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(storm::logic::StateFormula const& stateFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeExpectedTimes(storm::logic::EventuallyFormula const& eventuallyFormula, CheckSettings<double> const& checkSettings) override;
            
        private:
            // An object that is used for retrieving solvers for systems of linear equations that are the result of nondeterministic choices.
            std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType>> minMaxLinearEquationSolverFactory;
        };
    }
}

#endif /* STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_ */
