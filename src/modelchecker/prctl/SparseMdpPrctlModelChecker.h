#ifndef STORM_MODELCHECKER_SPARSEMDPPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_SPARSEMDPPRCTLMODELCHECKER_H_

#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/models/sparse/Mdp.h"
#include "src/utility/solver.h"
#include "src/solver/MinMaxLinearEquationSolver.h"
#include "src/storage/MaximalEndComponent.h"


namespace storm {
    namespace counterexamples {
        template<typename ValueType>
        class SMTMinimalCommandSetGenerator;
        
        template<typename ValueType>
        class MILPMinimalLabelSetGenerator;
    }
    
    namespace modelchecker {
        
        // Forward-declare other model checkers to make them friend classes.
        template<typename SparseMarkovAutomatonModelType>
        class SparseMarkovAutomatonCslModelChecker;
        
        template<class SparseMdpModelType>
        class SparseMdpPrctlModelChecker : public SparsePropositionalModelChecker<SparseMdpModelType> {
        public:
            typedef typename SparseMdpModelType::ValueType ValueType;
            typedef typename SparseMdpModelType::RewardModelType RewardModelType;
            
            friend class SparseMarkovAutomatonCslModelChecker<SparseMdpModelType>;
            friend class storm::counterexamples::SMTMinimalCommandSetGenerator<ValueType>;
            friend class storm::counterexamples::MILPMinimalLabelSetGenerator<ValueType>;
            
            explicit SparseMdpPrctlModelChecker(SparseMdpModelType const& model);
            explicit SparseMdpPrctlModelChecker(SparseMdpModelType const& model, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType>>&& MinMaxLinearEquationSolverFactory);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
			virtual std::unique_ptr<CheckResult> computeLongRunAverage(storm::logic::StateFormula const& stateFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            
        private:
            // The methods that perform the actual checking.
            std::vector<ValueType> computeBoundedUntilProbabilitiesHelper(bool minimize, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, uint_fast64_t stepBound) const;
            std::vector<ValueType> computeNextProbabilitiesHelper(bool minimize, storm::storage::BitVector const& nextStates);
            std::vector<ValueType> computeUntilProbabilitiesHelper(bool minimize, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative) const;
            static std::vector<ValueType> computeUntilProbabilitiesHelper(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& MinMaxLinearEquationSolverFactory, bool qualitative);
            std::vector<ValueType> computeInstantaneousRewardsHelper(RewardModelType const& rewardModel, bool minimize, uint_fast64_t stepCount) const;
            std::vector<ValueType> computeCumulativeRewardsHelper(RewardModelType const& rewardModel, bool minimize, uint_fast64_t stepBound) const;
            std::vector<ValueType> computeReachabilityRewardsHelper(RewardModelType const& rewardModel, bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& targetStates, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& MinMaxLinearEquationSolverFactory, bool qualitative) const;
			std::vector<ValueType> computeLongRunAverageHelper(bool minimize, storm::storage::BitVector const& psiStates, bool qualitative) const;
            
			static ValueType computeLraForMaximalEndComponent(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& goalStates, storm::storage::MaximalEndComponent const& mec);

            // An object that is used for retrieving solvers for systems of linear equations that are the result of nondeterministic choices.
            std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType>> MinMaxLinearEquationSolverFactory;
        };
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_SPARSEMDPPRCTLMODELCHECKER_H_ */
