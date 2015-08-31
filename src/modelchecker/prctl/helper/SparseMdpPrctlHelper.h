#ifndef STORM_MODELCHECKER_SPARSE_MDP_PRCTL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_SPARSE_MDP_PRCTL_MODELCHECKER_HELPER_H_

#include <vector>

#include "src/storage/SparseMatrix.h"
#include "src/storage/BitVector.h"
#include "src/storage/MaximalEndComponent.h"

#include "src/utility/solver.h"

#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace models {
        namespace sparse {
            template <typename ValueType>
            class StandardRewardModel;
        }
    }
    
    namespace modelchecker {
        namespace helper {
            
            template <typename ValueType>
            class SparseMdpPrctlHelper {
            public:
                static std::vector<ValueType> computeBoundedUntilProbabilities(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, uint_fast64_t stepBound, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                static std::vector<ValueType> computeNextProbabilities(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& nextStates, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                static std::vector<ValueType> computeUntilProbabilities(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename RewardModelType>
                static std::vector<ValueType> computeInstantaneousRewards(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepCount, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename RewardModelType>
                static std::vector<ValueType> computeCumulativeRewards(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename RewardModelType>
                static std::vector<ValueType> computeReachabilityRewards(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, RewardModelType const& rewardModel, storm::storage::BitVector const& targetStates, bool qualitative, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                static std::vector<ValueType> computeReachabilityRewards(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::models::sparse::StandardRewardModel<storm::Interval> const& intervalRewardModel, storm::storage::BitVector const& targetStates, bool qualitative, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                static std::vector<ValueType> computeLongRunAverage(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& psiStates, bool qualitative, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
            private:
                static std::vector<ValueType> computeReachabilityRewardsHelper(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::function<std::vector<ValueType>(uint_fast64_t, storm::storage::SparseMatrix<ValueType> const&, storm::storage::BitVector const&)> const& totalStateRewardVectorGetter, storm::storage::BitVector const& targetStates, bool qualitative, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                static ValueType computeLraForMaximalEndComponent(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& goalStates, storm::storage::MaximalEndComponent const& mec);
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_SPARSE_MDP_PRCTL_MODELCHECKER_HELPER_H_ */