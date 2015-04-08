#ifndef STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_
#define STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_

#include "src/modelchecker/AbstractModelChecker.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/storage/MaximalEndComponentDecomposition.h"
#include "src/solver/MinMaxLinearEquationSolver.h"
#include "src/utility/solver.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ValueType>
        class SparseMarkovAutomatonCslModelChecker : public AbstractModelChecker {
        public:
            explicit SparseMarkovAutomatonCslModelChecker(storm::models::sparse::MarkovAutomaton<ValueType> const& model, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType>>&& MinMaxLinearEquationSolver);
            explicit SparseMarkovAutomatonCslModelChecker(storm::models::sparse::MarkovAutomaton<ValueType> const& model);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverage(storm::logic::StateFormula const& stateFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeExpectedTimes(storm::logic::EventuallyFormula const& eventuallyFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>());
            virtual std::unique_ptr<CheckResult> checkBooleanLiteralFormula(storm::logic::BooleanLiteralFormula const& stateFormula) override;
            virtual std::unique_ptr<CheckResult> checkAtomicLabelFormula(storm::logic::AtomicLabelFormula const& stateFormula) override;
            
        private:
            // The methods that perform the actual checking.
            std::vector<ValueType> computeBoundedUntilProbabilitiesHelper(bool minimize, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair) const;
            static void computeBoundedReachabilityProbabilities(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<ValueType>& markovianNonGoalValues, std::vector<ValueType>& probabilisticNonGoalValues, ValueType delta, uint_fast64_t numberOfSteps, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& MinMaxLinearEquationSolverFactory);
            std::vector<ValueType> computeUntilProbabilitiesHelper(bool minimize, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative) const;
            std::vector<ValueType> computeReachabilityRewardsHelper(bool minimize, storm::storage::BitVector const& psiStates, bool qualitative) const;
            std::vector<ValueType> computeLongRunAverageHelper(bool minimize, storm::storage::BitVector const& psiStates, bool qualitative) const;
            std::vector<ValueType> computeExpectedTimesHelper(bool minimize, storm::storage::BitVector const& psiStates, bool qualitative) const;

            /*!
             * Computes the long-run average value for the given maximal end component of a Markov automaton.
             *
             * @param minimize Sets whether the long-run average is to be minimized or maximized.
             * @param transitionMatrix The transition matrix of the underlying Markov automaton.
             * @param nondeterministicChoiceIndices A vector indicating at which row the choice of a given state begins.
             * @param markovianStates A bit vector storing all markovian states.
             * @param exitRates A vector with exit rates for all states. Exit rates of probabilistic states are assumed to be zero.
             * @param goalStates A bit vector indicating which states are to be considered as goal states.
             * @param mec The maximal end component to consider for computing the long-run average.
             * @param mecIndex The index of the MEC.
             * @return The long-run average of being in a goal state for the given MEC.
             */
            static ValueType computeLraForMaximalEndComponent(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, storm::storage::BitVector const& markovianStates, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::MaximalEndComponent const& mec, uint_fast64_t mecIndex = 0);
            
            /*!
             * Computes the expected reward that is gained from each state before entering any of the goal states.
             *
             * @param minimize Indicates whether minimal or maximal rewards are to be computed.
             * @param goalStates The goal states that define until which point rewards are gained.
             * @param stateRewards A vector that defines the reward gained in each state. For probabilistic states, this is an instantaneous reward
             * that is fully gained and for Markovian states the actually gained reward is dependent on the expected time to stay in the
             * state, i.e. it is gouverned by the exit rate of the state.
             * @return A vector that contains the expected reward for each state of the model.
             */
            std::vector<ValueType> computeExpectedRewards(bool minimize, storm::storage::BitVector const& goalStates, std::vector<ValueType> const& stateRewards) const;
            
            // The model this model checker is supposed to analyze.
            storm::models::sparse::MarkovAutomaton<ValueType> const& model;
            
            // An object that is used for retrieving solvers for systems of linear equations that are the result of nondeterministic choices.
            std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType>> MinMaxLinearEquationSolverFactory;
        };
    }
}

#endif /* STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_ */
