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
                static std::vector<ValueType> computeLongRunAverageProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
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
                 * @param dir Sets whether the long-run average is to be minimized or maximized.
                 * @param transitionMatrix The transition matrix of the underlying Markov automaton.
                 * @param markovianStates A bit vector storing all markovian states.
                 * @param exitRateVector A vector with exit rates for all states. Exit rates of probabilistic states are
                 * assumed to be zero.
                 * @param goalStates A bit vector indicating which states are to be considered as goal states.
                 * @param mec The maximal end component to consider for computing the long-run average.
                 * @return The long-run average of being in a goal state for the given MEC.
                 */
                template <typename ValueType>
                static ValueType computeLraForMaximalEndComponent(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& goalStates, storm::storage::MaximalEndComponent const& mec);
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_SPARSE_MARKOVAUTOMATON_CSL_MODELCHECKER_HELPER_H_ */
