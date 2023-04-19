#include "storm/solver/helper/ValueIterationOperator.h"

#include <optional>

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/storage/SparseMatrix.h"

namespace storm::solver::helper {

template<typename ValueType, bool TrivialRowGrouping>
template<bool Backward>
void ValueIterationOperator<ValueType, TrivialRowGrouping>::setMatrix(storm::storage::SparseMatrix<ValueType> const& matrix,
                                                                      std::vector<IndexType> const* rowGroupIndices) {
    if constexpr (TrivialRowGrouping) {
        STORM_LOG_ASSERT(matrix.hasTrivialRowGrouping(), "Expected a matrix with trivial row grouping");
        STORM_LOG_ASSERT(rowGroupIndices == nullptr, "Row groups given, but grouping is supposed to be trivial.");
        this->rowGroupIndices = nullptr;
    } else {
        if (rowGroupIndices) {
            this->rowGroupIndices = rowGroupIndices;
        } else {
            this->rowGroupIndices = &matrix.getRowGroupIndices();
        }
    }
    this->backwards = Backward;
    this->hasSkippedRows = false;
    auto const numRows = matrix.getRowCount();
    matrixValues.clear();
    matrixColumns.clear();
    matrixValues.reserve(matrix.getNonzeroEntryCount());
    matrixColumns.reserve(matrix.getNonzeroEntryCount() + numRows + 1);  // matrixColumns also contain indications for when a row(group) starts
    if constexpr (!TrivialRowGrouping) {
        matrixColumns.push_back(StartOfRowGroupIndicator);  // indicate start of first row(group)
        for (auto groupIndex : indexRange<Backward>(0, this->rowGroupIndices->size() - 1)) {
            STORM_LOG_ASSERT(this->rowGroupIndices->at(groupIndex) != this->rowGroupIndices->at(groupIndex + 1),
                             "There is an empty row group. This is not expected.");
            for (auto rowIndex : indexRange<false>((*this->rowGroupIndices)[groupIndex], (*this->rowGroupIndices)[groupIndex + 1])) {
                for (auto const& entry : matrix.getRow(rowIndex)) {
                    matrixValues.push_back(entry.getValue());
                    matrixColumns.push_back(entry.getColumn());
                }
                matrixColumns.push_back(StartOfRowIndicator);  // Indicate start of next row
            }
            matrixColumns.back() = StartOfRowGroupIndicator;  // This is the start of the next row group
        }
    } else {
        matrixColumns.push_back(StartOfRowIndicator);  // Indicate start of first row
        for (auto rowIndex : indexRange<Backward>(0, numRows)) {
            for (auto const& entry : matrix.getRow(rowIndex)) {
                matrixValues.push_back(entry.getValue());
                matrixColumns.push_back(entry.getColumn());
            }
            matrixColumns.push_back(StartOfRowIndicator);  // Indicate start of next row
        }
    }
}

template<typename ValueType, bool TrivialRowGrouping>
void ValueIterationOperator<ValueType, TrivialRowGrouping>::setMatrixForwards(storm::storage::SparseMatrix<ValueType> const& matrix,
                                                                              std::vector<IndexType> const* rowGroupIndices) {
    setMatrix<false>(matrix, rowGroupIndices);
}

template<typename ValueType, bool TrivialRowGrouping>
void ValueIterationOperator<ValueType, TrivialRowGrouping>::setMatrixBackwards(storm::storage::SparseMatrix<ValueType> const& matrix,
                                                                               std::vector<IndexType> const* rowGroupIndices) {
    setMatrix<true>(matrix, rowGroupIndices);
}

template<typename ValueType, bool TrivialRowGrouping>
void ValueIterationOperator<ValueType, TrivialRowGrouping>::unsetIgnoredRows() {
    for (auto& c : matrixColumns) {
        if (c >= StartOfRowIndicator) {
            c &= StartOfRowGroupIndicator;
        }
    }
    hasSkippedRows = false;
}

template<typename ValueType, bool TrivialRowGrouping>
template<bool Backward>
void ValueIterationOperator<ValueType, TrivialRowGrouping>::setIgnoredRows(bool useLocalRowIndices, std::function<bool(IndexType, IndexType)> const& ignore) {
    STORM_LOG_ASSERT(!TrivialRowGrouping, "Tried to ignroe rows but the row grouping is trivial.");
    auto colIt = matrixColumns.begin();
    for (auto groupIndex : indexRange<Backward>(0, this->rowGroupIndices->size() - 1)) {
        STORM_LOG_ASSERT(colIt != matrixColumns.end(), "VI Operator in invalid state.");
        STORM_LOG_ASSERT(*colIt >= StartOfRowGroupIndicator, "VI Operator in invalid state.");
        auto const rowIndexRange = useLocalRowIndices ? indexRange<false>(0ull, (*this->rowGroupIndices)[groupIndex + 1] - (*this->rowGroupIndices)[groupIndex])
                                                      : indexRange<false>((*this->rowGroupIndices)[groupIndex], (*this->rowGroupIndices)[groupIndex + 1]);
        for (auto const rowIndex : rowIndexRange) {
            if (!ignore(groupIndex, rowIndex)) {
                *colIt &= StartOfRowGroupIndicator;  // Clear number of skipped entries
                moveToEndOfRow(colIt);
            } else if ((*colIt & SkipNumEntriesMask) == 0) {  // i.e. should ignore but is not already ignored
                auto currColIt = colIt;
                moveToEndOfRow(colIt);
                *currColIt += std::distance(currColIt, colIt);  // set number of skipped entries
            }
            STORM_LOG_ASSERT(
                !std::all_of(rowIndexRange.begin(), rowIndexRange.end(), [&ignore, &groupIndex](IndexType rowIndex) { return ignore(groupIndex, rowIndex); }),
                "All rows in row group " << groupIndex << " are ignored.");
            STORM_LOG_ASSERT(colIt != matrixColumns.end(), "VI Operator in invalid state.");
            STORM_LOG_ASSERT(*colIt >= StartOfRowIndicator, "VI Operator in invalid state.");
        }
        STORM_LOG_ASSERT(*colIt == StartOfRowGroupIndicator, "VI Operator in invalid state.");
    }
    hasSkippedRows = true;
}

template<typename ValueType, bool TrivialRowGrouping>
void ValueIterationOperator<ValueType, TrivialRowGrouping>::setIgnoredRows(bool useLocalRowIndices, std::function<bool(IndexType, IndexType)> const& ignore) {
    if (backwards) {
        setIgnoredRows<true>(useLocalRowIndices, ignore);
    } else {
        setIgnoredRows<false>(useLocalRowIndices, ignore);
    }
}

template<typename ValueType, bool TrivialRowGrouping>
std::vector<typename ValueIterationOperator<ValueType, TrivialRowGrouping>::IndexType> const&
ValueIterationOperator<ValueType, TrivialRowGrouping>::getRowGroupIndices() const {
    STORM_LOG_ASSERT(!TrivialRowGrouping, "Tried to get row group indices for trivial row grouping");
    return *rowGroupIndices;
}

template<typename ValueType, bool TrivialRowGrouping>
std::vector<ValueType>& ValueIterationOperator<ValueType, TrivialRowGrouping>::allocateAuxiliaryVector(uint64_t size,
                                                                                                       std::optional<ValueType> const& initialValue) {
    STORM_LOG_ASSERT(!auxiliaryVectorUsedExternally, "Auxiliary vector already in use.");
    if (initialValue) {
        auxiliaryVector.assign(size, *initialValue);
    } else {
        auxiliaryVector.resize(size);
    }
    auxiliaryVectorUsedExternally = true;
    return auxiliaryVector;
}

template<typename ValueType, bool TrivialRowGrouping>
void ValueIterationOperator<ValueType, TrivialRowGrouping>::freeAuxiliaryVector() {
    auxiliaryVectorUsedExternally = false;
}

template<typename ValueType, bool TrivialRowGrouping>
void ValueIterationOperator<ValueType, TrivialRowGrouping>::moveToEndOfRow(std::vector<IndexType>::iterator& matrixColumnIt) const {
    do {
        ++matrixColumnIt;
    } while (*matrixColumnIt < StartOfRowIndicator);
}

template<typename ValueType, bool TrivialRowGrouping>
bool ValueIterationOperator<ValueType, TrivialRowGrouping>::skipIgnoredRow(std::vector<IndexType>::const_iterator& matrixColumnIt,
                                                                           typename std::vector<ValueType>::const_iterator& matrixValueIt) const {
    if (IndexType entriesToSkip = (*matrixColumnIt & SkipNumEntriesMask)) {
        matrixColumnIt += entriesToSkip;
        matrixValueIt += entriesToSkip - 1;
        return true;
    }
    return false;
}

template<typename ValueType, bool TrivialRowGrouping>
uint64_t ValueIterationOperator<ValueType, TrivialRowGrouping>::skipMultipleIgnoredRows(std::vector<IndexType>::const_iterator& matrixColumnIt,
                                                                                        typename std::vector<ValueType>::const_iterator& matrixValueIt) const {
    IndexType result{0ull};
    while (skipIgnoredRow(matrixColumnIt, matrixValueIt)) {
        ++result;
        STORM_LOG_ASSERT(*matrixColumnIt >= StartOfRowIndicator, "Undexpected state of VI operator");
        // We (currently) don't use this past the end of a row group, so we may have this additional sanity check:
        STORM_LOG_ASSERT(*matrixColumnIt < StartOfRowGroupIndicator, "Undexpected state of VI operator");
    }
    return result;
}

template class ValueIterationOperator<double, true>;
template class ValueIterationOperator<double, false>;
template class ValueIterationOperator<storm::RationalNumber, true>;
template class ValueIterationOperator<storm::RationalNumber, false>;

}  // namespace storm::solver::helper
