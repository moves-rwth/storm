#ifndef STORM_MODELCHECKER_HYBRIDDTMCPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_HYBRIDDTMCPRCTLMODELCHECKER_H_

#include "src/modelchecker/propositional/SymbolicPropositionalModelChecker.h"

#include "src/models/symbolic/Dtmc.h"

#include "src/utility/solver.h"

namespace storm {
    namespace modelchecker {
        
        template<storm::dd::DdType DdType, typename ValueType>
        class HybridDtmcPrctlModelChecker : public SymbolicPropositionalModelChecker<DdType, ValueType> {
        public:
            explicit HybridDtmcPrctlModelChecker(storm::models::symbolic::Dtmc<DdType, ValueType> const& model);
            explicit HybridDtmcPrctlModelChecker(storm::models::symbolic::Dtmc<DdType, ValueType> const& model, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(CheckTask<storm::logic::NextFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeGloballyProbabilities(CheckTask<storm::logic::GloballyFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(CheckTask<storm::logic::CumulativeRewardFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(CheckTask<storm::logic::InstantaneousRewardFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(CheckTask<storm::logic::EventuallyFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula> const& checkTask) override;

        protected:
            storm::models::symbolic::Dtmc<DdType, ValueType> const& getModel() const override;
            
        private:
            // An object that is used for retrieving linear equation solvers.
            std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_HYBRIDDTMCPRCTLMODELCHECKER_H_ */
