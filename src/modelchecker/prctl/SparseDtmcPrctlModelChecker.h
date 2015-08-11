#ifndef STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_

#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/models/sparse/Dtmc.h"
#include "src/utility/solver.h"
#include "src/solver/LinearEquationSolver.h"
#include "src/storage/StronglyConnectedComponent.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType DdType, typename ValueType>
        class HybridDtmcPrctlModelChecker;
        
        // Forward-declare CTMC model checker so we can make it a friend.
        template<typename SparseModelType> class SparseCtmcCslModelChecker;
        
        template<class SparseDtmcModelType>
        class SparseDtmcPrctlModelChecker : public SparsePropositionalModelChecker<SparseDtmcModelType> {
        public:
            typedef typename SparseDtmcModelType::ValueType ValueType;
            typedef typename SparseDtmcModelType::RewardModelType RewardModelType;
            
            friend class HybridDtmcPrctlModelChecker<storm::dd::DdType::CUDD, ValueType>;
            friend class SparseCtmcCslModelChecker<ValueType>;
            
            explicit SparseDtmcPrctlModelChecker(SparseDtmcModelType const& model);
            explicit SparseDtmcPrctlModelChecker(SparseDtmcModelType const& model, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>());
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>());
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>());
            virtual std::unique_ptr<CheckResult> computeLongRunAverage(storm::logic::StateFormula const& stateFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            
        private:
            // The methods that perform the actual checking.
            std::vector<ValueType> computeBoundedUntilProbabilitiesHelper(storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, uint_fast64_t stepBound) const;
            static std::vector<ValueType> computeNextProbabilitiesHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& nextStates, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
            static std::vector<ValueType> computeUntilProbabilitiesHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
            std::vector<ValueType> computeInstantaneousRewardsHelper(RewardModelType const& rewardModel, uint_fast64_t stepCount) const;
            std::vector<ValueType> computeCumulativeRewardsHelper(RewardModelType const& rewardModel, uint_fast64_t stepBound) const;
            static std::vector<ValueType> computeReachabilityRewardsHelper(RewardModelType const& rewardModel, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& targetStates, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory, bool qualitative);
            static std::vector<ValueType> computeLongRunAverageHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& psiStates, bool qualitative, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);

            // An object that is used for retrieving linear equation solvers.
            std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_ */
