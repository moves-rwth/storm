#ifndef STORM_MODELCHECKER_SYMBOLICMDPPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_SYMBOLICMDPPRCTLMODELCHECKER_H_

#include "src/modelchecker/propositional/SymbolicPropositionalModelChecker.h"

#include "src/models/symbolic/Mdp.h"

#include "src/utility/solver.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType DdType, typename ValueType>
        class SymbolicMdpPrctlModelChecker : public SymbolicPropositionalModelChecker<DdType, ValueType> {
            public:
            explicit SymbolicMdpPrctlModelChecker(storm::models::symbolic::Mdp<DdType, ValueType> const& model);
            explicit SymbolicMdpPrctlModelChecker(storm::models::symbolic::Mdp<DdType, ValueType> const& model, std::unique_ptr<storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType>>&& linearEquationSolverFactory);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(CheckTask<storm::logic::NextFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeGloballyProbabilities(CheckTask<storm::logic::GloballyFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(CheckTask<storm::logic::CumulativeRewardFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(CheckTask<storm::logic::InstantaneousRewardFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(CheckTask<storm::logic::ReachabilityRewardFormula> const& checkTask) override;
            
            protected:
            storm::models::symbolic::Mdp<DdType, ValueType> const& getModel() const override;
            
            private:
            // An object that is used for retrieving linear equation solvers.
            std::unique_ptr<storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType>> linearEquationSolverFactory;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_SYMBOLICMDPPRCTLMODELCHECKER_H_ */
