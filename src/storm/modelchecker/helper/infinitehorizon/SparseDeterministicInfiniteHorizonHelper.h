#pragma once
#include "storm/modelchecker/helper/infinitehorizon/SparseInfiniteHorizonHelper.h"


namespace storm {
    
    namespace modelchecker {
        namespace helper {
        
            /*!
             * Helper class for model checking queries that depend on the long run behavior of the (nondeterministic) system.
             * @tparam ValueType the type a value can have
             */
            template <typename ValueType>
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
                 * @post if scheduler production is enabled and Nondeterministic is true, getProducedOptimalChoices() contains choices for the states of the given component which yield the returned LRA value. Choices for states outside of the component are not affected.
                 */
                virtual ValueType computeLraForComponent(Environment const& env, ValueGetter const& stateValuesGetter,  ValueGetter const& actionValuesGetter, storm::storage::StronglyConnectedComponent const& component) override;
                
            protected:
                
                virtual void createDecomposition() override;
                
                std::pair<bool, ValueType> computeLraForTrivialBscc(Environment const& env, ValueGetter const& stateValuesGetter,  ValueGetter const& actionValuesGetter, storm::storage::StronglyConnectedComponent const& bscc);
                
                /*!
                 * As computeLraForComponent but uses value iteration as a solution method (independent of what is set in env)
                 */
                ValueType computeLraForBsccVi(Environment const& env, ValueGetter const& stateValuesGetter, ValueGetter const& actionValuesGetter, storm::storage::StronglyConnectedComponent const& bscc);
                
                /*!
                 * As computeLraForComponent but solves a linear equation system encoding gain and bias (independent of what is set in env)
                 * @see Kretinsky, Meggendorfer: Efficient Strategy Iteration for Mean Payoff in Markov Decision Processes (ATVA 2017), https://doi.org/10.1007/978-3-319-68167-2_25
                 */
                std::pair<ValueType, std::vector<ValueType>> computeLraForBsccGainBias(Environment const& env, ValueGetter const& stateValuesGetter, ValueGetter const& actionValuesGetter, storm::storage::StronglyConnectedComponent const& bscc);
                /*!
                 * As computeLraForComponent but solves a linear equation system consisting encoding the long run average (steady state) distribution (independent of what is set in env)
                 */
                std::pair<ValueType, std::vector<ValueType>> computeLraForBsccLraDistr(Environment const& env, ValueGetter const& stateValuesGetter, ValueGetter const& actionValuesGetter, storm::storage::StronglyConnectedComponent const& bscc);
  
                std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> buildSspMatrixVector(std::vector<ValueType> const& bsccLraValues, std::vector<uint64_t> const& inputStateToBsccIndexMap, storm::storage::BitVector const& statesNotInComponent, bool asEquationSystem);
                
                /*!
                 * @return Lra values for each state
                 */
                virtual std::vector<ValueType> buildAndSolveSsp(Environment const& env, std::vector<ValueType> const& mecLraValues) override;
            
            };

        
        }
    }
}