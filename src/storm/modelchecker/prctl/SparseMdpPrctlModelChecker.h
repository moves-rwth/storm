#ifndef STORM_MODELCHECKER_SPARSEMDPPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_SPARSEMDPPRCTLMODELCHECKER_H_

#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"

namespace storm {
    
    class Environment;
    
    namespace modelchecker {
        template<class SparseMdpModelType>
        class SparseMdpPrctlModelChecker : public SparsePropositionalModelChecker<SparseMdpModelType> {
        public:
            typedef typename SparseMdpModelType::ValueType ValueType;
            typedef typename SparseMdpModelType::RewardModelType RewardModelType;
            
            explicit SparseMdpPrctlModelChecker(SparseMdpModelType const& model);
            explicit SparseMdpPrctlModelChecker(SparseMdpModelType const& model, std::unique_ptr<storm::solver::MinMaxLinearEquationSolverFactory<ValueType>>&& MinMaxLinearEquationSolverFactory);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(Environment const& env, CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(Environment const& env, CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeGloballyProbabilities(Environment const& env, CheckTask<storm::logic::GloballyFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeConditionalProbabilities(Environment const& env, CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(Environment const& env, CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> checkMultiObjectiveFormula(Environment const& env, CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask) override;
            
        private:
            // An object that is used for retrieving solvers for systems of linear equations that are the result of nondeterministic choices.
            std::unique_ptr<storm::solver::MinMaxLinearEquationSolverFactory<ValueType>> minMaxLinearEquationSolverFactory;
        };
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_SPARSEMDPPRCTLMODELCHECKER_H_ */
