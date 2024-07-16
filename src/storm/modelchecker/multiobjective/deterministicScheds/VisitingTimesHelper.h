#pragma once

#include <vector>

namespace storm::storage {
class BitVector;
class MaximalEndComponent;
template<typename ValueType>
class SparseMatrix;
}  // namespace storm::storage

namespace storm::modelchecker::multiobjective {

/*!
 * Provides helper functions for computing bounds on the expected visiting times
 * @see http://doi.org/10.18154/RWTH-2023-09669 Chapter 8.2
 */
template<typename ValueType>
class VisitingTimesHelper {
   public:
    /*!
     * Computes value l in (0,1] such that for any pair of distinct states s,s' and deterministic, memoryless scheduler under which s' is reachable from s we
     * have that l is a lower bound for the probability that we reach s' from s without visiting s in between.
     * @param assumeOptimalTransitionProbabilities if true, we assume transitions with the same graph structure (except for selfloops) with uniform distribution
     */
    static ValueType computeMecTraversalLowerBound(storm::storage::MaximalEndComponent const& mec, storm::storage::SparseMatrix<ValueType> const& transitions,
                                                   bool assumeOptimalTransitionProbabilities = false);

    /*!
     * Computes an upper bound for the largest finite expected number of times a state s in the given MEC is visited without leaving the MEC (under all
     * deterministic, memoryless scheduler)
     * @param assumeOptimalTransitionProbabilities if true, we assume transitions with the same graph structure (except for selfloops and MEC exiting choices)
     with uniform distribution

     */
    static ValueType computeMecVisitsUpperBound(storm::storage::MaximalEndComponent const& mec, storm::storage::SparseMatrix<ValueType> const& transitions,
                                                bool assumeOptimalTransitionProbabilities = false);

    /*!
     * Computes for each state in the given subsystem an upper bound for the maximal finite expected number of visits. Assumes that there is a strategy that
     * yields finite expected number of visits for each state in the subsystem
     * @return a vector with the upper bounds. The vector has size subsystem.size() with a -1 in it wherever subsystem is false.
     */
    static std::vector<ValueType> computeUpperBoundsOnExpectedVisitingTimes(storm::storage::BitVector const& subsystem,
                                                                            storm::storage::SparseMatrix<ValueType> const& transitions,
                                                                            storm::storage::SparseMatrix<ValueType> const& backwardTransitions);
};
}  // namespace storm::modelchecker::multiobjective