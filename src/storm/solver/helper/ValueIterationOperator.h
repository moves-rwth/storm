#pragma once
#include <functional>
#include <optional>
#include <utility>
#include <vector>

#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/irange.hpp>

#include "storm/storage/sparse/StateType.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"  // TODO

namespace storm {
class Environment;

namespace storage {
template<typename T>
class SparseMatrix;
}

namespace solver::helper {

/*!
 * This class represents the Value Iteration Operator (also known as Bellman operator).
 * It is tailored for efficiency, in particular when applied multiple times.
 * The application of the operator is heavily templated so that many different flavours of value iteration and related algorithms can be implemented using this.
 * @tparam ValueType The type of the matrix entries
 * @tparam TrivialRowGrouping True iff the underlying model is deterministic
 */
template<typename ValueType, bool TrivialRowGrouping>
class ValueIterationOperator {
   public:
    using IndexType = storm::storage::sparse::state_type;

    /*!
     * Initializes this operator with the given data
     * @tparam backwards if true, we iterate backwards starting with the largest rowgroup. This often makes in place (Gauss-Seidel) iterations more efficient
     * @param matrix the transition matrix
     * @param rowGroupIndices if given, overwrites the rowGroupIndices of the matrix. Must be nullptr if TrivialRowGrouping is true
     * @note The reference to the row group indices (either of the matrix or the given pointer) must not be invalidated as long as this operator is used.
     */
    template<bool Backward = true>
    void setMatrix(storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<IndexType> const* rowGroupIndices = nullptr);

    /*!
     * Initializes this operator with the given data for forward iterations (starting with the smallest row group
     * @param matrix the transition matrix
     * @param rowGroupIndices if given, overwrites the rowGroupIndices of the matrix. Must be nullptr if TrivialRowGrouping is true
     * @note The reference to the row group indices (either of the matrix or the given pointer) must not be invalidated as long as this operator is used.
     */
    void setMatrixForwards(storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<IndexType> const* rowGroupIndices = nullptr);

    /*!
     * Initializes this operator with the given data for backward iterations (starting with the largest row group)
     * @param matrix the transition matrix
     * @param rowGroupIndices if given, overwrites the rowGroupIndices of the matrix. Must be nullptr if TrivialRowGrouping is true
     * @note The reference to the row group indices (either of the matrix or the given pointer) must not be invalidated as long as this operator is used.
     */
    void setMatrixBackwards(storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<IndexType> const* rowGroupIndices = nullptr);

    /*!
     * Applies the operator with the given operands, offsets, and backend.
     * More specifically, for each row group and for each row in a row group,
     * this multiplies the matrix row with the given input operand vector and adds the offset row entry.
     * The backend is invoked with row results and---once all rows in a group are processed---assigns the new row group value to the output operand.
     * The following backend methods are invoked:
     * * backend.startNewIteration(); at the beginning of each call to `apply`
     * * backend.firstRow(rowResult, rowGroupIndex, rowIndex); once the first row of a row group is processed
     * * backend.nextRow(rowResult, rowGroupIndex, rowIndex); for the subsequent rows of a row group (not invoked if TrivialRowGrouping)
     * * backend.applyUpdate(operandOutReference, rowGroupIndex); once all rows of a group are processed. Here, the backend can set the result value for
     *   the row group to the first argument
     * * backend.abort(); invoked after a group is processed. If this returns true, the method is aborted, even if some groups have not been processed yet
     * * backend.endOfIteration(); invoked when all groups are processed
     * * backend.converged(); invoked when abort() returns true or all groups are processed. Determines the return value of this method
     *
     * @tparam OperandType The type of input and output operand. Can be a value vector or a pair of two value vectors with one entry per group.
     *                      In the latter case, the rowResult for backend.firstRow and backend.nextRow is a pair of values and
     *                      applyUpdate gets two operandOutReference's to write the group result to.
     * @tparam OffsetType The type of row offsets. Can be a single value vector (one entry per row) or a pair of a (pointer to a) value vector and a value.
     *                      The latter case is only valid if OperandType is a pair of two value vectors.
     * @tparam BackendType The type of backend, shall implement the methods above
     * @param operandIn Input operand
     * @param operandOut Output operand
     * @param offsets Row offsets which are added to each row result
     * @param backend the backend
     * @return whatever backend.converged() returns
     *
     * @note This and other apply methods are intentionally implemented in the header file as there are potentially many different BackendTypes
     */
    template<typename OperandType, typename OffsetType, typename BackendType>
    bool apply(OperandType const& operandIn, OperandType& operandOut, OffsetType const& offsets, BackendType& backend) const {
        if (hasSkippedRows) {
            if (backwards) {
                return apply<OperandType, OffsetType, BackendType, true, true>(operandOut, operandIn, offsets, backend);
            } else {
                return apply<OperandType, OffsetType, BackendType, false, true>(operandOut, operandIn, offsets, backend);
            }
        } else {
            if (backwards) {
                return apply<OperandType, OffsetType, BackendType, true, false>(operandOut, operandIn, offsets, backend);
            } else {
                return apply<OperandType, OffsetType, BackendType, false, false>(operandOut, operandIn, offsets, backend);
            }
        }
    }

    /*!
     * Same as `apply` but with operandOut==operandIn
     */
    template<typename OperandType, typename OffsetType, typename BackendType>
    bool applyInPlace(OperandType& operand, OffsetType const& offsets, BackendType& backend) const {
        return apply(operand, operand, offsets, backend);
    }

    /*!
     * Sets rows that will be skipped when applying the operator.
     * @note each row group shall have at least one row that is not ignored
     * @param useLocalRowIndices if true, the row indices are considered to be local to the group, i.e. row i is the i'th row in the given group
     *                           if false, row indices are global, i.e. row i refers to the i'th row of the entire matrix
     * @param ignore function object that takes a row group index and a row index and returns true, iff the corresponding row shall be skipped
     */
    void setIgnoredRows(bool useLocalRowIndices, std::function<bool(IndexType, IndexType)> const& ignore);

    /*!
     * Clears all ignored rows
     */
    void unsetIgnoredRows();

    /*!
     * @return The considered row group indices
     */
    std::vector<IndexType> const& getRowGroupIndices() const;

    /*!
     * Allocates additional storage that can be used e.g. when applying the operand
     * @param size the size of the auxiliary vector
     * @param initialValue optional initial value
     * @return a reference to the auxiliary vector
     */
    std::vector<ValueType>& allocateAuxiliaryVector(uint64_t size, std::optional<ValueType> const& initialValue = {});

    /*!
     * Clears the auxiliary vector, invalidating any references to it
     */
    void freeAuxiliaryVector();

   private:
    /*!
     * Internal variant of `apply`
     * @note This and other apply methods are intentionally implemented in the header file as there are potentially many different BackendTypes
     */
    template<typename OperandType, typename OffsetType, typename BackendType, bool Backward, bool SkipIgnoredRows>
    bool apply(OperandType& operandOut, OperandType const& operandIn, OffsetType const& offsets, BackendType& backend) const {
        STORM_LOG_ASSERT(getSize(operandIn) == getSize(operandOut), "Input and Output Operands have different sizes.");
        auto const operandSize = getSize(operandIn);
        STORM_LOG_ASSERT(TrivialRowGrouping || rowGroupIndices->size() == operandSize + 1, "Dimension mismatch");
        backend.startNewIteration();
        auto matrixValueIt = matrixValues.cbegin();
        auto matrixColumnIt = matrixColumns.cbegin();
        for (auto groupIndex : indexRange<Backward>(0, operandSize)) {
            STORM_LOG_ASSERT(matrixColumnIt != matrixColumns.end(), "VI Operator in invalid state.");
            STORM_LOG_ASSERT(*matrixColumnIt >= StartOfRowIndicator, "VI Operator in invalid state.");
            //            STORM_LOG_ASSERT(matrixValueIt != matrixValues.end(), "VI Operator in invalid state.");
            if constexpr (TrivialRowGrouping) {
                backend.firstRow(applyRow(matrixColumnIt, matrixValueIt, operandIn, offsets, groupIndex), groupIndex, groupIndex);
            } else {
                IndexType rowIndex = (*rowGroupIndices)[groupIndex];
                if constexpr (SkipIgnoredRows) {
                    rowIndex += skipMultipleIgnoredRows(matrixColumnIt, matrixValueIt);
                }
                backend.firstRow(applyRow(matrixColumnIt, matrixValueIt, operandIn, offsets, rowIndex), groupIndex, rowIndex);
                while (*matrixColumnIt < StartOfRowGroupIndicator) {
                    ++rowIndex;
                    if (!SkipIgnoredRows || !skipIgnoredRow(matrixColumnIt, matrixValueIt)) {
                        backend.nextRow(applyRow(matrixColumnIt, matrixValueIt, operandIn, offsets, rowIndex), groupIndex, rowIndex);
                    }
                }
            }
            if constexpr (isPair<OperandType>::value) {
                backend.applyUpdate(operandOut.first[groupIndex], operandOut.second[groupIndex], groupIndex);
            } else {
                backend.applyUpdate(operandOut[groupIndex], groupIndex);
            }
            if (backend.abort()) {
                return backend.converged();
            }
        }
        STORM_LOG_ASSERT(matrixColumnIt + 1 == matrixColumns.cend(), "Unexpected position of matrix column iterator.");
        STORM_LOG_ASSERT(matrixValueIt == matrixValues.cend(), "Unexpected position of matrix column iterator.");
        backend.endOfIteration();
        return backend.converged();
    }

    // Auxiliary methods to deal with various OperandTypes and OffsetTypes

    template<typename OpT, typename OffT>
    OpT initializeRowRes(std::vector<OpT> const&, std::vector<OffT> const& offsets, uint64_t offsetIndex) const {
        return offsets[offsetIndex];
    }

    template<typename OpT1, typename OpT2, typename OffT>
    std::pair<OpT1, OpT2> initializeRowRes(std::pair<std::vector<OpT1>, std::vector<OpT2>> const&, std::vector<OffT> const& offsets,
                                           uint64_t offsetIndex) const {
        return {offsets[offsetIndex], offsets[offsetIndex]};
    }

    template<typename OpT1, typename OpT2, typename OffT1, typename OffT2>
    std::pair<OpT1, OpT2> initializeRowRes(std::pair<std::vector<OpT1>, std::vector<OpT2>> const&, std::pair<std::vector<OffT1> const*, OffT2> const& offsets,
                                           uint64_t offsetIndex) const {
        return {(*offsets.first)[offsetIndex], offsets.second};
    }

    /*!
     * Computes the result for a single row and advances the given iterators to the end of the row
     */
    template<typename OperandType, typename OffsetType>
    auto applyRow(std::vector<IndexType>::const_iterator& matrixColumnIt, typename std::vector<ValueType>::const_iterator& matrixValueIt,
                  OperandType const& operand, OffsetType const& offsets, uint64_t offsetIndex) const {
        STORM_LOG_ASSERT(*matrixColumnIt >= StartOfRowIndicator, "VI Operator in invalid state.");
        auto result{initializeRowRes(operand, offsets, offsetIndex)};
        for (++matrixColumnIt; *matrixColumnIt < StartOfRowIndicator; ++matrixColumnIt, ++matrixValueIt) {
            if constexpr (isPair<OperandType>::value) {
                result.first += operand.first[*matrixColumnIt] * (*matrixValueIt);
                result.second += operand.second[*matrixColumnIt] * (*matrixValueIt);
            } else {
                result += operand[*matrixColumnIt] * (*matrixValueIt);
            }
        }
        return result;
    }

    // Auxiliary helpers used for metaprogramming
    template<bool Backward>
    auto indexRange(IndexType start, IndexType end) const {
        if constexpr (Backward) {
            return boost::adaptors::reverse(boost::irange(start, end));
        } else {
            return boost::irange(start, end);
        }
    }

    template<typename T>
    uint64_t getSize(std::vector<T> const& vec) const {
        return vec.size();
    }

    template<typename T1, typename T2>
    uint64_t getSize(std::pair<T1, T2> const& pairOfVec) const {
        return pairOfVec.first.size();
    }

    template<typename>
    struct isPair : std::false_type {};

    template<typename T1, typename T2>
    struct isPair<std::pair<T1, T2>> : std::true_type {};

    /*!
     * Internal variant of setIgnoredRows
     */
    template<bool Backward = true>
    void setIgnoredRows(bool useLocalRowIndices, std::function<bool(IndexType, IndexType)> const& ignore);

    /*!
     * Moves the given iterator to the end of the current row
     */
    void moveToEndOfRow(std::vector<IndexType>::iterator& matrixColumnIt) const;

    /*!
     * Skips the current row, if it is ignored. Advances the iterators accordingly
     */
    bool skipIgnoredRow(std::vector<IndexType>::const_iterator& matrixColumnIt, typename std::vector<ValueType>::const_iterator& matrixValueIt) const;

    /*!
     * Skips all ignored rows, advancing the iterators to the first successor row that is not ignored
     */
    uint64_t skipMultipleIgnoredRows(std::vector<IndexType>::const_iterator& matrixColumnIt,
                                     typename std::vector<ValueType>::const_iterator& matrixValueIt) const;

    /*!
     * The non-zero matrix entries.
     */
    std::vector<ValueType> matrixValues;

    /*!
     * Row indicators and columns of the matrix entries. Has size #non-zero matrix entries + #rows + 1
     * A row indicator is an index >= 1000...000. Before and after each row there is a row indicator.
     */
    std::vector<IndexType> matrixColumns;

    /*!
     * Row group indices as in the sparse matrix (even if the matrix is set in backwards order, this vector will not be reversed)
     */
    std::vector<IndexType> const* rowGroupIndices;

    /*!
     * True iff the matrix was set in backward orders
     */
    bool backwards{true};

    /*!
     * True iff there are some ignored rows
     */
    bool hasSkippedRows{false};

    /*!
     * Storage for the auxiliary vector
     */
    std::vector<ValueType> auxiliaryVector;

    /*!
     * True, if an auxiliary vector exists
     */
    bool auxiliaryVectorUsedExternally{false};

    /*!
     * Bitmask that indicates the start of a row in the 'matrixColumns' vector
     */
    IndexType const StartOfRowIndicator = 1ull << 63;  // 10000..0

    /*!
     * Bitmask that indicates the start of a row group in the 'matrixColumns' vector
     */
    IndexType const StartOfRowGroupIndicator = StartOfRowIndicator + (1ull << 62);  // 11000..0

    /*!
     * Ignored rows are encoded by adding the number of skipped entries to the row indicator. This Bitmask helps to get the number of skipped entries
     */
    IndexType const SkipNumEntriesMask = ~StartOfRowGroupIndicator;  // 00111..1
};

}  // namespace solver::helper
}  // namespace storm
