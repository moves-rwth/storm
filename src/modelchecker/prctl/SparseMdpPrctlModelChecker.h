#ifndef STORM_MODELCHECKER_SPARSEMDPPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_SPARSEMDPPRCTLMODELCHECKER_H_

#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/models/sparse/Mdp.h"
#include "src/utility/solver.h"
#include "src/solver/NondeterministicLinearEquationSolver.h"

namespace storm {
    namespace counterexamples {
        template<typename ValueType>
        class SMTMinimalCommandSetGenerator;
        
        template<typename ValueType>
        class MILPMinimalLabelSetGenerator;
    }
    
    namespace modelchecker {
        
        // Forward-declare other model checkers to make them friend classes.
        template<typename ValueType>
        class SparseMarkovAutomatonCslModelChecker;
        
        template<class ValueType>
        class SparseMdpPrctlModelChecker : public SparsePropositionalModelChecker<ValueType> {
        public:
            friend class SparseMarkovAutomatonCslModelChecker<ValueType>;
            friend class storm::counterexamples::SMTMinimalCommandSetGenerator<ValueType>;
            friend class storm::counterexamples::MILPMinimalLabelSetGenerator<ValueType>;
            
            explicit SparseMdpPrctlModelChecker(storm::models::sparse::Mdp<ValueType> const& model);
            explicit SparseMdpPrctlModelChecker(storm::models::sparse::Mdp<ValueType> const& model, std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> nondeterministicLinearEquationSolver);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            
        protected:
            storm::models::sparse::Mdp<ValueType> const& getModel() const override;
            
        private:
            // The methods that perform the actual checking.
            std::vector<ValueType> computeBoundedUntilProbabilitiesHelper(bool minimize, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, uint_fast64_t stepBound) const;
            std::vector<ValueType> computeNextProbabilitiesHelper(bool minimize, storm::storage::BitVector const& nextStates);
            std::vector<ValueType> computeUntilProbabilitiesHelper(bool minimize, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative) const;
            static std::vector<ValueType> computeUntilProbabilitiesHelper(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> nondeterministicLinearEquationSolver, bool qualitative);
            std::vector<ValueType> computeInstantaneousRewardsHelper(bool minimize, uint_fast64_t stepCount) const;
            std::vector<ValueType> computeCumulativeRewardsHelper(bool minimize, uint_fast64_t stepBound) const;
            std::vector<ValueType> computeReachabilityRewardsHelper(bool minimize, storm::storage::BitVector const& targetStates, bool qualitative) const;
            
            // A solver that is used for solving systems of linear equations that are the result of nondeterministic choices.
            std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> nondeterministicLinearEquationSolver;
        };
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_SPARSEMDPPRCTLMODELCHECKER_H_ */
