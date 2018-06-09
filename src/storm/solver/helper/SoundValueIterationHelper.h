#pragma once

#include <vector>

#include "storm/solver/OptimizationDirection.h"
#include "storm/solver/TerminationCondition.h"

namespace storm {
    
    namespace storage {
        template<typename ValueType>
        class SparseMatrix;
        
        class BitVector;
    }
    
    namespace solver {
        namespace helper {
            
            
            template<typename ValueType>
            class SoundValueIterationHelper {
            public:
                
                typedef uint32_t IndexType;
                
                /*!
                 * Creates a new helper from the given data
                 */
                SoundValueIterationHelper(storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType>& x, std::vector<ValueType>& y, bool relative, ValueType const& precision);
                
                /*!
                 * Creates a helper from the given data, considering the same matrix as the given old helper
                 */
                SoundValueIterationHelper(SoundValueIterationHelper<ValueType>&& oldHelper, std::vector<ValueType>& x, std::vector<ValueType>& y, bool relative, ValueType const& precision);
                
                /*!
                 * Sets the currently known lower / upper bound
                 */
                void setLowerBound(ValueType const& value);
                void setUpperBound(ValueType const& value);
                
                void setSolutionVector();
                
                /*!
                 * Performs one iteration step with respect to the given optimization direction.
                 */
                void performIterationStep(OptimizationDirection const& dir, std::vector<ValueType> const& b);
                
                /*!
                 * Performs one iteration step, assuming that the row grouping of the initial matrix is trivial.
                 */
                void performIterationStep(std::vector<ValueType> const& b);
                
                /*!
                 * Checks for convergence and updates the known lower/upper bounds.
                 */
                bool checkConvergenceUpdateBounds(OptimizationDirection const& dir, storm::storage::BitVector const* relevantValues = nullptr);
                
                /*!
                 * Checks for convergence and updates the known lower/upper bounds, assuming that the row grouping of the initial matrix is trivial.
                 */
                bool checkConvergenceUpdateBounds(storm::storage::BitVector const* relevantValues = nullptr);
                
                /*!
                 * Checks whether the provided termination condition triggers termination
                 */
                bool checkCustomTerminationCondition(storm::solver::TerminationCondition<ValueType> const& condition);
                
            private:
                
                enum class InternalOptimizationDirection {
                    None, Minimize, Maximize
                };
                
                template<InternalOptimizationDirection dir>
                void performIterationStep(std::vector<ValueType> const& b);
                
                template<InternalOptimizationDirection dir>
                void performIterationStepUpdateDecisionValue(std::vector<ValueType> const& b);
                
                void multiplyRow(IndexType const& rowIndex, ValueType const& bi, ValueType& xi, ValueType& yi);
    
                template<InternalOptimizationDirection dir>
                bool checkConvergenceUpdateBounds(storm::storage::BitVector const* relevantValues = nullptr);
                
                bool checkConvergencePhase1();
                bool checkConvergencePhase2(storm::storage::BitVector const* relevantValues = nullptr);
                
                bool isPreciseEnough(ValueType const& xi, ValueType const& yi, ValueType const& lb, ValueType const& ub);
                
                template<InternalOptimizationDirection dir>
                bool preliminaryConvergenceCheck(ValueType& lowerBoundCandidate, ValueType& upperBoundCandidate);
                
                template<InternalOptimizationDirection dir>
                void updateLowerUpperBound(ValueType& lowerBoundCandidate, ValueType& upperBoundCandidate);
                
                template<InternalOptimizationDirection dir>
                void checkIfDecisionValueBlocks();
                
                // Auxiliary helper functions to avoid case distinctions due to different optimization directions
                template<InternalOptimizationDirection dir>
                inline bool better(ValueType const& val1, ValueType const& val2) {
                    return (dir == InternalOptimizationDirection::Maximize) ? val1 > val2 : val1 < val2;
                }
                template<InternalOptimizationDirection dir>
                inline ValueType& getPrimaryBound() {
                    return (dir == InternalOptimizationDirection::Maximize) ? upperBound : lowerBound;
                }
                template<InternalOptimizationDirection dir>
                inline bool& hasPrimaryBound() {
                    return (dir == InternalOptimizationDirection::Maximize) ? hasUpperBound : hasLowerBound;
                }
                template<InternalOptimizationDirection dir>
                inline ValueType& getSecondaryBound() {
                    return (dir == InternalOptimizationDirection::Maximize) ? lowerBound : upperBound;
                }
                template<InternalOptimizationDirection dir>
                inline uint64_t& getPrimaryIndex() {
                    return (dir == InternalOptimizationDirection::Maximize) ? maxIndex : minIndex;
                }
                template<InternalOptimizationDirection dir>
                inline uint64_t& getSecondaryIndex() {
                    return (dir == InternalOptimizationDirection::Maximize) ? minIndex : maxIndex;
                }
                
                std::vector<ValueType>& x;
                std::vector<ValueType>& y;
                std::vector<ValueType> xTmp, yTmp;
                
                ValueType lowerBound, upperBound, decisionValue;
                bool hasLowerBound, hasUpperBound, hasDecisionValue;
                bool convergencePhase1;
                bool decisionValueBlocks;
                uint64_t firstIndexViolatingConvergence;
                uint64_t minIndex, maxIndex;
                
                bool relative;
                ValueType precision;
                
                IndexType numRows;
                std::vector<ValueType> matrixValues;
                std::vector<IndexType> matrixColumns;
                std::vector<IndexType> rowIndications;
                std::vector<uint_fast64_t> const* rowGroupIndices;
            };
            
        }
    }
}

