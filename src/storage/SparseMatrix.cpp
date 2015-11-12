#include <boost/functional/hash.hpp>
#include <src/storage/sparse/StateType.h>

// To detect whether the usage of TBB is possible, this include is neccessary
#include "storm-config.h"

#ifdef STORM_HAVE_INTELTBB
#include "tbb/tbb.h"
#endif

#include "src/storage/SparseMatrix.h"
#include "src/adapters/CarlAdapter.h"

#include "src/storage/BitVector.h"
#include "src/utility/constants.h"
#include "src/utility/ConstantsComparator.h"

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/OutOfRangeException.h"

#include "src/utility/macros.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
    namespace storage {
        
        template<typename IndexType, typename ValueType>
        MatrixEntry<IndexType, ValueType>::MatrixEntry(IndexType column, ValueType value) : entry(column, value) {
            // Intentionally left empty.
        }
        
        template<typename IndexType, typename ValueType>
        MatrixEntry<IndexType, ValueType>::MatrixEntry(std::pair<IndexType, ValueType>&& pair) : entry(std::move(pair)) {
            // Intentionally left empty.
        }
        
        template<typename IndexType, typename ValueType>
        IndexType const& MatrixEntry<IndexType, ValueType>::getColumn() const {
            return this->entry.first;
        }
        
        template<typename IndexType, typename ValueType>
        void MatrixEntry<IndexType, ValueType>::setColumn(IndexType const& column) {
            this->entry.first = column;
        }
        
        template<typename IndexType, typename ValueType>
        ValueType const& MatrixEntry<IndexType, ValueType>::getValue() const {
            return this->entry.second;
        }
        
        template<typename IndexType, typename ValueType>
        void MatrixEntry<IndexType, ValueType>::setValue(ValueType const& value) {
            this->entry.second = value;
        }
        
        template<typename IndexType, typename ValueType>
        std::pair<IndexType, ValueType> const& MatrixEntry<IndexType, ValueType>::getColumnValuePair() const {
            return this->entry;
        }
        
        template<typename IndexType, typename ValueType>
        MatrixEntry<IndexType, ValueType> MatrixEntry<IndexType, ValueType>::operator*(value_type factor) const {
            return MatrixEntry(this->getColumn(), this->getValue() * factor);
        }
        
        template<typename IndexTypePrime, typename ValueTypePrime>
        std::ostream& operator<<(std::ostream& out, MatrixEntry<IndexTypePrime, ValueTypePrime> const& entry) {
            out << "(" << entry.getColumn() << ", " << entry.getValue() << ")";
            return out;
        }
        
        template<typename ValueType>
        SparseMatrixBuilder<ValueType>::SparseMatrixBuilder(index_type rows, index_type columns, index_type entries, bool forceDimensions, bool hasCustomRowGrouping, index_type rowGroups) : initialRowCountSet(rows != 0), initialRowCount(rows), initialColumnCountSet(columns != 0), initialColumnCount(columns), initialEntryCountSet(entries != 0), initialEntryCount(entries), forceInitialDimensions(forceDimensions), hasCustomRowGrouping(hasCustomRowGrouping), initialRowGroupCountSet(rowGroups != 0), initialRowGroupCount(rowGroups), rowGroupIndices(), columnsAndValues(), rowIndications(), currentEntryCount(0), lastRow(0), lastColumn(0), highestColumn(0), currentRowGroup(0) {
            // Prepare the internal storage.
            if (initialRowCountSet) {
                rowIndications.reserve(initialRowCount + 1);
            }
            if (initialEntryCountSet) {
                columnsAndValues.reserve(initialEntryCount);
            }
            if (initialRowGroupCountSet) {
                rowGroupIndices.reserve(initialRowGroupCount + 1);
            }
            rowIndications.push_back(0);
        }
        
        template<typename ValueType>
        SparseMatrixBuilder<ValueType>::SparseMatrixBuilder(SparseMatrix<ValueType>&& matrix) :  initialRowCountSet(false), initialRowCount(0), initialColumnCountSet(false), initialColumnCount(0), initialEntryCountSet(false), initialEntryCount(0), forceInitialDimensions(false), hasCustomRowGrouping(matrix.nontrivialRowGrouping), initialRowGroupCountSet(false), initialRowGroupCount(0), rowGroupIndices(), columnsAndValues(std::move(matrix.columnsAndValues)), rowIndications(std::move(matrix.rowIndications)), currentEntryCount(matrix.entryCount), lastRow(matrix.rowCount - 1), lastColumn(columnsAndValues.back().getColumn()), highestColumn(matrix.getColumnCount() - 1), currentRowGroup() {
            
            // If the matrix has a custom row grouping, we move it and remove the last element to make it 'open' again.
            if (hasCustomRowGrouping) {
                rowGroupIndices = std::move(matrix.rowGroupIndices);
                rowGroupIndices.pop_back();
                currentRowGroup = rowGroupIndices.size() - 1;
            }
            
            // Likewise, we need to 'open' the row indications again.
            rowIndications.pop_back();
        }
        
        template<typename ValueType>
        void SparseMatrixBuilder<ValueType>::addNextValue(index_type row, index_type column, ValueType const& value) {
            // Check that we did not move backwards wrt. the row.
            if (row < lastRow) {
                throw storm::exceptions::InvalidArgumentException() << "Illegal call to SparseMatrixBuilder::addNextValue: adding an element in row " << row << ", but an element in row " << lastRow << " has already been added.";
            }
            
            // Check that we did not move backwards wrt. to column.
            if (row == lastRow && column < lastColumn) {
                throw storm::exceptions::InvalidArgumentException() << "Illegal call to SparseMatrixBuilder::addNextValue: adding an element in column " << column << " in row " << row << ", but an element in column " << lastColumn << " has already been added in that row.";
            }
            
            // If we switched to another row, we have to adjust the missing entries in the row indices vector.
            if (row != lastRow) {
                // Otherwise, we need to push the correct values to the vectors, which might trigger reallocations.
                for (index_type i = lastRow + 1; i <= row; ++i) {
                    rowIndications.push_back(currentEntryCount);
                }
                
                lastRow = row;
            }
            
            lastColumn = column;
            
            // Finally, set the element and increase the current size.
            columnsAndValues.emplace_back(column, value);
            highestColumn = std::max(highestColumn, column);
            ++currentEntryCount;
            
            // In case we did not expect this value, we throw an exception.
            if (forceInitialDimensions) {
                STORM_LOG_THROW(!initialRowCountSet || lastRow < initialRowCount, storm::exceptions::OutOfRangeException, "Cannot insert value at illegal row " << lastRow << ".");
                STORM_LOG_THROW(!initialColumnCountSet || lastColumn < initialColumnCount, storm::exceptions::OutOfRangeException, "Cannot insert value at illegal column " << lastColumn << ".");
                STORM_LOG_THROW(!initialEntryCountSet || currentEntryCount <= initialEntryCount, storm::exceptions::OutOfRangeException, "Too many entries in matrix, expected only " << initialEntryCount << ".");
            }
        }
        
        template<typename ValueType>
        void SparseMatrixBuilder<ValueType>::newRowGroup(index_type startingRow) {
            STORM_LOG_THROW(hasCustomRowGrouping, storm::exceptions::InvalidStateException, "Matrix was not created to have a custom row grouping.");
            STORM_LOG_THROW(startingRow >= lastRow, storm::exceptions::InvalidStateException, "Illegal row group with negative size.");
            rowGroupIndices.push_back(startingRow);
            ++currentRowGroup;

            // Close all rows from the most recent one to the starting row.
            for (index_type i = lastRow + 1; i <= startingRow; ++i) {
                rowIndications.push_back(currentEntryCount);
            }

            // Reset the most recently seen row/column to allow for proper insertion of the following elements.
            lastRow = startingRow;
            lastColumn = 0;
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType> SparseMatrixBuilder<ValueType>::build(index_type overriddenRowCount, index_type overriddenColumnCount, index_type overriddenRowGroupCount) {
            uint_fast64_t rowCount = lastRow + 1;
            if (initialRowCountSet && forceInitialDimensions) {
                STORM_LOG_THROW(rowCount <= initialRowCount, storm::exceptions::InvalidStateException, "Expected not more than " << initialRowCount << " rows, but got " << rowCount << ".");
                rowCount = std::max(rowCount, initialRowCount);
            }
            rowCount = std::max(rowCount, overriddenRowCount);

            // If the current row count was overridden, we may need to add empty rows.
            for (index_type i = lastRow + 1; i < rowCount; ++i) {
                rowIndications.push_back(currentEntryCount);
            }
            
            // We put a sentinel element at the last position of the row indices array. This eases iteration work,
            // as now the indices of row i are always between rowIndications[i] and rowIndications[i + 1], also for
            // the first and last row.
            rowIndications.push_back(currentEntryCount);
            
            uint_fast64_t columnCount = highestColumn + 1;
            if (initialColumnCountSet && forceInitialDimensions) {
                STORM_LOG_THROW(columnCount <= initialColumnCount, storm::exceptions::InvalidStateException, "Expected not more than " << initialColumnCount << " columns, but got " << columnCount << ".");
                columnCount = std::max(columnCount, initialColumnCount);
            }
            columnCount = std::max(columnCount, overriddenColumnCount);
            
            uint_fast64_t entryCount = currentEntryCount;
            if (initialEntryCountSet && forceInitialDimensions) {
                STORM_LOG_THROW(entryCount == initialEntryCount, storm::exceptions::InvalidStateException, "Expected " << initialEntryCount << " entries, but got " << entryCount << ".");
            }
            
            // Check whether row groups are missing some entries.
            if (!hasCustomRowGrouping) {
                for (index_type i = 0; i <= rowCount; ++i) {
                    rowGroupIndices.push_back(i);
                }
            } else {
                uint_fast64_t rowGroupCount = currentRowGroup;
                if (initialRowGroupCountSet && forceInitialDimensions) {
                    STORM_LOG_THROW(rowGroupCount <= initialRowGroupCount, storm::exceptions::InvalidStateException, "Expected not more than " << initialRowGroupCount << " row groups, but got " << rowGroupCount << ".");
                    rowGroupCount = std::max(rowGroupCount, initialRowGroupCount);
                }
                rowGroupCount = std::max(rowGroupCount, overriddenRowGroupCount);
                
                for (index_type i = currentRowGroup; i <= rowGroupCount; ++i) {
                    rowGroupIndices.push_back(rowCount);
                }
            }

            return SparseMatrix<ValueType>(columnCount, std::move(rowIndications), std::move(columnsAndValues), std::move(rowGroupIndices), hasCustomRowGrouping);
        }
        
        template<typename ValueType>
        typename SparseMatrixBuilder<ValueType>::index_type SparseMatrixBuilder<ValueType>::getLastRow() const {
            return lastRow;
        }
        
        template<typename ValueType>
        typename SparseMatrixBuilder<ValueType>::index_type SparseMatrixBuilder<ValueType>::getLastColumn() const {
            return lastColumn;
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>::rows::rows(iterator begin, index_type entryCount) : beginIterator(begin), entryCount(entryCount) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::iterator SparseMatrix<ValueType>::rows::begin() {
            return beginIterator;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::iterator SparseMatrix<ValueType>::rows::end() {
            return beginIterator + entryCount;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::index_type SparseMatrix<ValueType>::rows::getNumberOfEntries() const {
            return this->entryCount;
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>::const_rows::const_rows(const_iterator begin, index_type entryCount) : beginIterator(begin), entryCount(entryCount) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::const_iterator SparseMatrix<ValueType>::const_rows::begin() const {
            return beginIterator;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::const_iterator SparseMatrix<ValueType>::const_rows::end() const {
            return beginIterator + entryCount;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::index_type SparseMatrix<ValueType>::const_rows::getNumberOfEntries() const {
            return this->entryCount;
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>::SparseMatrix() : rowCount(0), columnCount(0), entryCount(0), nonzeroEntryCount(0), columnsAndValues(), rowIndications(), nontrivialRowGrouping(false), rowGroupIndices() {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>::SparseMatrix(SparseMatrix<ValueType> const& other) : rowCount(other.rowCount), columnCount(other.columnCount), entryCount(other.entryCount), nonzeroEntryCount(other.nonzeroEntryCount), columnsAndValues(other.columnsAndValues), rowIndications(other.rowIndications), nontrivialRowGrouping(other.nontrivialRowGrouping), rowGroupIndices(other.rowGroupIndices) {
            // Intentionally left empty.
            std::cout << "!!!! copying matrix (constructor)" << std::endl;
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>::SparseMatrix(SparseMatrix<value_type> const& other, bool insertDiagonalElements) {
            storm::storage::BitVector rowConstraint(other.getRowCount(), true);
            storm::storage::BitVector columnConstraint(other.getColumnCount(), true);
            *this = other.getSubmatrix(false, rowConstraint, columnConstraint, insertDiagonalElements);
            std::cout << "!!!! copying matrix (insertingDiagElements)" << std::endl;
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>::SparseMatrix(SparseMatrix<ValueType>&& other) : rowCount(other.rowCount), columnCount(other.columnCount), entryCount(other.entryCount), nonzeroEntryCount(other.nonzeroEntryCount), columnsAndValues(std::move(other.columnsAndValues)), rowIndications(std::move(other.rowIndications)), nontrivialRowGrouping(other.nontrivialRowGrouping), rowGroupIndices(std::move(other.rowGroupIndices)) {
            // Now update the source matrix
            other.rowCount = 0;
            other.columnCount = 0;
            other.entryCount = 0;
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>::SparseMatrix(index_type columnCount, std::vector<index_type> const& rowIndications, std::vector<MatrixEntry<index_type, ValueType>> const& columnsAndValues, std::vector<index_type> const& rowGroupIndices, bool nontrivialRowGrouping) : rowCount(rowIndications.size() - 1), columnCount(columnCount), entryCount(columnsAndValues.size()), nonzeroEntryCount(0), columnsAndValues(columnsAndValues), rowIndications(rowIndications), nontrivialRowGrouping(nontrivialRowGrouping), rowGroupIndices(rowGroupIndices) {
            this->updateNonzeroEntryCount();
            std::cout << "!!!! copying matrix (copying Ingredients)" << std::endl;
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>::SparseMatrix(index_type columnCount, std::vector<index_type>&& rowIndications, std::vector<MatrixEntry<index_type, ValueType>>&& columnsAndValues, std::vector<index_type>&& rowGroupIndices, bool nontrivialRowGrouping) : rowCount(rowIndications.size() - 1), columnCount(columnCount), entryCount(columnsAndValues.size()), nonzeroEntryCount(0), columnsAndValues(std::move(columnsAndValues)), rowIndications(std::move(rowIndications)), nontrivialRowGrouping(nontrivialRowGrouping), rowGroupIndices(std::move(rowGroupIndices)) {
            this->updateNonzeroEntryCount();
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>& SparseMatrix<ValueType>::operator=(SparseMatrix<ValueType> const& other) {
            std::cout << "!!!! copying matrix (operator=)" << std::endl;
            // Only perform assignment if source and target are not the same.
            if (this != &other) {
                rowCount = other.rowCount;
                columnCount = other.columnCount;
                entryCount = other.entryCount;
                nonzeroEntryCount = other.nonzeroEntryCount;
                
                columnsAndValues = other.columnsAndValues;
                rowIndications = other.rowIndications;
                rowGroupIndices = other.rowGroupIndices;
                nontrivialRowGrouping = other.nontrivialRowGrouping;
            }
            
            return *this;
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>& SparseMatrix<ValueType>::operator=(SparseMatrix<ValueType>&& other) {
            // Only perform assignment if source and target are not the same.
            if (this != &other) {
                rowCount = other.rowCount;
                columnCount = other.columnCount;
                entryCount = other.entryCount;
                nonzeroEntryCount = other.nonzeroEntryCount;
                
                columnsAndValues = std::move(other.columnsAndValues);
                rowIndications = std::move(other.rowIndications);
                rowGroupIndices = std::move(other.rowGroupIndices);
                nontrivialRowGrouping = other.nontrivialRowGrouping;
            }
            
            return *this;
        }
        
        template<typename ValueType>
        bool SparseMatrix<ValueType>::operator==(SparseMatrix<ValueType> const& other) const {
            if (this == &other) {
                return true;
            }
            
            bool equalityResult = true;
            
            equalityResult &= this->getRowCount() == other.getRowCount();
            if (!equalityResult) {
                return false;
            }
            equalityResult &= this->getColumnCount() == other.getColumnCount();
            if (!equalityResult) {
                return false;
            }
            equalityResult &= this->getRowGroupIndices() == other.getRowGroupIndices();
            if (!equalityResult) {
                return false;
            }
            
            // For the actual contents, we need to do a little bit more work, because we want to ignore elements that
            // are set to zero, please they may be represented implicitly in the other matrix.
            for (index_type row = 0; row < this->getRowCount(); ++row) {
                for (const_iterator it1 = this->begin(row), ite1 = this->end(row), it2 = other.begin(row), ite2 = other.end(row); it1 != ite1 && it2 != ite2; ++it1, ++it2) {
                    // Skip over all zero entries in both matrices.
                    while (it1 != ite1 && storm::utility::isZero(it1->getValue())) {
                        ++it1;
                    }
                    while (it2 != ite2 && storm::utility::isZero(it2->getValue())) {
                        ++it2;
                    }
                    if ((it1 == ite1) || (it2 == ite2)) {
                        equalityResult = (it1 == ite1) ^ (it2 == ite2);
                        break;
                    } else {
                        if (it1->getColumn() != it2->getColumn() || it1->getValue() != it2->getValue()) {
                            equalityResult = false;
                            break;
                        }
                    }
                }
                if (!equalityResult) {
                    return false;
                }
            }
            
            return equalityResult;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::index_type SparseMatrix<ValueType>::getRowCount() const {
            return rowCount;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::index_type SparseMatrix<ValueType>::getColumnCount() const {
            return columnCount;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::index_type SparseMatrix<ValueType>::getEntryCount() const {
            return entryCount;
        }
        
        template<typename T>
        uint_fast64_t SparseMatrix<T>::getRowGroupEntryCount(uint_fast64_t const group) const {
            uint_fast64_t result = 0;
            for (uint_fast64_t row = this->getRowGroupIndices()[group]; row < this->getRowGroupIndices()[group + 1]; ++row) {
                result += (this->rowIndications[row + 1] - this->rowIndications[row]);
            }
            return result;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::index_type SparseMatrix<ValueType>::getNonzeroEntryCount() const {
            return nonzeroEntryCount;
        }
        
        template<typename ValueType>
        void SparseMatrix<ValueType>::updateNonzeroEntryCount() const {
            //SparseMatrix<ValueType>* self = const_cast<SparseMatrix<ValueType>*>(this);
            this->nonzeroEntryCount = 0;
            for (auto const& element : *this) {
                if (element.getValue() != storm::utility::zero<ValueType>()) {
                    ++this->nonzeroEntryCount;
                }
            }
        }
        
        template<typename ValueType>
        void SparseMatrix<ValueType>::updateNonzeroEntryCount(std::make_signed<index_type>::type difference) {
            this->nonzeroEntryCount += difference;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::index_type SparseMatrix<ValueType>::getRowGroupCount() const {
            return rowGroupIndices.size() - 1;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::index_type SparseMatrix<ValueType>::getRowGroupSize(index_type group) const {
            return this->getRowGroupIndices()[group + 1] - this->getRowGroupIndices()[group];
        }
        
        template<typename ValueType>
        std::vector<typename SparseMatrix<ValueType>::index_type> const& SparseMatrix<ValueType>::getRowGroupIndices() const {
            return rowGroupIndices;
        }
        
        template<typename ValueType>
        void SparseMatrix<ValueType>::makeRowsAbsorbing(storm::storage::BitVector const& rows) {
            for (auto row : rows) {
                makeRowDirac(row, row);
            }
        }
        
        template<typename ValueType>
        void SparseMatrix<ValueType>::makeRowGroupsAbsorbing(storm::storage::BitVector const& rowGroupConstraint) {
            for (auto rowGroup : rowGroupConstraint) {
                for (index_type row = this->getRowGroupIndices()[rowGroup]; row < this->getRowGroupIndices()[rowGroup + 1]; ++row) {
                    makeRowDirac(row, rowGroup);
                }
            }
        }
        
        template<typename ValueType>
        void SparseMatrix<ValueType>::makeRowDirac(index_type row, index_type column) {
            iterator columnValuePtr = this->begin(row);
            iterator columnValuePtrEnd = this->end(row);
            
            // If the row has no elements in it, we cannot make it absorbing, because we would need to move all elements
            // in the vector of nonzeros otherwise.
            if (columnValuePtr >= columnValuePtrEnd) {
                throw storm::exceptions::InvalidStateException() << "Illegal call to SparseMatrix::makeRowDirac: cannot make row " << row << " absorbing, but there is no entry in this row.";
            }
            
            // If there is at least one entry in this row, we can just set it to one, modify its column value to the
            // one given by the parameter and set all subsequent elements of this row to zero.
            columnValuePtr->setColumn(column);
            columnValuePtr->setValue(storm::utility::one<ValueType>());
            ++columnValuePtr;
            for (; columnValuePtr != columnValuePtrEnd; ++columnValuePtr) {
                ++this->nonzeroEntryCount;
                columnValuePtr->setColumn(0);
                columnValuePtr->setValue(storm::utility::zero<ValueType>());
            }
        }
        
        template<typename ValueType>
        ValueType SparseMatrix<ValueType>::getConstrainedRowSum(index_type row, storm::storage::BitVector const& constraint) const {
            ValueType result = storm::utility::zero<ValueType>();
            for (const_iterator it = this->begin(row), ite = this->end(row); it != ite; ++it) {
                if (constraint.get(it->getColumn())) {
                    result += it->getValue();
                }
            }
            return result;
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMatrix<ValueType>::getConstrainedRowSumVector(storm::storage::BitVector const& rowConstraint, storm::storage::BitVector const& columnConstraint) const {
            std::vector<ValueType> result(rowConstraint.getNumberOfSetBits());
            index_type currentRowCount = 0;
            for (auto row : rowConstraint) {
                result[currentRowCount++] = getConstrainedRowSum(row, columnConstraint);
            }
            return result;
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMatrix<ValueType>::getConstrainedRowGroupSumVector(storm::storage::BitVector const& rowGroupConstraint, storm::storage::BitVector const& columnConstraint) const {
            std::vector<ValueType> result;
            result.reserve(rowGroupConstraint.getNumberOfSetBits());
            for (auto rowGroup : rowGroupConstraint) {
                for (index_type row = this->getRowGroupIndices()[rowGroup]; row < this->getRowGroupIndices()[rowGroup + 1]; ++row) {
                    result.push_back(getConstrainedRowSum(row, columnConstraint));
                }
            }
            return result;
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType> SparseMatrix<ValueType>::getSubmatrix(bool useGroups, storm::storage::BitVector const& rowConstraint, storm::storage::BitVector const& columnConstraint, bool insertDiagonalElements) const {
            if (useGroups) {
                return getSubmatrix(rowConstraint, columnConstraint, this->getRowGroupIndices(), insertDiagonalElements);
            } else {
                // Create a fake row grouping to reduce this to a call to a more general method.
                std::vector<index_type> fakeRowGroupIndices(rowCount + 1);
                index_type i = 0;
                for (std::vector<index_type>::iterator it = fakeRowGroupIndices.begin(); it != fakeRowGroupIndices.end(); ++it, ++i) {
                    *it = i;
                }
                auto res = getSubmatrix(rowConstraint, columnConstraint, fakeRowGroupIndices, insertDiagonalElements);

                // Create a new row grouping that reflects the new sizes of the row groups.
                std::vector<uint_fast64_t> newRowGroupIndices;
                newRowGroupIndices.push_back(0);
                auto selectedRowIt = rowConstraint.begin();

                // For this, we need to count how many rows were preserved in every group.
                for (uint_fast64_t group = 0; group < this->getRowGroupCount(); ++group) {
                    uint_fast64_t newRowCount = 0;
                    while (*selectedRowIt < this->getRowGroupIndices()[group + 1]) {
                        ++selectedRowIt;
                        ++newRowCount;
                    }
                    if (newRowCount > 0) {
                        newRowGroupIndices.push_back(newRowGroupIndices.back() + newRowCount);
                    }
                }
                
                res.rowGroupIndices = newRowGroupIndices;
                return res;
            }
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType> SparseMatrix<ValueType>::getSubmatrix(storm::storage::BitVector const& rowGroupConstraint, storm::storage::BitVector const& columnConstraint, std::vector<index_type> const& rowGroupIndices, bool insertDiagonalEntries) const {
            uint_fast64_t submatrixColumnCount = columnConstraint.getNumberOfSetBits();
            
            // Start by creating a temporary vector that stores for each index whose bit is set to true the number of
            // bits that were set before that particular index.
            std::vector<index_type> columnBitsSetBeforeIndex;
            columnBitsSetBeforeIndex.reserve(columnCount);
            
            // Compute the information to fill this vector.
            index_type lastIndex = 0;
            index_type currentNumberOfSetBits = 0;
            for (auto index : columnConstraint) {
                while (lastIndex <= index) {
                    columnBitsSetBeforeIndex.push_back(currentNumberOfSetBits);
                    ++lastIndex;
                }
                ++currentNumberOfSetBits;
            }
            std::vector<index_type>* rowBitsSetBeforeIndex;
            if (&rowGroupConstraint == &columnConstraint) {
                rowBitsSetBeforeIndex = &columnBitsSetBeforeIndex;
            } else {
                rowBitsSetBeforeIndex = new std::vector<index_type>(rowCount);
                
                lastIndex = 0;
                currentNumberOfSetBits = 0;
                for (auto index : rowGroupConstraint) {
                    while (lastIndex <= index) {
                        rowBitsSetBeforeIndex->push_back(currentNumberOfSetBits);
                        ++lastIndex;
                    }
                    ++currentNumberOfSetBits;
                }
            }
            
            // Then, we need to determine the number of entries and the number of rows of the submatrix.
            index_type subEntries = 0;
            index_type subRows = 0;
            index_type rowGroupCount = 0;
            for (auto index : rowGroupConstraint) {
                subRows += rowGroupIndices[index + 1] - rowGroupIndices[index];
                for (index_type i = rowGroupIndices[index]; i < rowGroupIndices[index + 1]; ++i) {
                    bool foundDiagonalElement = false;
                    
                    for (const_iterator it = this->begin(i), ite = this->end(i); it != ite; ++it) {
                        if (columnConstraint.get(it->getColumn())) {
                            ++subEntries;
                            
                            if (columnBitsSetBeforeIndex[it->getColumn()] == (*rowBitsSetBeforeIndex)[index]) {
                                foundDiagonalElement = true;
                            }
                        }
                    }
                    
                    // If requested, we need to reserve one entry more for inserting the diagonal zero entry.
                    if (insertDiagonalEntries && !foundDiagonalElement && rowGroupCount < submatrixColumnCount) {
                        ++subEntries;
                    }
                }
                ++rowGroupCount;
            }
            
            // Create and initialize resulting matrix.
            SparseMatrixBuilder<ValueType> matrixBuilder(subRows, submatrixColumnCount, subEntries, true, this->hasNontrivialRowGrouping());
            
            // Copy over selected entries.
            rowGroupCount = 0;
            index_type rowCount = 0;
            for (auto index : rowGroupConstraint) {
                if (this->hasNontrivialRowGrouping()) {
                    matrixBuilder.newRowGroup(rowCount);
                }
                for (index_type i = rowGroupIndices[index]; i < rowGroupIndices[index + 1]; ++i) {
                    bool insertedDiagonalElement = false;
                    
                    for (const_iterator it = this->begin(i), ite = this->end(i); it != ite; ++it) {
                        if (columnConstraint.get(it->getColumn())) {
                            if (columnBitsSetBeforeIndex[it->getColumn()] == (*rowBitsSetBeforeIndex)[index]) {
                                insertedDiagonalElement = true;
                            } else if (insertDiagonalEntries && !insertedDiagonalElement && columnBitsSetBeforeIndex[it->getColumn()] > (*rowBitsSetBeforeIndex)[index]) {
                                matrixBuilder.addNextValue(rowCount, rowGroupCount, storm::utility::zero<ValueType>());
                                insertedDiagonalElement = true;
                            }
                            matrixBuilder.addNextValue(rowCount, columnBitsSetBeforeIndex[it->getColumn()], it->getValue());
                        }
                    }
                    if (insertDiagonalEntries && !insertedDiagonalElement && rowGroupCount < submatrixColumnCount) {
                        matrixBuilder.addNextValue(rowGroupCount, rowGroupCount, storm::utility::zero<ValueType>());
                    }
                    ++rowCount;
                }
                ++rowGroupCount;
            }
            
            // If the constraints were not the same, we have allocated some additional memory we need to free now.
            if (&rowGroupConstraint != &columnConstraint) {
                delete rowBitsSetBeforeIndex;
            }
            
            return matrixBuilder.build();
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType> SparseMatrix<ValueType>::restrictRows(storm::storage::BitVector const& rowsToKeep) const {
            // For now, we use the expensive call to submatrix.
            assert(rowsToKeep.size() == getRowCount());
            assert(rowsToKeep.getNumberOfSetBits() >= getRowGroupCount());
            SparseMatrix<ValueType> res(getSubmatrix(false, rowsToKeep, storm::storage::BitVector(getColumnCount(), true), false));
            assert(res.getRowCount() == rowsToKeep.getNumberOfSetBits());
            assert(res.getColumnCount() == getColumnCount());
            assert(getRowGroupCount() == res.getRowGroupCount());
            return res;
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType> SparseMatrix<ValueType>::selectRowsFromRowGroups(std::vector<index_type> const& rowGroupToRowIndexMapping, bool insertDiagonalEntries) const {
            // First, we need to count how many non-zero entries the resulting matrix will have and reserve space for
            // diagonal entries if requested.
            index_type subEntries = 0;
            for (index_type rowGroupIndex = 0, rowGroupIndexEnd = rowGroupToRowIndexMapping.size(); rowGroupIndex < rowGroupIndexEnd; ++rowGroupIndex) {
                // Determine which row we need to select from the current row group.
                index_type rowToCopy = rowGroupIndices[rowGroupIndex] + rowGroupToRowIndexMapping[rowGroupIndex];
                
                // Iterate through that row and count the number of slots we have to reserve for copying.
                bool foundDiagonalElement = false;
                for (const_iterator it = this->begin(rowToCopy), ite = this->end(rowToCopy); it != ite; ++it) {
                    if (it->getColumn() == rowGroupIndex) {
                        foundDiagonalElement = true;
                    }
                    ++subEntries;
                }
                if (insertDiagonalEntries && !foundDiagonalElement) {
                    ++subEntries;
                }
            }
            
            // Now create the matrix to be returned with the appropriate size.
            SparseMatrixBuilder<ValueType> matrixBuilder(rowGroupIndices.size() - 1, columnCount, subEntries);
            
            // Copy over the selected lines from the source matrix.
            for (index_type rowGroupIndex = 0, rowGroupIndexEnd = rowGroupToRowIndexMapping.size(); rowGroupIndex < rowGroupIndexEnd; ++rowGroupIndex) {
                // Determine which row we need to select from the current row group.
                index_type rowToCopy = rowGroupIndices[rowGroupIndex] + rowGroupToRowIndexMapping[rowGroupIndex];
                
                // Iterate through that row and copy the entries. This also inserts a zero element on the diagonal if
                // there is no entry yet.
                bool insertedDiagonalElement = false;
                for (const_iterator it = this->begin(rowToCopy), ite = this->end(rowToCopy); it != ite; ++it) {
                    if (it->getColumn() == rowGroupIndex) {
                        insertedDiagonalElement = true;
                    } else if (insertDiagonalEntries && !insertedDiagonalElement && it->getColumn() > rowGroupIndex) {
                        matrixBuilder.addNextValue(rowGroupIndex, rowGroupIndex, storm::utility::zero<ValueType>());
                        insertedDiagonalElement = true;
                    }
                    matrixBuilder.addNextValue(rowGroupIndex, it->getColumn(), it->getValue());
                }
                if (insertDiagonalEntries && !insertedDiagonalElement) {
                    matrixBuilder.addNextValue(rowGroupIndex, rowGroupIndex, storm::utility::zero<ValueType>());
                }
            }
            
            // Finalize created matrix and return result.
            return matrixBuilder.build();
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType> SparseMatrix<ValueType>::selectRowsFromRowIndexSequence(std::vector<index_type> const& rowIndexSequence, bool insertDiagonalEntries) const{
            // First, we need to count how many non-zero entries the resulting matrix will have and reserve space for
            // diagonal entries if requested.
            index_type newEntries = 0;
            for(index_type row = 0, rowEnd = rowIndexSequence.size(); row < rowEnd; ++row) {
                bool foundDiagonalElement = false;
                for (const_iterator it = this->begin(rowIndexSequence[row]), ite = this->end(rowIndexSequence[row]); it != ite; ++it) {
                    if (it->getColumn() == row) {
                        foundDiagonalElement = true;
                    }
                    ++newEntries;
                }
                if (insertDiagonalEntries && !foundDiagonalElement) {
                    ++newEntries;
                }
            }
            
            // Now create the matrix to be returned with the appropriate size.
            SparseMatrixBuilder<ValueType> matrixBuilder(rowIndexSequence.size(), columnCount, newEntries);
            
            // Copy over the selected rows from the source matrix.
            for(index_type row = 0, rowEnd = rowIndexSequence.size(); row < rowEnd; ++row) {
                bool insertedDiagonalElement = false;
                for (const_iterator it = this->begin(rowIndexSequence[row]), ite = this->end(rowIndexSequence[row]); it != ite; ++it) {
                    if (it->getColumn() == row) {
                        insertedDiagonalElement = true;
                    } else if (insertDiagonalEntries && !insertedDiagonalElement && it->getColumn() > row) {
                        matrixBuilder.addNextValue(row, row, storm::utility::zero<ValueType>());
                        insertedDiagonalElement = true;
                    }
                    matrixBuilder.addNextValue(row, it->getColumn(), it->getValue());
                }
                if (insertDiagonalEntries && !insertedDiagonalElement) {
                    matrixBuilder.addNextValue(row, row, storm::utility::zero<ValueType>());
                }
            }
            
            // Finally create matrix and return result.
            return matrixBuilder.build();
        }
        
        template <typename ValueType>
        SparseMatrix<ValueType> SparseMatrix<ValueType>::transpose(bool joinGroups, bool keepZeros) const {
            index_type rowCount = this->getColumnCount();
            index_type columnCount = joinGroups ? this->getRowGroupCount() : this->getRowCount();
            index_type entryCount;
            if (keepZeros) {
                entryCount = this->getEntryCount();
            } else {
                this->updateNonzeroEntryCount();
                entryCount = this->getNonzeroEntryCount();
            }
            
            std::vector<index_type> rowIndications(rowCount + 1);
            std::vector<MatrixEntry<index_type, ValueType>> columnsAndValues(entryCount);
            
            // First, we need to count how many entries each column has.
            for (index_type group = 0; group < columnCount; ++group) {
                for (auto const& transition : joinGroups ? this->getRowGroup(group) : this->getRow(group)) {
                    if (transition.getValue() != storm::utility::zero<ValueType>() || keepZeros) {
                        ++rowIndications[transition.getColumn() + 1];
                    }
                }
            }
            
            // Now compute the accumulated offsets.
            for (index_type i = 1; i < rowCount + 1; ++i) {
                rowIndications[i] = rowIndications[i - 1] + rowIndications[i];
            }
            
            // Create an array that stores the index for the next value to be added for
            // each row in the transposed matrix. Initially this corresponds to the previously
            // computed accumulated offsets.
            std::vector<index_type> nextIndices = rowIndications;
            
            // Now we are ready to actually fill in the values of the transposed matrix.
            for (index_type group = 0; group < columnCount; ++group) {
                for (auto const& transition : joinGroups ? this->getRowGroup(group) : this->getRow(group)) {
                    if (transition.getValue() != storm::utility::zero<ValueType>() || keepZeros) {
                        columnsAndValues[nextIndices[transition.getColumn()]] = std::make_pair(group, transition.getValue());
                        nextIndices[transition.getColumn()]++;
                    }
                }
            }
            
            std::vector<index_type> rowGroupIndices(rowCount + 1);
            for (index_type i = 0; i <= rowCount; ++i) {
                rowGroupIndices[i] = i;
            }
            
            storm::storage::SparseMatrix<ValueType> transposedMatrix(columnCount, std::move(rowIndications), std::move(columnsAndValues), std::move(rowGroupIndices), false);
            
            return transposedMatrix;
        }
        
        template<typename ValueType>
        void SparseMatrix<ValueType>::convertToEquationSystem() {
            invertDiagonal();
            negateAllNonDiagonalEntries();
        }
        
        template<typename ValueType>
        void SparseMatrix<ValueType>::invertDiagonal() {
            // Now iterate over all row groups and set the diagonal elements to the inverted value.
            // If there is a row without the diagonal element, an exception is thrown.
            ValueType one = storm::utility::one<ValueType>();
            ValueType zero = storm::utility::zero<ValueType>();
            bool foundDiagonalElement = false;
            for (index_type group = 0; group < this->getRowGroupCount(); ++group) {
                for (auto& entry : this->getRowGroup(group)) {
                    if (entry.getColumn() == group) {
                        if (entry.getValue() == one) {
                            --this->nonzeroEntryCount;
                            entry.setValue(zero);
                        } else if (entry.getValue() == zero) {
                            ++this->nonzeroEntryCount;
                            entry.setValue(one);
                        } else {
                            entry.setValue(one - entry.getValue());
                        }
                        foundDiagonalElement = true;
                    }
                }
                
                // Throw an exception if a row did not have an element on the diagonal.
                if (!foundDiagonalElement) {
                    throw storm::exceptions::InvalidArgumentException() << "Illegal call to SparseMatrix::invertDiagonal: matrix is missing diagonal entries.";
                }
            }
        }
        
        template<typename ValueType>
        void SparseMatrix<ValueType>::negateAllNonDiagonalEntries() {
            // Iterate over all row groups and negate all the elements that are not on the diagonal.
            for (index_type group = 0; group < this->getRowGroupCount(); ++group) {
                for (auto& entry : this->getRowGroup(group)) {
                    if (entry.getColumn() != group) {
                        entry.setValue(-entry.getValue());
                    }
                }
            }
        }
        
        template<typename ValueType>
        void SparseMatrix<ValueType>::deleteDiagonalEntries() {
            // Iterate over all rows and negate all the elements that are not on the diagonal.
            for (index_type group = 0; group < this->getRowGroupCount(); ++group) {
                for (auto& entry : this->getRowGroup(group)) {
                    if (entry.getColumn() == group) {
                        --this->nonzeroEntryCount;
                        entry.setValue(storm::utility::zero<ValueType>());
                    }
                }
            }
        }
        
        template<typename ValueType>
        typename std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> SparseMatrix<ValueType>::getJacobiDecomposition() const {
            STORM_LOG_THROW(this->getRowCount() == this->getColumnCount(), storm::exceptions::InvalidArgumentException, "Canno compute Jacobi decomposition of non-square matrix.");
            
            // Prepare the resulting data structures.
            SparseMatrixBuilder<ValueType> luBuilder(this->getRowCount(), this->getColumnCount());
            std::vector<ValueType> invertedDiagonal(rowCount);
            
            // Copy entries to the appropriate matrices.
            for (index_type rowNumber = 0; rowNumber < rowCount; ++rowNumber) {
                for (const_iterator it = this->begin(rowNumber), ite = this->end(rowNumber); it != ite; ++it) {
                    if (it->getColumn() == rowNumber) {
                        invertedDiagonal[rowNumber] = storm::utility::one<ValueType>() / it->getValue();
                    } else {
                        luBuilder.addNextValue(rowNumber, it->getColumn(), it->getValue());
                    }
                }
            }
            
            return std::make_pair(luBuilder.build(), std::move(invertedDiagonal));
        }
        
#ifdef STORM_HAVE_CARL
        template<>
        typename std::pair<storm::storage::SparseMatrix<RationalFunction>, std::vector<RationalFunction>> SparseMatrix<RationalFunction>::getJacobiDecomposition() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This operation is not supported.");
        }
#endif
        
        template<typename ValueType>
        template<typename OtherValueType, typename ResultValueType>
        std::vector<ResultValueType> SparseMatrix<ValueType>::getPointwiseProductRowSumVector(storm::storage::SparseMatrix<OtherValueType> const& otherMatrix) const {
            std::vector<ResultValueType> result(rowCount, storm::utility::zero<ResultValueType>());
            
            // Iterate over all elements of the current matrix and either continue with the next element in case the
            // given matrix does not have a non-zero element at this column position, or multiply the two entries and
            // add the result to the corresponding position in the vector.
            for (index_type row = 0; row < rowCount && row < otherMatrix.getRowCount(); ++row) {
                typename storm::storage::SparseMatrix<OtherValueType>::const_iterator it2 = otherMatrix.begin(row);
                typename storm::storage::SparseMatrix<OtherValueType>::const_iterator ite2 = otherMatrix.end(row);
                for (const_iterator it1 = this->begin(row), ite1 = this->end(row); it1 != ite1 && it2 != ite2; ++it1) {
                    if (it1->getColumn() < it2->getColumn()) {
                        continue;
                    } else {
                        // If the precondition of this method (i.e. that the given matrix is a submatrix
                        // of the current one) was fulfilled, we know now that the two elements are in
                        // the same column, so we can multiply and add them to the row sum vector.
                        result[row] += it2->getValue() * OtherValueType(it1->getValue());
                        ++it2;
                    }
                }
            }
            
            return result;
        }
        
        template<typename ValueType>
        void SparseMatrix<ValueType>::multiplyWithVector(std::vector<ValueType> const& vector, std::vector<ValueType>& result) const {
#ifdef STORM_HAVE_INTELTBB
            if (this->getNonzeroEntryCount() > 10000) {
                return this->multiplyWithVectorParallel(vector, result);
            } else {
                return this->multiplyWithVectorSequential(vector, result);
            }
#else
            return multiplyWithVectorSequential(vector, result);
#endif
        }
        
        template<typename ValueType>
        void SparseMatrix<ValueType>::multiplyWithVectorSequential(std::vector<ValueType> const& vector, std::vector<ValueType>& result) const {
            const_iterator it = this->begin();
            const_iterator ite;
            std::vector<index_type>::const_iterator rowIterator = rowIndications.begin();
            typename std::vector<ValueType>::iterator resultIterator = result.begin();
            typename std::vector<ValueType>::iterator resultIteratorEnd = result.end();
            
            // If the vector to multiply with and the target vector are actually the same, we need an auxiliary variable
            // to store the intermediate result.
            if (&vector == &result) {
                for (; resultIterator != resultIteratorEnd; ++rowIterator, ++resultIterator) {
                    ValueType tmpValue = storm::utility::zero<ValueType>();
                    
                    for (ite = this->begin() + *(rowIterator + 1); it != ite; ++it) {
                        tmpValue += it->getValue() * vector[it->getColumn()];
                    }
                    *resultIterator = tmpValue;
                }
            } else {
                for (; resultIterator != resultIteratorEnd; ++rowIterator, ++resultIterator) {
                    *resultIterator = storm::utility::zero<ValueType>();
                    
                    for (ite = this->begin() + *(rowIterator + 1); it != ite; ++it) {
                        *resultIterator += it->getValue() * vector[it->getColumn()];
                    }
                }
            }
        }
        
#ifdef STORM_HAVE_INTELTBB
        template<typename ValueType>
        void SparseMatrix<ValueType>::multiplyWithVectorParallel(std::vector<ValueType> const& vector, std::vector<ValueType>& result) const {
            tbb::parallel_for(tbb::blocked_range<index_type>(0, result.size(), 10),
                              [&] (tbb::blocked_range<index_type> const& range) {
                                  index_type startRow = range.begin();
                                  index_type endRow = range.end();
                                  const_iterator it = this->begin(startRow);
                                  const_iterator ite;
                                  std::vector<index_type>::const_iterator rowIterator = this->rowIndications.begin() + startRow;
                                  std::vector<index_type>::const_iterator rowIteratorEnd = this->rowIndications.begin() + endRow;
                                  typename std::vector<ValueType>::iterator resultIterator = result.begin() + startRow;
                                  typename std::vector<ValueType>::iterator resultIteratorEnd = result.begin() + endRow;
                                  
                                  for (; resultIterator != resultIteratorEnd; ++rowIterator, ++resultIterator) {
                                      *resultIterator = storm::utility::zero<ValueType>();
                                      
                                      for (ite = this->begin() + *(rowIterator + 1); it != ite; ++it) {
                                          *resultIterator += it->getValue() * vector[it->getColumn()];
                                      }
                                  }
                              });
        }
#endif
        
        template<typename ValueType>
        void SparseMatrix<ValueType>::performSuccessiveOverRelaxationStep(ValueType omega, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            const_iterator it = this->begin();
            const_iterator ite;
            std::vector<index_type>::const_iterator rowIterator = rowIndications.begin();
            typename std::vector<ValueType>::const_iterator bIt = b.begin();
            typename std::vector<ValueType>::iterator resultIterator = x.begin();
            typename std::vector<ValueType>::iterator resultIteratorEnd = x.end();
            
            // If the vector to multiply with and the target vector are actually the same, we need an auxiliary variable
            // to store the intermediate result.
            index_type currentRow = 0;
            for (; resultIterator != resultIteratorEnd; ++rowIterator, ++resultIterator, ++bIt) {
                ValueType tmpValue = storm::utility::zero<ValueType>();
                ValueType diagonalElement = storm::utility::zero<ValueType>();
                
                for (ite = this->begin() + *(rowIterator + 1); it != ite; ++it) {
                    if (it->getColumn() != currentRow) {
                        tmpValue += it->getValue() * x[it->getColumn()];
                    } else {
                        diagonalElement += it->getValue();
                    }
                }
                
                *resultIterator = ((storm::utility::one<ValueType>() - omega) * *resultIterator) + (omega / diagonalElement) * (*bIt - tmpValue);
                ++currentRow;
            }
        }
        
        template<typename ValueType>
        void SparseMatrix<ValueType>::multiplyVectorWithMatrix(std::vector<value_type> const& vector, std::vector<value_type>& result) const {
            const_iterator it = this->begin();
            const_iterator ite;
            std::vector<index_type>::const_iterator rowIterator = rowIndications.begin();
            std::vector<index_type>::const_iterator rowIteratorEnd = rowIndications.end();
            
            uint_fast64_t currentRow = 0;
            for (; rowIterator != rowIteratorEnd - 1; ++rowIterator) {
                for (ite = this->begin() + *(rowIterator + 1); it != ite; ++it) {
                    result[it->getColumn()] += it->getValue() * vector[currentRow];
                }
                ++currentRow;
            }
        }
        
        template<typename ValueType>
        std::size_t SparseMatrix<ValueType>::getSizeInBytes() const {
            uint_fast64_t size = sizeof(*this);
            
            // Add size of columns and values.
            size += sizeof(MatrixEntry<index_type, ValueType>) * columnsAndValues.capacity();
            
            // Add row_indications size.
            size += sizeof(uint_fast64_t) * rowIndications.capacity();
            
            return size;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::const_rows SparseMatrix<ValueType>::getRows(index_type startRow, index_type endRow) const {
            return const_rows(this->columnsAndValues.begin() + this->rowIndications[startRow], this->rowIndications[endRow + 1] - this->rowIndications[startRow]);
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::rows SparseMatrix<ValueType>::getRows(index_type startRow, index_type endRow) {
            return rows(this->columnsAndValues.begin() + this->rowIndications[startRow], this->rowIndications[endRow + 1] - this->rowIndications[startRow]);
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::const_rows SparseMatrix<ValueType>::getRow(index_type row) const {
            return getRows(row, row);
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::rows SparseMatrix<ValueType>::getRow(index_type row) {
            return getRows(row, row);
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::const_rows SparseMatrix<ValueType>::getRow(index_type rowGroup, index_type offset) const {
            assert(rowGroup < this->getRowGroupCount());
            assert(offset < this->getRowGroupEntryCount(rowGroup));
            return getRow(rowGroupIndices[rowGroup] + offset);
        }
        
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::rows SparseMatrix<ValueType>::getRow(index_type rowGroup, index_type offset) {
            assert(rowGroup < this->getRowGroupCount());
            assert(offset < this->getRowGroupEntryCount(rowGroup));
            return getRow(rowGroupIndices[rowGroup] + offset);
        }
        
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::const_rows SparseMatrix<ValueType>::getRowGroup(index_type rowGroup) const {
            assert(rowGroup < this->getRowGroupCount());
            return getRows(rowGroupIndices[rowGroup], rowGroupIndices[rowGroup + 1] - 1);
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::rows SparseMatrix<ValueType>::getRowGroup(index_type rowGroup) {
            assert(rowGroup < this->getRowGroupCount());
            return getRows(rowGroupIndices[rowGroup], rowGroupIndices[rowGroup + 1] - 1);
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::const_iterator SparseMatrix<ValueType>::begin(index_type row) const {
            return this->columnsAndValues.begin() + this->rowIndications[row];
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::iterator SparseMatrix<ValueType>::begin(index_type row)  {
            return this->columnsAndValues.begin() + this->rowIndications[row];
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::const_iterator SparseMatrix<ValueType>::end(index_type row) const {
            return this->columnsAndValues.begin() + this->rowIndications[row + 1];
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::iterator SparseMatrix<ValueType>::end(index_type row)  {
            return this->columnsAndValues.begin() + this->rowIndications[row + 1];
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::const_iterator SparseMatrix<ValueType>::end() const {
            return this->columnsAndValues.begin() + this->rowIndications[rowCount];
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::iterator SparseMatrix<ValueType>::end()  {
            return this->columnsAndValues.begin() + this->rowIndications[rowCount];
        }
        
        template<typename ValueType>
        bool SparseMatrix<ValueType>::hasNontrivialRowGrouping() const {
            return nontrivialRowGrouping;
        }
        
        template<typename ValueType>
        ValueType SparseMatrix<ValueType>::getRowSum(index_type row) const {
            ValueType sum = storm::utility::zero<ValueType>();
            for (const_iterator it = this->begin(row), ite = this->end(row); it != ite; ++it) {
                sum += it->getValue();
            }
            return sum;
        }
        
        template<typename ValueType>
        bool SparseMatrix<ValueType>::isProbabilistic() const {
            storm::utility::ConstantsComparator<ValueType> comparator;
            for(index_type row = 0; row < this->rowCount; ++row) {
                if(!comparator.isOne(getRowSum(row))) {
                    return false;
                }
            }
            return true;
        }
        
        template<typename ValueType>
        template<typename OtherValueType>
        bool SparseMatrix<ValueType>::isSubmatrixOf(SparseMatrix<OtherValueType> const& matrix) const {
            // Check for matching sizes.
            if (this->getRowCount() != matrix.getRowCount()) return false;
            if (this->getColumnCount() != matrix.getColumnCount()) return false;
            if (this->getRowGroupIndices() != matrix.getRowGroupIndices()) return false;
            
            // Check the subset property for all rows individually.
            for (index_type row = 0; row < this->getRowCount(); ++row) {
                auto it2 = matrix.begin(row);
                auto ite2 = matrix.end(row);
                for (const_iterator it1 = this->begin(row), ite1 = this->end(row); it1 != ite1; ++it1) {
                    // Skip over all entries of the other matrix that are before the current entry in the current matrix.
                    while (it2 != ite2 && it2->getColumn() < it1->getColumn()) {
                        ++it2;
                    }
                    if (it2 == ite2 || it1->getColumn() != it2->getColumn()) {
                        return false;
                    }
                }
            }
            return true;
        }
        
        template<typename ValueType>
        std::ostream& operator<<(std::ostream& out, SparseMatrix<ValueType> const& matrix) {
            // Print column numbers in header.
            out << "\t\t";
            for (typename SparseMatrix<ValueType>::index_type i = 0; i < matrix.getColumnCount(); ++i) {
                out << i << "\t";
            }
            out << std::endl;
            
            // Iterate over all row groups.
            for (typename SparseMatrix<ValueType>::index_type group = 0; group < matrix.getRowGroupCount(); ++group) {
                out << "\t---- group " << group << "/" << (matrix.getRowGroupCount() - 1) << " ---- " << std::endl;
                for (typename SparseMatrix<ValueType>::index_type i = matrix.getRowGroupIndices()[group]; i < matrix.getRowGroupIndices()[group + 1]; ++i) {
                    typename SparseMatrix<ValueType>::index_type nextIndex = matrix.rowIndications[i];
                    
                    // Print the actual row.
                    out << i << "\t(\t";
                    typename SparseMatrix<ValueType>::index_type currentRealIndex = 0;
                    while (currentRealIndex < matrix.columnCount) {
                        if (nextIndex < matrix.rowIndications[i + 1] && currentRealIndex == matrix.columnsAndValues[nextIndex].getColumn()) {
                            out << matrix.columnsAndValues[nextIndex].getValue() << "\t";
                            ++nextIndex;
                        } else {
                            out << "0\t";
                        }
                        ++currentRealIndex;
                    }
                    out << "\t)\t" << i << std::endl;
                }
            }
            
            // Print column numbers in footer.
            out << "\t\t";
            for (typename SparseMatrix<ValueType>::index_type i = 0; i < matrix.getColumnCount(); ++i) {
                out << i << "\t";
            }
            out << std::endl;
            
            return out;
        }
        
        template<typename ValueType>
        std::size_t SparseMatrix<ValueType>::hash() const {
            std::size_t result = 0;
            
            boost::hash_combine(result, this->getRowCount());
            boost::hash_combine(result, this->getColumnCount());
            boost::hash_combine(result, this->getEntryCount());
            boost::hash_combine(result, boost::hash_range(columnsAndValues.begin(), columnsAndValues.end()));
            boost::hash_combine(result, boost::hash_range(rowIndications.begin(), rowIndications.end()));
            boost::hash_combine(result, boost::hash_range(rowGroupIndices.begin(), rowGroupIndices.end()));
            
            return result;
        }
        
        // Explicitly instantiate the entry, builder and the matrix.
        // double
        template class MatrixEntry<typename SparseMatrix<double>::index_type, double>;
        template std::ostream& operator<<(std::ostream& out, MatrixEntry<typename SparseMatrix<double>::index_type, double> const& entry);
        template class SparseMatrixBuilder<double>;
        template class SparseMatrix<double>;
        template std::ostream& operator<<(std::ostream& out, SparseMatrix<double> const& matrix);
        template std::vector<double> SparseMatrix<double>::getPointwiseProductRowSumVector(storm::storage::SparseMatrix<double> const& otherMatrix) const;
        template bool SparseMatrix<double>::isSubmatrixOf(SparseMatrix<double> const& matrix) const;

        // float
        template class MatrixEntry<typename SparseMatrix<float>::index_type, float>;
        template std::ostream& operator<<(std::ostream& out, MatrixEntry<typename SparseMatrix<float>::index_type, float> const& entry);
        template class SparseMatrixBuilder<float>;
        template class SparseMatrix<float>;
        template std::ostream& operator<<(std::ostream& out, SparseMatrix<float> const& matrix);
        template std::vector<float> SparseMatrix<float>::getPointwiseProductRowSumVector(storm::storage::SparseMatrix<float> const& otherMatrix) const;
        template bool SparseMatrix<float>::isSubmatrixOf(SparseMatrix<float> const& matrix) const;

        // int
        template class MatrixEntry<typename SparseMatrix<int>::index_type, int>;
        template std::ostream& operator<<(std::ostream& out, MatrixEntry<typename SparseMatrix<int>::index_type, int> const& entry);
        template class SparseMatrixBuilder<int>;
        template class SparseMatrix<int>;
        template std::ostream& operator<<(std::ostream& out, SparseMatrix<int> const& matrix);
        template bool SparseMatrix<int>::isSubmatrixOf(SparseMatrix<int> const& matrix) const;

        // state_type
        template class MatrixEntry<typename SparseMatrix<storm::storage::sparse::state_type>::index_type, storm::storage::sparse::state_type>;
        template std::ostream& operator<<(std::ostream& out, MatrixEntry<typename SparseMatrix<storm::storage::sparse::state_type>::index_type, storm::storage::sparse::state_type> const& entry);
        template class SparseMatrixBuilder<storm::storage::sparse::state_type>;
        template class SparseMatrix<storm::storage::sparse::state_type>;
        template std::ostream& operator<<(std::ostream& out, SparseMatrix<storm::storage::sparse::state_type> const& matrix);
        template bool SparseMatrix<int>::isSubmatrixOf(SparseMatrix<storm::storage::sparse::state_type> const& matrix) const;

#ifdef STORM_HAVE_CARL
        // Rat Function
        template class MatrixEntry<typename SparseMatrix<RationalFunction>::index_type, RationalFunction>;
        template std::ostream& operator<<(std::ostream& out, MatrixEntry<uint_fast64_t, RationalFunction> const& entry);
        template class SparseMatrixBuilder<RationalFunction>;
        template class SparseMatrix<RationalFunction>;
        template std::ostream& operator<<(std::ostream& out, SparseMatrix<RationalFunction> const& matrix);
        template std::vector<storm::RationalFunction> SparseMatrix<RationalFunction>::getPointwiseProductRowSumVector(storm::storage::SparseMatrix<storm::RationalFunction> const& otherMatrix) const;
        template std::vector<storm::RationalFunction> SparseMatrix<double>::getPointwiseProductRowSumVector(storm::storage::SparseMatrix<storm::RationalFunction> const& otherMatrix) const;
        template std::vector<storm::RationalFunction> SparseMatrix<float>::getPointwiseProductRowSumVector(storm::storage::SparseMatrix<storm::RationalFunction> const& otherMatrix) const;
        template std::vector<storm::RationalFunction> SparseMatrix<int>::getPointwiseProductRowSumVector(storm::storage::SparseMatrix<storm::RationalFunction> const& otherMatrix) const;
        template bool SparseMatrix<storm::RationalFunction>::isSubmatrixOf(SparseMatrix<storm::RationalFunction> const& matrix) const;

        // Intervals
        template std::vector<storm::Interval> SparseMatrix<double>::getPointwiseProductRowSumVector(storm::storage::SparseMatrix<storm::Interval> const& otherMatrix) const;
        template class MatrixEntry<typename SparseMatrix<Interval>::index_type, Interval>;
        template std::ostream& operator<<(std::ostream& out, MatrixEntry<uint_fast64_t, Interval> const& entry);
        template class SparseMatrixBuilder<Interval>;
        template class SparseMatrix<Interval>;
        template std::ostream& operator<<(std::ostream& out, SparseMatrix<Interval> const& matrix);
        template std::vector<storm::Interval> SparseMatrix<Interval>::getPointwiseProductRowSumVector(storm::storage::SparseMatrix<storm::Interval> const& otherMatrix) const;
        template bool SparseMatrix<storm::Interval>::isSubmatrixOf(SparseMatrix<storm::Interval> const& matrix) const;

        template bool SparseMatrix<storm::Interval>::isSubmatrixOf(SparseMatrix<double> const& matrix) const;
#endif
        
        
    } // namespace storage
} // namespace storm



