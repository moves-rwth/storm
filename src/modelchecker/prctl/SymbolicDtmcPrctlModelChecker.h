#ifndef STORM_MODELCHECKER_SYMBOLICDTMCPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_SYMBOLICDTMCPRCTLMODELCHECKER_H_

#include "src/modelchecker/propositional/SymbolicPropositionalModelChecker.h"
#include "src/models/symbolic/Dtmc.h"
#include "src/utility/solver.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType DdType, typename ValueType>
        class SymbolicDtmcPrctlModelChecker : public SymbolicPropositionalModelChecker<DdType, ValueType> {
        public:
            explicit SymbolicDtmcPrctlModelChecker(storm::models::symbolic::Dtmc<DdType, ValueType> const& model);
            explicit SymbolicDtmcPrctlModelChecker(storm::models::symbolic::Dtmc<DdType, ValueType> const& model, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType>>&& linearEquationSolverFactory);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(storm::logic::NextFormula const& pathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeGloballyProbabilities(storm::logic::GloballyFormula const& pathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) override;
            
        protected:
            storm::models::symbolic::Dtmc<DdType, ValueType> const& getModel() const override;
            
        private:
            // An object that is used for retrieving linear equation solvers.
            std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType>> linearEquationSolverFactory;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_SYMBOLICDTMCPRCTLMODELCHECKER_H_ */
