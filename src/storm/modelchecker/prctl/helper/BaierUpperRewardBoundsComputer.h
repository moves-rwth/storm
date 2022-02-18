#pragma once

#include <vector>

namespace storm {
namespace storage {
template<typename ValueType>
class SparseMatrix;
}

namespace modelchecker {
namespace helper {

template<typename ValueType>
class BaierUpperRewardBoundsComputer {
   public:
    /*!
     * Creates an object that can compute upper bounds on the *maximal* expected rewards for the provided MDP.
     * @see http://doi.org/10.1007/978-3-319-63387-9_8
     * @param transitionMatrix The matrix defining the transitions of the system without the transitions
     * that lead directly to the goal state.
     * @param rewards The rewards of each choice.
     * @param oneStepTargetProbabilities For each choice the probability to go to a goal state in one step.
     */
    BaierUpperRewardBoundsComputer(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& rewards,
                                   std::vector<ValueType> const& oneStepTargetProbabilities);

    /*!
     * Computes an upper bound on the expected rewards.
     */
    ValueType computeUpperBound();

    /*!
     * Computes for each state an upper bound for the maximal expected times each state is visited.
     * @param transitionMatrix The matrix defining the transitions of the system without the transitions
     * that lead directly to the goal state.
     * @param oneStepTargetProbabilities For each choice the probability to go to a goal state in one step.
     */
    static std::vector<ValueType> computeUpperBoundOnExpectedVisitingTimes(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                           std::vector<ValueType> const& oneStepTargetProbabilities);

   private:
    storm::storage::SparseMatrix<ValueType> const& _transitionMatrix;
    std::vector<ValueType> const& _rewards;
    std::vector<ValueType> const& _oneStepTargetProbabilities;
};
}  // namespace helper
}  // namespace modelchecker
}  // namespace storm
