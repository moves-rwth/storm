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

                /*!
                 * Computes TBU according to the UnifPlus algorithm
                 *
                 * @param boundsPair With precondition that the first component is 0, the second one gives the time bound
                 * @param exitRateVector the exit-rates of the given MA
                 * @param transitionMatrix the transitions of the given MA
                 * @param markovianStates bitvector refering to the markovian states
                 * @param psiStates bitvector refering to the goal states
                 *
                 * @return the probability vector
                 *
                 */
                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static std::vector<ValueType> unifPlus(OptimizationDirection dir, std::pair<double, double> const& boundsPair, std::vector<ValueType> const& exitRateVector, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static std::vector<ValueType> computeBoundedUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static std::vector<ValueType> computeBoundedUntilProbabilitiesImca(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static std::vector<ValueType> computeBoundedUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType>
                static std::vector<ValueType> computeUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType, typename RewardModelType>
                static std::vector<ValueType> computeReachabilityRewards(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                
                template <typename ValueType>
                static std::vector<ValueType> computeLongRunAverageProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType, typename RewardModelType>
                static std::vector<ValueType> computeLongRunAverageRewards(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType>
                static std::vector<ValueType> computeReachabilityTimes(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
            private:
                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static void calculateUnifPlusVector(uint64_t k, uint64_t node, uint64_t const kind, ValueType lambda, uint64_t probSize, std::vector<std::vector<ValueType>> const& relativeReachability, OptimizationDirection dir, std::vector<std::vector<std::vector<ValueType>>>& unifVectors, storm::storage::SparseMatrix<ValueType> const& fullTransitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> const& solver);

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static void deleteProbDiagonals(storm::storage::SparseMatrix<ValueType>& transitionMatrix, storm::storage::BitVector const& markovianStates);

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

                    template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static storm::storage::BitVector identifyProbCyclesGoalStates(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,  storm::storage::BitVector const& cycleStates);


                    template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static storm::storage::BitVector identifyProbCycles(storm::storage::SparseMatrix<ValueType> const& TransitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates);
                /*!
                 * Computes the poission-distribution
                 *
                 *
                 * @param parameter lambda to use
                 * @param point i
                 * TODO: replace with Fox-glynn
                 * @return the probability
                 */
                template <typename ValueType>
                static ValueType poisson(ValueType lambda, uint64_t i);

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static uint64_t trajans(storm::storage::SparseMatrix<ValueType> const& TransitionMatrix, uint64_t node, std::vector<uint64_t>& disc, std::vector<uint64_t>& finish, uint64_t * counter);

                /*
                 * Computes vu vector according to UnifPlus
                 *
                 */
                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static void calculateVu(std::vector<std::vector<ValueType>> const& relativeReachability, OptimizationDirection dir, uint64_t k, uint64_t node, uint64_t const kind, ValueType lambda, uint64_t probSize, std::vector<std::vector<std::vector<ValueType>>>& unifVectors, storm::storage::SparseMatrix<ValueType> const& fullTransitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> const& solver);




                /*!
                 * Prints the TransitionMatrix and the vectors vd, vu, wu to the logfile
                 * TODO: delete when development is finished
                 *
                 */

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static void printTransitions(storm::storage::SparseMatrix<ValueType> const& fullTransitionMatrix, std::vector<ValueType> const& exitRateVector,  storm::storage::BitVector const& markovianStates,
                                             storm::storage::BitVector const& psiStates,  std::vector<std::vector<ValueType>> relReachability,
                                             storm::storage::BitVector const& cycleStates , storm::storage::BitVector const& cycleGoalStates ,std::vector<std::vector<std::vector<ValueType>>>& unifVectors);

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static void computeBoundedReachabilityProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<ValueType>& markovianNonGoalValues, std::vector<ValueType>& probabilisticNonGoalValues, ValueType delta, uint64_t numberOfSteps, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                 template <typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                 static void computeBoundedReachabilityProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<ValueType>& markovianNonGoalValues, std::vector<ValueType>& probabilisticNonGoalValues, ValueType delta, uint64_t numberOfSteps, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
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
                static ValueType computeLraForMaximalEndComponent(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                template <typename ValueType, typename RewardModelType>
                static ValueType computeLraForMaximalEndComponentLP(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec);
                template <typename ValueType, typename RewardModelType>
                static ValueType computeLraForMaximalEndComponentVI(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_SPARSE_MARKOVAUTOMATON_CSL_MODELCHECKER_HELPER_H_ */
