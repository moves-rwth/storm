#ifndef STORM_MODELCHECKER_HYBRIDCTMCCSLMODELCHECKER_H_
#define STORM_MODELCHECKER_HYBRIDCTMCCSLMODELCHECKER_H_

#include "src/modelchecker/propositional/SymbolicPropositionalModelChecker.h"

#include "src/models/symbolic/Ctmc.h"

#include "src/utility/solver.h"

namespace storm {
    namespace modelchecker {
        
        template<storm::dd::DdType DdType, class ValueType>
        class HybridCtmcCslModelChecker : public SymbolicPropositionalModelChecker<DdType, ValueType> {
        public:
            explicit HybridCtmcCslModelChecker(storm::models::symbolic::Ctmc<DdType, ValueType> const& model);
            explicit HybridCtmcCslModelChecker(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverage(storm::logic::StateFormula const& stateFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;

        protected:
            storm::models::symbolic::Ctmc<DdType, ValueType> const& getModel() const override;
            
        private:
            // An object that is used for solving linear equations and performing matrix-vector multiplication.
            std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_HYBRIDCTMCCSLMODELCHECKER_H_ */
