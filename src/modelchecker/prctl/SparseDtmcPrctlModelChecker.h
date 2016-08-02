#ifndef STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_

#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/models/sparse/Dtmc.h"
#include "src/utility/solver.h"
#include "src/solver/LinearEquationSolver.h"
#include "src/storage/StronglyConnectedComponent.h"

namespace storm {
    namespace modelchecker {
        template<class SparseDtmcModelType>
        class SparseDtmcPrctlModelChecker : public SparsePropositionalModelChecker<SparseDtmcModelType> {
        public:
            typedef typename SparseDtmcModelType::ValueType ValueType;
            typedef typename SparseDtmcModelType::RewardModelType RewardModelType;
                        
            explicit SparseDtmcPrctlModelChecker(SparseDtmcModelType const& model);
            explicit SparseDtmcPrctlModelChecker(SparseDtmcModelType const& model, std::unique_ptr<storm::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(CheckTask<storm::logic::NextFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeGloballyProbabilities(CheckTask<storm::logic::GloballyFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeConditionalProbabilities(CheckTask<storm::logic::ConditionalFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula> const& checkTask) override;

            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeConditionalRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::ConditionalFormula> const& checkTask) override;

        private:
            // An object that is used for retrieving linear equation solvers.
            std::unique_ptr<storm::solver::LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_ */
