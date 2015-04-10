#ifndef STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_

#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/models/Dtmc.h"
#include "src/utility/solver.h"
#include "src/solver/LinearEquationSolver.h"
#include "src/storage/StronglyConnectedComponent.h"

namespace storm {
    namespace modelchecker {
        
        template<class ValueType>
        class SparseDtmcPrctlModelChecker : public SparsePropositionalModelChecker<ValueType> {
        public:
            explicit SparseDtmcPrctlModelChecker(storm::models::Dtmc<ValueType> const& model);
            explicit SparseDtmcPrctlModelChecker(storm::models::Dtmc<ValueType> const& model, std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>&& linearEquationSolver);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
			virtual std::unique_ptr<CheckResult> computeLongRunAverage(storm::logic::StateFormula const& stateFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            
        protected:
            storm::models::Dtmc<ValueType> const& getModel() const override;
            
        private:
            // The methods that perform the actual checking.
            std::vector<ValueType> computeBoundedUntilProbabilitiesHelper(storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, uint_fast64_t stepBound) const;
            std::vector<ValueType> computeNextProbabilitiesHelper(storm::storage::BitVector const& nextStates);
            std::vector<ValueType> computeUntilProbabilitiesHelper(storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative) const;
            std::vector<ValueType> computeInstantaneousRewardsHelper(uint_fast64_t stepCount) const;
            std::vector<ValueType> computeCumulativeRewardsHelper(uint_fast64_t stepBound) const;
            std::vector<ValueType> computeReachabilityRewardsHelper(storm::storage::BitVector const& targetStates, bool qualitative) const;
			std::vector<ValueType> computeLongRunAverageHelper(storm::storage::BitVector const& psiStates, bool qualitative) const;

			static ValueType computeLraForBSCC(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& goalStates, storm::storage::StronglyConnectedComponent const& bscc);

            // An object that is used for solving linear equations and performing matrix-vector multiplication.
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> linearEquationSolver;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_ */
