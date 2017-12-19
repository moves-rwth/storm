#ifndef STORM_MODELCHECKER_HYBRIDCTMCCSLMODELCHECKER_H_
#define STORM_MODELCHECKER_HYBRIDCTMCCSLMODELCHECKER_H_

#include "storm/modelchecker/propositional/SymbolicPropositionalModelChecker.h"

#include "storm/models/symbolic/Ctmc.h"

#include "storm/solver/LinearEquationSolver.h"

#include "storm/utility/NumberTraits.h"

namespace storm {
    
    namespace modelchecker {

        template<typename ModelType>
        class HybridCtmcCslModelChecker : public SymbolicPropositionalModelChecker<ModelType> {
        public:
            typedef typename ModelType::ValueType ValueType;
            static const storm::dd::DdType DdType = ModelType::DdType;

            explicit HybridCtmcCslModelChecker(ModelType const& model);
            explicit HybridCtmcCslModelChecker(ModelType const& model, std::unique_ptr<storm::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(Environment const& env, CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(Environment const& env, CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(Environment const& env, CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) override;

            virtual std::unique_ptr<CheckResult> computeLongRunAverageRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;

        private:
            template<typename CValueType = ValueType, typename std::enable_if<storm::NumberTraits<CValueType>::SupportsExponential, int>::type = 0>
            bool canHandleImplementation(CheckTask<storm::logic::Formula, CValueType> const& checkTask) const;
            
            template<typename CValueType = ValueType, typename std::enable_if<!storm::NumberTraits<CValueType>::SupportsExponential, int>::type = 0>
            bool canHandleImplementation(CheckTask<storm::logic::Formula, CValueType> const& checkTask) const;
            
            // An object that is used for solving linear equations and performing matrix-vector multiplication.
            std::unique_ptr<storm::solver::LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_HYBRIDCTMCCSLMODELCHECKER_H_ */
