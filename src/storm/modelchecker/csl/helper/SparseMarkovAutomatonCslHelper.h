#pragma once

#include <storm/utility/numerical.h>
#include "storm/modelchecker/prctl/helper/MDPModelCheckingHelperReturnType.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/MaximalEndComponent.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/SolveGoal.h"
#include "storm/utility/NumberTraits.h"

namespace storm {
    
    class Environment;
    
    namespace modelchecker {
        namespace helper {
            
            class SparseMarkovAutomatonCslHelper {
            public:

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static std::vector<ValueType> computeBoundedUntilProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair);

                template <typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static std::vector<ValueType> computeBoundedUntilProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair);
                
                template <typename ValueType>
                static MDPSparseModelCheckingHelperReturnType<ValueType> computeUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, bool produceScheduler);
                
                template <typename ValueType, typename RewardModelType>
                static MDPSparseModelCheckingHelperReturnType<ValueType> computeTotalRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, bool produceScheduler);
                
                template <typename ValueType, typename RewardModelType>
                static MDPSparseModelCheckingHelperReturnType<ValueType> computeReachabilityRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::BitVector const& psiStates, bool produceScheduler);

                template <typename ValueType>
                static std::vector<ValueType> computeLongRunAverageProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates);
                
                template <typename ValueType, typename RewardModelType>
                static std::vector<ValueType> computeLongRunAverageRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel);
                
                template <typename ValueType>
                static MDPSparseModelCheckingHelperReturnType<ValueType> computeReachabilityTimes(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, bool produceScheduler);
                
            private:
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
                 * @return The long-run average of being in a goal state for the given MEC.
                 */
                template <typename ValueType, typename RewardModelType>
                static ValueType computeLraForMaximalEndComponent(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec);
                template <typename ValueType, typename RewardModelType>
                static ValueType computeLraForMaximalEndComponentLP(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec);
                template <typename ValueType, typename RewardModelType>
                static ValueType computeLraForMaximalEndComponentVI(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec);
                
            };
            
        }
    }
}
