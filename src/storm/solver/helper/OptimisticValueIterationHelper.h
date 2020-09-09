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
                class IterationHelper {
                public:
                    typedef uint32_t IndexType;
                    IterationHelper(storm::storage::SparseMatrix<ValueType> const& matrix);
                    enum class IterateResult {
                        AlwaysHigherOrEqual,
                        AlwaysLowerOrEqual,
                        Equal,
                        Incomparable
                    };
                    
                    /// performs a single iteration and returns the maximal difference between the iterations as well as the index where this difference happened
                    ValueType singleIterationWithDiff(std::vector<ValueType>& x, std::vector<ValueType> const& b, bool computeRelativeDiff);
                    ValueType singleIterationWithDiff(storm::solver::OptimizationDirection const& dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, bool computeRelativeDiff);
                    
                    /// returns the number of performed iterations
                    uint64_t repeatedIterate(std::vector<ValueType>& x, std::vector<ValueType> const& b, ValueType precision, bool relative);
                    uint64_t repeatedIterate(storm::solver::OptimizationDirection const& dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, ValueType precision, bool relative);
                    
                    /// Performs a single iteration for the upper bound
                    IterateResult iterateUpper(std::vector<ValueType>& x, std::vector<ValueType> const& b, bool takeMinOfOldAndNew);
                    IterateResult iterateUpper(storm::solver::OptimizationDirection const& dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, bool takeMinOfOldAndNew);
                
                private:
                    
                    template<bool HasRowGroups, storm::solver::OptimizationDirection Dir>
                    ValueType singleIterationWithDiffInternal(std::vector<ValueType>& x, std::vector<ValueType> const& b, bool computeRelativeDiff);
                    template<bool HasRowGroups, storm::solver::OptimizationDirection Dir>
                    uint64_t repeatedIterateInternal(std::vector<ValueType>& x, std::vector<ValueType> const& b, ValueType precision, bool relative);
                    template<bool HasRowGroups, storm::solver::OptimizationDirection Dir>
                    IterateResult iterateUpperInternal(std::vector<ValueType>& x, std::vector<ValueType> const& b, bool takeMinOfOldAndNew);
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
                 * @param env
                 * @param lowerX Needs to be some arbitrary lower bound on the actual values initially
                 * @param upperX Does not need to be an upper bound initially
                 * @param b the values added to each matrix row (the b in A*x+b)
                 * @param dir The optimization direction
                 * @param relevantValues If given, we only check the precision at the states with the given indices.
                 * @return The status upon termination as well as the number of iterations Also, the maximum (relative/absolute) difference between lowerX and upperX will be 2*epsilon
                 * with the provided precision parameters.
                 */
                std::pair<SolverStatus, uint64_t> solveEquations(Environment const& env, std::vector<ValueType>* lowerX, std::vector<ValueType>* upperX, std::vector<ValueType> const& b, bool relative, ValueType precision, uint64_t maxOverallIterations, boost::optional<storm::solver::OptimizationDirection> dir, boost::optional<storm::storage::BitVector> const& relevantValues);

            private:
                oviinternal::IterationHelper<ValueType> iterationHelper;
                
            };
        }
    }
}
