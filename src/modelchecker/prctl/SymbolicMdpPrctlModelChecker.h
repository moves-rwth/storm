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
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            virtual std::unique_ptr<CheckResult> computeGloballyProbabilities(storm::logic::GloballyFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            
            protected:
            storm::models::symbolic::Mdp<DdType, ValueType> const& getModel() const override;
            
            private:
            // An object that is used for retrieving linear equation solvers.
            std::unique_ptr<storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType>> linearEquationSolverFactory;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_SYMBOLICMDPPRCTLMODELCHECKER_H_ */
