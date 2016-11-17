#ifndef STORM_MODELCHECKER_HYBRIDDTMCPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_HYBRIDDTMCPRCTLMODELCHECKER_H_

#include "src/storm/modelchecker/propositional/SymbolicPropositionalModelChecker.h"

#include "src/storm/models/symbolic/Dtmc.h"

#include "src/storm/solver/LinearEquationSolver.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ModelType>
        class HybridDtmcPrctlModelChecker : public SymbolicPropositionalModelChecker<ModelType> {
        public:
            typedef typename ModelType::ValueType ValueType;
            static const storm::dd::DdType DdType = ModelType::DdType;

            explicit HybridDtmcPrctlModelChecker(ModelType const& model);
            explicit HybridDtmcPrctlModelChecker(ModelType const& model, std::unique_ptr<storm::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeGloballyProbabilities(CheckTask<storm::logic::GloballyFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) override;

        private:
            // An object that is used for retrieving linear equation solvers.
            std::unique_ptr<storm::solver::LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_HYBRIDDTMCPRCTLMODELCHECKER_H_ */
