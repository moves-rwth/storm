#ifndef STORM_MODELCHECKER_SPARSE_MARKOVAUTOMATON_CSL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_SPARSE_MARKOVAUTOMATON_CSL_MODELCHECKER_HELPER_H_

#include "storm/storage/BitVector.h"
#include "storm/storage/MaximalEndComponent.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/utility/NumberTraits.h"

namespace storm {
    
    namespace modelchecker {
        namespace helper {
            
            class SparseMarkovAutomatonCslHelper {
            public:
                
                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static std::vector<ValueType> computeBoundedUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static std::vector<ValueType> computeBoundedUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType>
                static std::vector<ValueType> computeUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType, typename RewardModelType>
                static std::vector<ValueType> computeReachabilityRewards(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                
                template <typename ValueType>
                static std::vector<ValueType> computeLongRunAverageProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory, bool useLpBasedTechnique = false);
                
                template <typename ValueType, typename RewardModelType>
                static std::vector<ValueType> computeLongRunAverageRewards(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory, bool useLpBasedTechnique = false);
                
                template <typename ValueType>
                static std::vector<ValueType> computeReachabilityTimes(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
            private:
                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static void computeBoundedReachabilityProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<ValueType>& markovianNonGoalValues, std::vector<ValueType>& probabilisticNonGoalValues, ValueType delta, uint_fast64_t numberOfSteps, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                 template <typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                 static void computeBoundedReachabilityProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<ValueType>& markovianNonGoalValues, std::vector<ValueType>& probabilisticNonGoalValues, ValueType delta, uint_fast64_t numberOfSteps, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                /*!
                 * Computes the long-run average value for the given maximal end component of a Markov automaton.
                 *
                 * Implementations are based on Linear Programming (LP) and Value Iteration (VI).
                 *
                 * @param dir Sets whether the long-run average is to be minimized or maximized.
                 * @param transitionMatrix The transition matrix of the underlying Markov automaton.
                 * @param markovianStates A bit vector storing all markovian states.
                 * @param exitRateVector A vector with exit rates for all states. Exit rates of probabilistic states are
                 * assumed to be zero.
                 * @param rewardModel The considered reward model
                 * @param actionRewards The action rewards (earned instantaneously).
                 * @param mec The maximal end component to consider for computing the long-run average.
                 * @param minMaxLinearEquationSolverFactory The factory for creating MinMaxLinearEquationSolvers (if needed by the performed method
                 * @return The long-run average of being in a goal state for the given MEC.
                 */
                template <typename ValueType, typename RewardModelType>
                static ValueType computeLraForMaximalEndComponent(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory, bool useLpBasedTechnique = false);
                template <typename ValueType, typename RewardModelType>
                static ValueType computeLraForMaximalEndComponentLP(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec);
                template <typename ValueType, typename RewardModelType>
                static ValueType computeLraForMaximalEndComponentVI(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_SPARSE_MARKOVAUTOMATON_CSL_MODELCHECKER_HELPER_H_ */
