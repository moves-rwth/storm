#ifndef STORM_MODELCHECKER_SPARSE_MDP_PRCTL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_SPARSE_MDP_PRCTL_MODELCHECKER_HELPER_H_

#include <vector>

#include "src/models/sparse/StandardRewardModel.h"

#include "src/storage/SparseMatrix.h"
#include "src/storage/BitVector.h"
#include "src/storage/MaximalEndComponent.h"

#include "src/utility/solver.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
            class SparseMdpPrctlHelper {
            public:
                static std::vector<ValueType> computeBoundedUntilProbabilities(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, uint_fast64_t stepBound, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                static std::vector<ValueType> computeNextProbabilities(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& nextStates, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                static std::vector<ValueType> computeUntilProbabilities(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                static std::vector<ValueType> computeUntilProbabilities(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& MinMaxLinearEquationSolverFactory, bool qualitative);
                
                static std::vector<ValueType> computeInstantaneousRewards(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, RewardModelType const& rewardModel, bool minimize, uint_fast64_t stepCount, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                static std::vector<ValueType> computeCumulativeRewards(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, RewardModelType const& rewardModel, bool minimize, uint_fast64_t stepBound, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                static std::vector<ValueType> computeReachabilityRewards(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, RewardModelType const& rewardModel, storm::storage::BitVector const& targetStates, bool qualitative, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                static std::vector<ValueType> computeLongRunAverage(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& psiStates, bool qualitative, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
            private:
                static ValueType computeLraForMaximalEndComponent(bool minimize, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& goalStates, storm::storage::MaximalEndComponent const& mec);
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_SPARSE_MDP_PRCTL_MODELCHECKER_HELPER_H_ */