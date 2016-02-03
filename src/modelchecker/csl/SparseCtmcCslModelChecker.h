#ifndef STORM_MODELCHECKER_SPARSECTMCCSLMODELCHECKER_H_
#define STORM_MODELCHECKER_SPARSECTMCCSLMODELCHECKER_H_

#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"

#include "src/models/sparse/Ctmc.h"

#include "src/utility/solver.h"

namespace storm {
    namespace modelchecker {
        
        template<class SparseCtmcModelType>
        class SparseCtmcCslModelChecker : public SparsePropositionalModelChecker<SparseCtmcModelType> {
        public:
            typedef typename SparseCtmcModelType::ValueType ValueType;
            typedef typename SparseCtmcModelType::RewardModelType RewardModelType;
            
            explicit SparseCtmcCslModelChecker(SparseCtmcModelType const& model);
            explicit SparseCtmcCslModelChecker(SparseCtmcModelType const& model, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(storm::logic::NextFormula const& pathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(storm::logic::StateFormula const& stateFormula, CheckSettings<double> const& checkSettings) override;

        private:
            // An object that is used for solving linear equations and performing matrix-vector multiplication.
            std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_SPARSECTMCCSLMODELCHECKER_H_ */
