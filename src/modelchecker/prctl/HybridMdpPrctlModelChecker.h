#ifndef STORM_MODELCHECKER_HYBRIDMDPPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_HYBRIDMDPPRCTLMODELCHECKER_H_

#include "src/modelchecker/propositional/SymbolicPropositionalModelChecker.h"

#include "src/models/symbolic/Mdp.h"

#include "src/utility/solver.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType DdType, typename ValueType>
        class HybridMdpPrctlModelChecker : public SymbolicPropositionalModelChecker<DdType> {
        public:
            explicit HybridMdpPrctlModelChecker(storm::models::symbolic::Mdp<DdType> const& model);
            explicit HybridMdpPrctlModelChecker(storm::models::symbolic::Mdp<DdType> const& model, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            
        protected:
            storm::models::symbolic::Mdp<DdType> const& getModel() const override;
            
        private:
            // An object that is used for retrieving linear equation solvers.
            std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_HYBRIDMDPPRCTLMODELCHECKER_H_ */
