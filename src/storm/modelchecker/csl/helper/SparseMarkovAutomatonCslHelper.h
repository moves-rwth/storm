#ifndef STORM_MODELCHECKER_SPARSE_MARKOVAUTOMATON_CSL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_SPARSE_MARKOVAUTOMATON_CSL_MODELCHECKER_HELPER_H_

#include <storm/utility/numerical.h>
#include "storm/storage/BitVector.h"
#include "storm/storage/MaximalEndComponent.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/utility/NumberTraits.h"

namespace storm {
    
    class Environment;
    
    namespace modelchecker {
        namespace helper {
            
            class SparseMarkovAutomatonCslHelper {
            public:

                /*!
                 * Computes time-bounded reachability according to the UnifPlus algorithm
                 *
                 * @return the probability vector
                 *
                 */
                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static std::vector<ValueType> unifPlus(Environment const& env, OptimizationDirection dir, std::pair<double, double> const& boundsPair, std::vector<ValueType> const& exitRateVector, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static std::vector<ValueType> computeBoundedUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static std::vector<ValueType> computeBoundedUntilProbabilitiesImca(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static std::vector<ValueType> computeBoundedUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType>
                static std::vector<ValueType> computeUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType, typename RewardModelType>
                static std::vector<ValueType> computeReachabilityRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                
                template <typename ValueType>
                static std::vector<ValueType> computeLongRunAverageProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType, typename RewardModelType>
                static std::vector<ValueType> computeLongRunAverageRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType>
                static std::vector<ValueType> computeReachabilityTimes(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
            private:
                /*
                 * calculating the unifVectors uv, ow according to Unif+ for MA
                 */
                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static void calculateUnifPlusVector(Environment const& env, uint64_t k, uint64_t node, uint64_t const kind, ValueType lambda, uint64_t probSize, std::vector<std::vector<ValueType>> const& relativeReachability, OptimizationDirection dir, std::vector<std::vector<std::vector<ValueType>>>& unifVectors, storm::storage::SparseMatrix<ValueType> const& fullTransitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> const& solver, storm::utility::numerical::FoxGlynnResult<ValueType> const & poisson, bool cycleFree);

                /*
                 * deleting the probabilistic Diagonals
                 */
                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static void deleteProbDiagonals(storm::storage::SparseMatrix<ValueType>& transitionMatrix, storm::storage::BitVector const& markovianStates);

                /*
                 * with having only a subset of the originalMatrix/vector, we need to transform indice
                 */
                static uint64_t transformIndice(storm::storage::BitVector const& subset, uint64_t fakeId){
                    uint64_t id =0;
                    uint64_t counter =0;
                    while(counter<=fakeId){
                        if(subset[id]){
                            counter++;
                        }
                        id++;
                    }
                    return id-1;
                }

                /*
                 * Computes vu vector according to Unif+ for MA
                 *
                 */
                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static void calculateVu(Environment const& env, std::vector<std::vector<ValueType>> const& relativeReachability, OptimizationDirection dir, uint64_t k, uint64_t node, uint64_t const kind, ValueType lambda, uint64_t probSize, std::vector<std::vector<std::vector<ValueType>>>& unifVectors, storm::storage::SparseMatrix<ValueType> const& fullTransitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> const& solver, storm::utility::numerical::FoxGlynnResult<ValueType> const & poisson, bool cycleFree);

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static void computeBoundedReachabilityProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<ValueType>& markovianNonGoalValues, std::vector<ValueType>& probabilisticNonGoalValues, ValueType delta, uint64_t numberOfSteps, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                 template <typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                 static void computeBoundedReachabilityProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<ValueType>& markovianNonGoalValues, std::vector<ValueType>& probabilisticNonGoalValues, ValueType delta, uint64_t numberOfSteps, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
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
                static ValueType computeLraForMaximalEndComponent(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                template <typename ValueType, typename RewardModelType>
                static ValueType computeLraForMaximalEndComponentLP(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec);
                template <typename ValueType, typename RewardModelType>
                static ValueType computeLraForMaximalEndComponentVI(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_SPARSE_MARKOVAUTOMATON_CSL_MODELCHECKER_HELPER_H_ */
