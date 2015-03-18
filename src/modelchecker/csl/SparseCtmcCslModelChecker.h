#ifndef STORM_MODELCHECKER_SPARSECTMCCSLMODELCHECKER_H_
#define STORM_MODELCHECKER_SPARSECTMCCSLMODELCHECKER_H_

#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/models/Ctmc.h"
#include "src/solver/LinearEquationSolver.h"

namespace storm {
    namespace modelchecker {
        
        template<class ValueType>
        class SparseCtmcCslModelChecker : public SparsePropositionalModelChecker<ValueType> {
        public:
            explicit SparseCtmcCslModelChecker(storm::models::Ctmc<ValueType> const& model);
            explicit SparseCtmcCslModelChecker(storm::models::Ctmc<ValueType> const& model, std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>&& linearEquationSolver);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
//            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
//            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
//            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            
        protected:
            storm::models::Ctmc<ValueType> const& getModel() const override;
            
        private:
            // The methods that perform the actual checking.
            std::vector<ValueType> computeBoundedUntilProbabilitiesHelper(storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, double lowerBound, double upperBound) const;
            std::vector<ValueType> computeUntilProbabilitiesHelper(storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative) const;
//            std::vector<ValueType> computeInstantaneousRewardsHelper(uint_fast64_t stepCount) const;
//            std::vector<ValueType> computeCumulativeRewardsHelper(uint_fast64_t stepBound) const;
//            std::vector<ValueType> computeReachabilityRewardsHelper(storm::storage::BitVector const& targetStates, bool qualitative) const;
            
            // An object that is used for solving linear equations and performing matrix-vector multiplication.
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> linearEquationSolver;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_SPARSECTMCCSLMODELCHECKER_H_ */
