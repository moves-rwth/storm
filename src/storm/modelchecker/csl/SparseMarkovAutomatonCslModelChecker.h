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
            
            explicit SparseMarkovAutomatonCslModelChecker(SparseMarkovAutomatonModelType const& model);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(Environment const& env, CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeTotalRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::TotalRewardFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(Environment const& env, CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityTimes(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> checkMultiObjectiveFormula(Environment const& env, CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask) override;
            
        };
    }
}

#endif /* STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_ */
