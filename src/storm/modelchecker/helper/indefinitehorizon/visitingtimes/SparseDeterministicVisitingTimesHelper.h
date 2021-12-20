#pragma once
#include <vector>

#include "storm/modelchecker/helper/SingleValueModelCheckerHelper.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"

namespace storm {
class Environment;

namespace modelchecker {
namespace helper {

/*!
 * Helper class for computing for each state the expected number of times to visit that state assuming a given initial distribution.
 * @tparam ValueType the type a value can have
 */
template<typename ValueType>
class SparseDeterministicVisitingTimesHelper : public SingleValueModelCheckerHelper<ValueType, storm::models::ModelRepresentation::Sparse> {
   public:
    /*!
     * Function mapping from indices to values
     */
    typedef std::function<ValueType(uint64_t)> ValueGetter;

    /*!
     * Initializes the helper for a DTMC
     */
    SparseDeterministicVisitingTimesHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix);

    /*!
     * Initializes the helper for a CTMC
     */
    SparseDeterministicVisitingTimesHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates);

    /*!
     * Provides the backward transitions that can be used during the computation.
     * Providing the backward transitions is optional. If they are not provided, they will be computed internally.
     * Be aware that this class does not take ownership, i.e. the caller has to make sure that the reference to the given matrix remains valid.
     */
    void provideBackwardTransitions(storm::storage::SparseMatrix<ValueType> const& backwardTransitions);

    /*!
     * Provides the decomposition into SCCs that can be used during the computation.
     * Providing the decomposition is optional. If it is not provided, they will be computed internally.
     * Be aware that this class does not take ownership, i.e. the caller has to make sure that the reference to the decomposition remains valid.
     * The given decomposition shall
     * * include naive SCCs,
     * * be topologically sorted, and
     * * (if sound model checking is requested) shall contain sccDepths
     */
    void provideSCCDecomposition(storm::storage::StronglyConnectedComponentDecomposition<ValueType> const& decomposition);

    /*!
     * Computes for each state the expected number of times we are visiting that state assuming the given initial state(s).
     *
     * @note In contrast to most other queries, this method assumes a uniform distribution over the provided initial state.
     *
     * @return a value for each state
     */
    std::vector<ValueType> computeExpectedVisitingTimes(Environment const& env, storm::storage::BitVector const& initialStates);

    /*!
     * Computes for each state the expected number of times we are visiting that state assuming the given initial state.
     *
     * @return a value for each state
     */
    std::vector<ValueType> computeExpectedVisitingTimes(Environment const& env, uint64_t initialState);

    /*!
     * Computes for each state the expected number of times we are visiting that state assuming the given initial state probabilities
     * @param initialStateValueGetter function that returns for each state the initial value (probability) for that state.
     * The values can actually sum up to something different than 1 but should be non-negative.
     */
    std::vector<ValueType> computeExpectedVisitingTimes(Environment const& env, ValueGetter const& initialStateValueGetter);

    /*!
     * Computes for each state the expected number of times we are visiting that state assuming the given initial state(s).
     * @pre parameter stateValues vector contains for each state the initial value (probability) for that state.
     * The values can actually sum up to something different than 1 but should be non-negative.
     * @post parameter stateValues contains the desired values
     */
    void computeExpectedVisitingTimes(Environment const& env, std::vector<ValueType>& stateValues);

   private:
    /*!
     * @return true iff this is a computation on a continuous time model (i.e. CTMC, MA)
     */
    bool isContinuousTime() const;

    /*!
     * @post _backwardTransitions points to the backward transitions
     */
    void createBackwardTransitions();

    /*!
     * @post _sccDecomposition points to an SCC decomposition
     */
    void createDecomposition(Environment const& env);

    /*!
     * @return the environment used when solving non-trivial SCCs.
     */
    storm::Environment getEnvironmentForSccSolver(storm::Environment const& env) const;

    /*!
     * Processes (bottom or non-bottom SCCs consisting of a single state). The resulting value is directly inserted into stateValues
     */
    void processSingletonScc(uint64_t sccState, std::vector<ValueType>& stateValues) const;

    /*!
     * Solves the equation system for non-trivial SCCs (i.e. non-bottom SCCs with more than 1 state).
     * @return for each state of the given SCC the expected number of times that state is visited.
     */
    std::vector<ValueType> computeValueForNonTrivialScc(storm::Environment const& env, storm::storage::BitVector const& sccAsBitVector,
                                                        std::vector<ValueType> const& stateValues) const;

    storm::storage::SparseMatrix<ValueType> const& _transitionMatrix;
    std::vector<ValueType> const* _exitRates;

    storm::storage::SparseMatrix<ValueType> const* _backwardTransitions;
    std::unique_ptr<storm::storage::SparseMatrix<ValueType>> _computedBackwardTransitions;

    storm::storage::StronglyConnectedComponentDecomposition<ValueType> const* _sccDecomposition;
    std::unique_ptr<storm::storage::StronglyConnectedComponentDecomposition<ValueType>> _computedSccDecomposition;
};

}  // namespace helper
}  // namespace modelchecker
}  // namespace storm