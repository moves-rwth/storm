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
            explicit SparseDtmcPrctlModelChecker(SparseDtmcModelType const& model, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(storm::logic::NextFormula const& pathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeGloballyProbabilities(storm::logic::GloballyFormula const& pathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeConditionalProbabilities(storm::logic::ConditionalPathFormula const& pathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(storm::logic::StateFormula const& stateFormula, CheckSettings<double> const& checkSettings) override;
            
        private:
            // An object that is used for retrieving linear equation solvers.
            std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_ */
