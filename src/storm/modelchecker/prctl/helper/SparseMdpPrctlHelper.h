#ifndef STORM_MODELCHECKER_SPARSE_MDP_PRCTL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_SPARSE_MDP_PRCTL_MODELCHECKER_HELPER_H_

#include <vector>

#include "storm/modelchecker/hints/ModelCheckerHint.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/MaximalEndComponent.h"
#include "storm/modelchecker/multiobjective/rewardbounded/MultiDimensionalRewardUnfolding.h"
#include "MDPModelCheckingHelperReturnType.h"

#include "storm/utility/solver.h"
#include "storm/solver/SolveGoal.h"

#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
    namespace storage {
        class BitVector;
    }
    
    namespace models {
        namespace sparse {
            template <typename ValueType>
            class StandardRewardModel;
        }
    }
    
    namespace modelchecker {
        class CheckResult;
        
        namespace helper {
            
            template <typename ValueType>
            class SparseMdpPrctlHelper {
            public:
                static std::vector<ValueType> computeStepBoundedUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, uint_fast64_t stepBound, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory, ModelCheckerHint const& hint = ModelCheckerHint());
                
                static std::map<storm::storage::sparse::state_type, ValueType> computeNonTrivialBoundedUntilProbabilities(OptimizationDirection dir, storm::modelchecker::multiobjective::MultiDimensionalRewardUnfolding<ValueType, true>& rewardUnfolding, storm::storage::BitVector const& initialStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                static std::vector<ValueType> computeNextProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& nextStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                static MDPSparseModelCheckingHelperReturnType<ValueType> computeUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, bool produceScheduler, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory, ModelCheckerHint const& hint = ModelCheckerHint());

                static std::pair<boost::optional<std::vector<ValueType>>, boost::optional<std::vector<uint_fast64_t>>> extractHintInformationForMaybeStates(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& maybeStates, boost::optional<storm::storage::BitVector> const& selectedChoices, ModelCheckerHint const& hint, bool skipECWithinMaybeStatesCheck);
                
                static std::pair<std::vector<ValueType>, boost::optional<std::vector<uint_fast64_t>>> computeValuesOnlyMaybeStates(storm::solver::SolveGoal const& goal, storm::storage::SparseMatrix<ValueType> const& submatrix, std::vector<ValueType> const& b, bool produceScheduler, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory, boost::optional<std::vector<ValueType>>&& hintValues = boost::none, boost::optional<std::vector<uint_fast64_t>>&& hintChoices = boost::none, boost::optional<ValueType> const& lowerResultBound = boost::none, boost::optional<ValueType> const& upperResultBound = boost::none);
                
                static MDPSparseModelCheckingHelperReturnType<ValueType> computeUntilProbabilities(storm::solver::SolveGoal const& goal, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, bool produceScheduler, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory, ModelCheckerHint const& hint = ModelCheckerHint());
                
                static std::vector<ValueType> computeGloballyProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& psiStates, bool qualitative, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory, bool useMecBasedTechnique = false);
                
                template<typename RewardModelType>
                static std::vector<ValueType> computeInstantaneousRewards(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepCount, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template<typename RewardModelType>
                static std::vector<ValueType> computeCumulativeRewards(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template<typename RewardModelType>
                static MDPSparseModelCheckingHelperReturnType<ValueType> computeReachabilityRewards(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, RewardModelType const& rewardModel, storm::storage::BitVector const& targetStates, bool qualitative, bool produceScheduler, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory, ModelCheckerHint const& hint = ModelCheckerHint());
                
                template<typename RewardModelType>
                static MDPSparseModelCheckingHelperReturnType<ValueType> computeReachabilityRewards(storm::solver::SolveGoal const& goal, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, RewardModelType const& rewardModel, storm::storage::BitVector const& targetStates, bool qualitative, bool produceScheduler, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory, ModelCheckerHint const& hint = ModelCheckerHint());

#ifdef STORM_HAVE_CARL
                static std::vector<ValueType> computeReachabilityRewards(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::models::sparse::StandardRewardModel<storm::Interval> const& intervalRewardModel, bool lowerBoundOfIntervals, storm::storage::BitVector const& targetStates, bool qualitative, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
#endif
                
                static std::vector<ValueType> computeLongRunAverageProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                
                template<typename RewardModelType>
                static std::vector<ValueType> computeLongRunAverageRewards(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, RewardModelType const& rewardModel, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                static std::unique_ptr<CheckResult> computeConditionalProbabilities(OptimizationDirection dir, storm::storage::sparse::state_type initialState, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& targetStates, storm::storage::BitVector const& conditionStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
            private:
                static MDPSparseModelCheckingHelperReturnType<ValueType> computeReachabilityRewardsHelper(storm::solver::SolveGoal const& goal, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::function<std::vector<ValueType>(uint_fast64_t, storm::storage::SparseMatrix<ValueType> const&, storm::storage::BitVector const&)> const& totalStateRewardVectorGetter, storm::storage::BitVector const& targetStates, bool qualitative, bool produceScheduler, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory, ModelCheckerHint const& hint = ModelCheckerHint());

                template<typename RewardModelType>
                static ValueType computeLraForMaximalEndComponent(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                template<typename RewardModelType>
                static ValueType computeLraForMaximalEndComponentVI(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                template<typename RewardModelType>
                static ValueType computeLraForMaximalEndComponentLP(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec);

            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_SPARSE_MDP_PRCTL_MODELCHECKER_HELPER_H_ */
