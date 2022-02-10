#pragma once
#include "storm/modelchecker/helper/infinitehorizon/SparseInfiniteHorizonHelper.h"

namespace storm {

namespace modelchecker {
namespace helper {

/*!
 * Helper class for model checking queries that depend on the long run behavior of the (nondeterministic) system.
 * @tparam ValueType the type a value can have
 */
template<typename ValueType>
class SparseDeterministicInfiniteHorizonHelper : public SparseInfiniteHorizonHelper<ValueType, false> {
   public:
    /*!
     * Function mapping from indices to values
     */
    typedef typename SparseInfiniteHorizonHelper<ValueType, true>::ValueGetter ValueGetter;

    /*!
     * Initializes the helper for a discrete time model (i.e. DTMC)
     */
    SparseDeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix);

    /*!
     * Initializes the helper for a continuous time model (i.e. CTMC)
     * @note The transition matrix shall be probabilistic (i.e. the rows sum up to one)
     */
    SparseDeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates);

    /*!
     * @param stateValuesGetter a function returning a value for a given state index
     * @param actionValuesGetter a function returning a value for a given (global) choice index
     * @return the (unique) optimal LRA value for the given component.
     * @post if scheduler production is enabled and Nondeterministic is true, getProducedOptimalChoices() contains choices for the states of the given component
     * which yield the returned LRA value. Choices for states outside of the component are not affected.
     */
    virtual ValueType computeLraForComponent(Environment const& env, ValueGetter const& stateValuesGetter, ValueGetter const& actionValuesGetter,
                                             storm::storage::StronglyConnectedComponent const& component) override;

    /*!
     * Computes the long run average state distribution, i.e., a vector that assigns for each state s the average fraction of the time we spent in s,
     * assuming an infinite run.
     * Note that all the probability muss will be in bottom SCCs.
     * If there are multiple bottom SCCs, the value will depend on the initial state.
     * It is also possible to provide a distribution over initial states.
     * However, the standard treatment of multiple initial states is not possible here, as this is not defined then.
     * @throws InvalidOperationException if no initial state or distribution is provided and the model has multiple BSCCs.
     */
    std::vector<ValueType> computeLongRunAverageStateDistribution(Environment const& env);
    std::vector<ValueType> computeLongRunAverageStateDistribution(Environment const& env, uint64_t const& initialState);
    std::vector<ValueType> computeLongRunAverageStateDistribution(Environment const& env, ValueGetter const& initialDistributionGetter);

   protected:
    virtual void createDecomposition() override;

    /*!
     * Computes for each BSCC the probability to reach that SCC assuming the given distribution over initial states.
     */
    std::vector<ValueType> computeBsccReachabilityProbabilities(Environment const& env, ValueGetter const& initialDistributionGetter);

    /*!
     * Computes the long run average (steady state) distribution for the given BSCC.
     */
    std::vector<ValueType> computeSteadyStateDistrForBscc(Environment const& env, storm::storage::StronglyConnectedComponent const& bscc);

    std::pair<bool, ValueType> computeLraForTrivialBscc(Environment const& env, ValueGetter const& stateValuesGetter, ValueGetter const& actionValuesGetter,
                                                        storm::storage::StronglyConnectedComponent const& bscc);

    /*!
     * As computeLraForComponent but uses value iteration as a solution method (independent of what is set in env)
     */
    ValueType computeLraForBsccVi(Environment const& env, ValueGetter const& stateValuesGetter, ValueGetter const& actionValuesGetter,
                                  storm::storage::StronglyConnectedComponent const& bscc);

    /*!
     * As computeLraForComponent but solves a linear equation system encoding gain and bias (independent of what is set in env)
     * @see Kretinsky, Meggendorfer: Efficient Strategy Iteration for Mean Payoff in Markov Decision Processes (ATVA 2017),
     * https://doi.org/10.1007/978-3-319-68167-2_25
     */
    std::pair<ValueType, std::vector<ValueType>> computeLraForBsccGainBias(Environment const& env, ValueGetter const& stateValuesGetter,
                                                                           ValueGetter const& actionValuesGetter,
                                                                           storm::storage::StronglyConnectedComponent const& bscc);

    /*!
     * As computeLraForComponent but does the computation by computing the long run average (steady state) distribution (independent of what is set in env)
     */
    std::pair<ValueType, std::vector<ValueType>> computeLraForBsccSteadyStateDistr(Environment const& env, ValueGetter const& stateValuesGetter,
                                                                                   ValueGetter const& actionValuesGetter,
                                                                                   storm::storage::StronglyConnectedComponent const& bscc);

    std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> buildSspMatrixVector(std::vector<ValueType> const& bsccLraValues,
                                                                                                    std::vector<uint64_t> const& inputStateToBsccIndexMap,
                                                                                                    storm::storage::BitVector const& statesNotInComponent,
                                                                                                    bool asEquationSystem);

    /*!
     * @return Lra values for each state
     */
    virtual std::vector<ValueType> buildAndSolveSsp(Environment const& env, std::vector<ValueType> const& mecLraValues) override;
};

}  // namespace helper
}  // namespace modelchecker
}  // namespace storm