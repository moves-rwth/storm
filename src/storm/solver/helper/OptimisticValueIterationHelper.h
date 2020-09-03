#pragma once

#include <vector>
#include <boost/optional.hpp>

#include "storm/storage/SparseMatrix.h"

#include "storm/solver/OptimizationDirection.h"
#include "storm/solver/SolverStatus.h"
#include "storm/storage/BitVector.h"


namespace storm {
    class Environment;
    
    namespace solver {
        namespace helper {
            namespace oviinternal {
    
                template <typename ValueType>
                class UpperBoundIterator {
                public:
                    typedef uint32_t IndexType;
                    UpperBoundIterator(storm::storage::SparseMatrix<ValueType> const& matrix);
                    enum class IterateResult {
                        AlwaysHigherOrEqual,
                        AlwaysLowerOrEqual,
                        Equal,
                        Incomparable
                    };
                    
                    IterateResult iterate(std::vector<ValueType>& x, std::vector<ValueType> const& b, bool takeMinOfOldAndNew);
                    IterateResult iterate(storm::solver::OptimizationDirection const& dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, bool takeMinOfOldAndNew);
                
                private:
                    
                    template<bool HasRowGroups, storm::solver::OptimizationDirection Dir>
                    IterateResult iterateInternal(std::vector<ValueType>& x, std::vector<ValueType> const& b, bool takeMinOfOldAndNew);
                    ValueType multiplyRow(IndexType const& rowIndex, ValueType const& bi, std::vector<ValueType> const& x);
                    template<storm::solver::OptimizationDirection Dir>
                    ValueType multiplyRowGroup(IndexType const& rowGroupIndex, std::vector<ValueType> const& b, std::vector<ValueType> const& x);
                    
                    std::vector<ValueType> matrixValues;
                    std::vector<IndexType> matrixColumns;
                    std::vector<IndexType> rowIndications;
                    std::vector<uint64_t> const* rowGroupIndices;
                };
            }
            
            /*!
             * Performs Optimistic value iteration.
             * @see Hartmanns, Kaminski: Optimistic Value Iteration. https://doi.org/10.1007/978-3-030-53291-8\_26
             */
            template<typename ValueType>
            class OptimisticValueIterationHelper {
            public:
                OptimisticValueIterationHelper(storm::storage::SparseMatrix<ValueType> const& matrix);
    
                /*!
                 * Return type of value iteration callback.
                 * The first component shall be the number of performed iterations, the second component is the final convergence status.
                 */
                typedef std::pair<uint64_t, storm::solver::SolverStatus> ValueIterationReturnType;
                
                /*!
                 * Value iteration callback. Performs conventional value iteration for the given input.
                 * @pre y points to a vector that contains starting values
                 * @post y points to a vector that contains resulting values
                 * @param yPrime points to auxiliary storage
                 * @param precision is the target precision
                 * @param relative sets whether relative precision is considered
                 * @param maxI the maximal number of iterations to perform
                 */
                typedef std::function<ValueIterationReturnType(std::vector<ValueType>*& y, std::vector<ValueType>*& yPrime, ValueType const& precision, bool const& relative, uint64_t const& i, uint64_t const& maxI)> ValueIterationCallBackType;
                
                /*!
                 * Should perform a single value iteration step (using conventional value iteration).
                 * @pre y points to a vector that contains starting values
                 * @post y points to a vector that contains resulting values
                 * @param yPrime points to auxiliary storage
                 * @param currI the current iteration (can be used to display progress within the callback)
                 */
                typedef std::function<void(std::vector<ValueType>* y, std::vector<ValueType>* yPrime, uint64_t const& currI)> SingleIterationCallBackType;
                
                /*!
                 * @param env
                 * @param lowerX Needs to be some arbitrary lower bound on the actual values initially
                 * @param upperX Does not need to be an upper bound initially
                 * @param auxVector auxiliary storage (same size as lowerX and upperX)
                 * @param b the values added to each matrix row (the b in A*x+b)
                 * @param valueIterationCallback  Function that should perform standard value iteration on the input vector
                 * @param singleIterationCallback Function that should perform a single value iteration step on the input vector e.g. ( x' = min/max(A*x + b))
                 * @param dir The optimization direction
                 * @param relevantValues If given, we only check the precision at the states with the given indices.
                 * @return The status upon termination as well as the number of iterations Also, the maximum (relative/absolute) difference between lowerX and upperX will be 2*epsilon
                 * with the provided precision parameters.
                 */
                std::pair<SolverStatus, uint64_t> solveEquationsOptimisticValueIteration(Environment const& env, std::vector<ValueType>* lowerX, std::vector<ValueType>* upperX, std::vector<ValueType>* auxVector, std::vector<ValueType> const& b, ValueIterationCallBackType const& valueIterationCallback, SingleIterationCallBackType const& singleIterationCallback, bool relative, ValueType precision, uint64_t maxOverallIterations, boost::optional<storm::solver::OptimizationDirection> dir, boost::optional<storm::storage::BitVector> relevantValues = boost::none);

            private:
                oviinternal::UpperBoundIterator<ValueType> upperBoundIterator;
                
            };
        }
    }
}
