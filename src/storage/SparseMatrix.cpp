#include <boost/functional/hash.hpp>

// To detect whether the usage of TBB is possible, this include is neccessary
#include "storm-config.h"

#ifdef STORM_HAVE_INTELTBB
#include "tbb/tbb.h"
#endif

#include "src/storage/SparseMatrix.h"
#include "src/exceptions/InvalidStateException.h"

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
        
        template<typename ValueType>
        SparseMatrixBuilder<ValueType>::SparseMatrixBuilder(IndexType rows, IndexType columns, IndexType entries, bool hasCustomRowGrouping, IndexType rowGroups) : rowCountSet(rows != 0), rowCount(rows), columnCountSet(columns != 0), columnCount(columns), entryCount(entries), hasCustomRowGrouping(hasCustomRowGrouping), rowGroupCountSet(rowGroups != 0), rowGroupCount(rowGroups), rowGroupIndices(), storagePreallocated(rows != 0 && columns != 0 && entries != 0), columnsAndValues(), rowIndications(), currentEntryCount(0), lastRow(0), lastColumn(0), currentRowGroup(0) {
            this->prepareInternalStorage();
        }
        
        template<typename ValueType>
        void SparseMatrixBuilder<ValueType>::addNextValue(IndexType row, IndexType column, ValueType const& value) {
            // Depending on whether the internal data storage was preallocated or not, adding the value is done somewhat
            // differently.
            if (storagePreallocated) {
                // Check whether the given row and column positions are valid and throw error otherwise.
                if (row >= rowCount || column >= columnCount) {
                    throw storm::exceptions::OutOfRangeException() << "Illegal call to SparseMatrixBuilder::addNextValue: adding entry at out-of-bounds position (" << row << ", " << column << ") in matrix of size (" << rowCount << ", " << columnCount << ").";
                }
            } else {
                if (rowCountSet) {
                    if (row >= rowCount) {
                        throw storm::exceptions::OutOfRangeException() << "Illegal call to SparseMatrixBuilder::addNextValue: adding entry at out-of-bounds row " << row << " in matrix with " << rowCount << " rows.";
                    }
                }
                if (columnCountSet) {
                    if (column >= columnCount) {
                        throw storm::exceptions::OutOfRangeException() << "Illegal call to SparseMatrixBuilder::addNextValue: adding entry at out-of-bounds column " << column << " in matrix with " << columnCount << " columns.";
                    }
                }
            }
            
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
                if (storagePreallocated) {
                    // If the storage was preallocated, we can access the elements in the vectors with the subscript
                    // operator.
                    for (IndexType i = lastRow + 1; i <= row; ++i) {
                        rowIndications[i] = currentEntryCount;
                    }
                } else {
                    // Otherwise, we need to push the correct values to the vectors, which might trigger reallocations.
                    for (IndexType i = lastRow + 1; i <= row; ++i) {
                        rowIndications.push_back(currentEntryCount);
                    }
                }
                
                if (!hasCustomRowGrouping) {
                    for (IndexType i = lastRow + 1; i <= row; ++i) {
                        rowGroupIndices.push_back(i);
                        ++currentRowGroup;
                    }
                }
                
                lastRow = row;
            }
            
            lastColumn = column;
            
            // Finally, set the element and increase the current size.
            if (storagePreallocated) {
                columnsAndValues[currentEntryCount] = std::make_pair(column, value);
            } else {
                columnsAndValues.emplace_back(column, value);
                if (!columnCountSet) {
                    columnCount = std::max(columnCount, column + 1);
                }
                if (!rowCountSet) {
                    rowCount = row + 1;
                }
            }
            ++currentEntryCount;
        }
        
        template<typename ValueType>
        void SparseMatrixBuilder<ValueType>::newRowGroup(IndexType startingRow) {
            if (!hasCustomRowGrouping) {
                throw storm::exceptions::InvalidStateException() << "Illegal call to SparseMatrix::newRowGroup: matrix was not created to have a custom row grouping.";
            } else if (rowGroupIndices.size() > 0 && startingRow < rowGroupIndices.back()) {
                throw storm::exceptions::InvalidStateException() << "Illegal call to SparseMatrix::newRowGroup: illegal row group with negative size.";
            }
            if (rowGroupCountSet) {
                rowGroupIndices[currentRowGroup] = startingRow;
                ++currentRowGroup;
            } else {
                rowGroupIndices.push_back(startingRow);
            }
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType> SparseMatrixBuilder<ValueType>::build(IndexType overriddenRowCount, IndexType overriddenColumnCount, IndexType overriddenRowGroupCount) {
            // Check whether it's safe to finalize the matrix and throw error otherwise.
            if (storagePreallocated && currentEntryCount != entryCount) {
                throw storm::exceptions::InvalidStateException() << "Illegal call to SparseMatrix::build: expected " << entryCount << " entries, but got " << currentEntryCount << " instead.";
            } else {
                // Fill in the missing entries in the row indices array, as there may be empty rows at the end.
                if (storagePreallocated) {
                    for (IndexType i = lastRow + 1; i < rowCount; ++i) {
                        rowIndications[i] = currentEntryCount;
                    }
                } else {
                    if (!rowCountSet) {
                        rowCount = std::max(overriddenRowCount, rowCount);
                    }
                    for (IndexType i = lastRow + 1; i < rowCount; ++i) {
                        rowIndications.push_back(currentEntryCount);
                    }
                }
                
                // We put a sentinel element at the last position of the row indices array. This eases iteration work,
                // as now the indices of row i are always between rowIndications[i] and rowIndications[i + 1], also for
                // the first and last row.
                if (storagePreallocated) {
                    rowIndications[rowCount] = currentEntryCount;
                } else {
                    rowIndications.push_back(currentEntryCount);
                    if (!columnCountSet) {
                        columnCount = std::max(columnCount, overriddenColumnCount);
                    }
                }
                
                entryCount = currentEntryCount;
                
                if (hasCustomRowGrouping && rowGroupCountSet) {
                    rowGroupIndices[rowGroupCount] = rowCount;
                } else {
                    if (!hasCustomRowGrouping) {
                        for (IndexType i = currentRowGroup; i < rowCount; ++i) {
                            rowGroupIndices.push_back(i + 1);
                        }
                    } else {
                        overriddenRowGroupCount = std::max(overriddenRowGroupCount, currentRowGroup + 1);
                        for (IndexType i = currentRowGroup; i < overriddenRowGroupCount; ++i) {
                            rowGroupIndices.push_back(rowCount);
                        }
                    }
                }
            }
            
            return SparseMatrix<ValueType>(columnCount, std::move(rowIndications), std::move(columnsAndValues), std::move(rowGroupIndices));
        }
        
        template<typename ValueType>
        void SparseMatrixBuilder<ValueType>::prepareInternalStorage() {
            // Only allocate the memory for the matrix contents if the dimensions of the matrix are already known.
            if (storagePreallocated) {
                columnsAndValues = std::vector<MatrixEntry<IndexType, ValueType>>(entryCount, MatrixEntry<IndexType, ValueType>(0, storm::utility::constantZero<ValueType>()));
                rowIndications = std::vector<IndexType>(rowCount + 1, 0);
            } else {
                rowIndications.push_back(0);
            }
            
            // Only allocate the memory for the row grouping of the matrix contents if the number of groups is already
            // known.
            if (hasCustomRowGrouping && rowGroupCountSet) {
                rowGroupIndices = std::vector<IndexType>(rowGroupCount + 1, 0);
            } else {
                if (hasCustomRowGrouping) {
                    // Nothing to do in this case
                } else {
                    rowGroupIndices.push_back(0);
                }
            }
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>::rows::rows(iterator begin, IndexType entryCount) : beginIterator(begin), entryCount(entryCount) {
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
        typename SparseMatrix<ValueType>::IndexType SparseMatrix<ValueType>::rows::getNumberOfEntries() const {
            return this->entryCount;
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>::const_rows::const_rows(const_iterator begin, IndexType entryCount) : beginIterator(begin), entryCount(entryCount) {
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
        typename SparseMatrix<ValueType>::IndexType SparseMatrix<ValueType>::const_rows::getNumberOfEntries() const {
            return this->entryCount;
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>::SparseMatrix() : rowCount(0), columnCount(0), entryCount(0), nonzeroEntryCount(0), columnsAndValues(), rowIndications(), rowGroupIndices() {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>::SparseMatrix(SparseMatrix<ValueType> const& other) : rowCount(other.rowCount), columnCount(other.columnCount), entryCount(other.entryCount), nonzeroEntryCount(other.nonzeroEntryCount), columnsAndValues(other.columnsAndValues), rowIndications(other.rowIndications), rowGroupIndices(other.rowGroupIndices) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>::SparseMatrix(SparseMatrix<ValueType>&& other) : rowCount(other.rowCount), columnCount(other.columnCount), entryCount(other.entryCount), nonzeroEntryCount(other.nonzeroEntryCount), columnsAndValues(std::move(other.columnsAndValues)), rowIndications(std::move(other.rowIndications)), rowGroupIndices(std::move(other.rowGroupIndices)) {
            // Now update the source matrix
            other.rowCount = 0;
            other.columnCount = 0;
            other.entryCount = 0;
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>::SparseMatrix(IndexType columnCount, std::vector<IndexType> const& rowIndications, std::vector<MatrixEntry<IndexType, ValueType>> const& columnsAndValues, std::vector<IndexType> const& rowGroupIndices) : rowCount(rowIndications.size() - 1), columnCount(columnCount), entryCount(columnsAndValues.size()), nonzeroEntryCount(0), columnsAndValues(columnsAndValues), rowIndications(rowIndications), rowGroupIndices(rowGroupIndices) {
            for (auto const& element : *this) {
                if (element.getValue() != storm::utility::constantZero<ValueType>()) {
                    ++this->nonzeroEntryCount;
                }
            }
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType>::SparseMatrix(IndexType columnCount, std::vector<IndexType>&& rowIndications, std::vector<MatrixEntry<IndexType, ValueType>>&& columnsAndValues, std::vector<IndexType>&& rowGroupIndices) : rowCount(rowIndications.size() - 1), columnCount(columnCount), entryCount(columnsAndValues.size()), nonzeroEntryCount(0), columnsAndValues(std::move(columnsAndValues)), rowIndications(std::move(rowIndications)), rowGroupIndices(std::move(rowGroupIndices)) {
            for (auto const& element : *this) {
                if (element.getValue() != storm::utility::constantZero<ValueType>()) {
                    ++this->nonzeroEntryCount;
                }
            }
        }
                
        template<typename ValueType>
        SparseMatrix<ValueType>& SparseMatrix<ValueType>::operator=(SparseMatrix<ValueType> const& other) {
            // Only perform assignment if source and target are not the same.
            if (this != &other) {
                rowCount = other.rowCount;
                columnCount = other.columnCount;
                entryCount = other.entryCount;
                nonzeroEntryCount = other.nonzeroEntryCount;
                
                columnsAndValues = other.columnsAndValues;
                rowIndications = other.rowIndications;
                rowGroupIndices = other.rowGroupIndices;
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
            equalityResult &= this->getColumnCount() == other.getColumnCount();
            equalityResult &= this->getRowGroupIndices() == other.getRowGroupIndices();
            
            // For the actual contents, we need to do a little bit more work, because we want to ignore elements that
            // are set to zero, please they may be represented implicitly in the other matrix.
            for (IndexType row = 0; row < this->getRowCount(); ++row) {
                for (const_iterator it1 = this->begin(row), ite1 = this->end(row), it2 = other.begin(row), ite2 = other.end(row); it1 != ite1 && it2 != ite2; ++it1, ++it2) {
                    // Skip over all zero entries in both matrices.
                    while (it1 != ite1 && it1->getValue() == storm::utility::constantZero<ValueType>()) {
                        ++it1;
                    }
                    while (it2 != ite2 && it2->getValue() == storm::utility::constantZero<ValueType>()) {
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
            }
            
            return equalityResult;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::IndexType SparseMatrix<ValueType>::getRowCount() const {
            return rowCount;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::IndexType SparseMatrix<ValueType>::getColumnCount() const {
            return columnCount;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::IndexType SparseMatrix<ValueType>::getEntryCount() const {
            return entryCount;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::IndexType SparseMatrix<ValueType>::getNonzeroEntryCount() const {
            return nonzeroEntryCount;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::IndexType SparseMatrix<ValueType>::getRowGroupCount() const {
            return rowGroupIndices.size() - 1;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::IndexType SparseMatrix<ValueType>::getRowGroupSize(IndexType group) const {
            return this->getRowGroupIndices()[group + 1] - this->getRowGroupIndices()[group];
        }
        
        template<typename ValueType>
        std::vector<typename SparseMatrix<ValueType>::IndexType> const& SparseMatrix<ValueType>::getRowGroupIndices() const {
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
                for (IndexType row = this->getRowGroupIndices()[rowGroup]; row < this->getRowGroupIndices()[rowGroup + 1]; ++row) {
                    makeRowDirac(row, rowGroup);
                }
            }
        }
        
        template<typename ValueType>
        void SparseMatrix<ValueType>::makeRowDirac(IndexType row, IndexType column) {
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
            columnValuePtr->setValue(storm::utility::constantOne<ValueType>());
            ++columnValuePtr;
            for (; columnValuePtr != columnValuePtrEnd; ++columnValuePtr) {
                columnValuePtr->setColumn(0);
                columnValuePtr->setValue(storm::utility::constantZero<ValueType>());
            }
        }
        
        template<typename ValueType>
        ValueType SparseMatrix<ValueType>::getConstrainedRowSum(IndexType row, storm::storage::BitVector const& constraint) const {
            ValueType result = storm::utility::constantZero<ValueType>();
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
            IndexType currentRowCount = 0;
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
                for (IndexType row = this->getRowGroupIndices()[rowGroup]; row < this->getRowGroupIndices()[rowGroup + 1]; ++row) {
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
                std::vector<IndexType> fakeRowGroupIndices(rowCount + 1);
                IndexType i = 0;
                for (std::vector<IndexType>::iterator it = fakeRowGroupIndices.begin(); it != fakeRowGroupIndices.end(); ++it, ++i) {
                    *it = i;
                }
                return getSubmatrix(rowConstraint, columnConstraint, fakeRowGroupIndices, insertDiagonalElements);
            }
        }
                
        template<typename ValueType>
        SparseMatrix<ValueType> SparseMatrix<ValueType>::getSubmatrix(storm::storage::BitVector const& rowGroupConstraint, storm::storage::BitVector const& columnConstraint, std::vector<IndexType> const& rowGroupIndices, bool insertDiagonalEntries) const {
            // First, we need to determine the number of entries and the number of rows of the submatrix.
            IndexType subEntries = 0;
            IndexType subRows = 0;
            for (auto index : rowGroupConstraint) {
                subRows += rowGroupIndices[index + 1] - rowGroupIndices[index];
                for (IndexType i = rowGroupIndices[index]; i < rowGroupIndices[index + 1]; ++i) {
                    bool foundDiagonalElement = false;
                    
                    for (const_iterator it = this->begin(i), ite = this->end(i); it != ite; ++it) {
                        if (columnConstraint.get(it->getColumn())) {
                            ++subEntries;
                            
                            if (index == it->getColumn()) {
                                foundDiagonalElement = true;
                            }
                        }
                    }
                    
                    // If requested, we need to reserve one entry more for inserting the diagonal zero entry.
                    if (insertDiagonalEntries && !foundDiagonalElement) {
                        ++subEntries;
                    }
                }
            }
            
            // Create and initialize resulting matrix.
            SparseMatrixBuilder<ValueType> matrixBuilder(subRows, columnConstraint.getNumberOfSetBits(), subEntries, true);
            
            // Create a temporary vector that stores for each index whose bit is set to true the number of bits that
            // were set before that particular index.
            std::vector<IndexType> bitsSetBeforeIndex;
            bitsSetBeforeIndex.reserve(columnCount);
            
            // Compute the information to fill this vector.
            IndexType lastIndex = 0;
            IndexType currentNumberOfSetBits = 0;
            
            // If we are requested to add missing diagonal entries, we need to make sure the corresponding rows are also
            // taken.
            storm::storage::BitVector columnBitCountConstraint = columnConstraint;
            if (insertDiagonalEntries) {
                columnBitCountConstraint |= rowGroupConstraint;
            }
            for (auto index : columnBitCountConstraint) {
                while (lastIndex <= index) {
                    bitsSetBeforeIndex.push_back(currentNumberOfSetBits);
                    ++lastIndex;
                }
                ++currentNumberOfSetBits;
            }
            
            // Copy over selected entries.
            IndexType rowCount = 0;
            for (auto index : rowGroupConstraint) {
                matrixBuilder.newRowGroup(rowCount);
                for (IndexType i = rowGroupIndices[index]; i < rowGroupIndices[index + 1]; ++i) {
                    bool insertedDiagonalElement = false;
                    
                    for (const_iterator it = this->begin(i), ite = this->end(i); it != ite; ++it) {
                        if (columnConstraint.get(it->getColumn())) {
                            if (index == it->getColumn()) {
                                insertedDiagonalElement = true;
                            } else if (insertDiagonalEntries && !insertedDiagonalElement && it->getColumn() > index) {
                                matrixBuilder.addNextValue(rowCount, bitsSetBeforeIndex[index], storm::utility::constantZero<ValueType>());
                                insertedDiagonalElement = true;
                            }
                            matrixBuilder.addNextValue(rowCount, bitsSetBeforeIndex[it->getColumn()], it->getValue());
                        }
                    }
                    if (insertDiagonalEntries && !insertedDiagonalElement) {
                        matrixBuilder.addNextValue(rowCount, bitsSetBeforeIndex[index], storm::utility::constantZero<ValueType>());
                    }
                    
                    ++rowCount;
                }
            }
            
            return matrixBuilder.build();
        }
        
        template<typename ValueType>
        SparseMatrix<ValueType> SparseMatrix<ValueType>::selectRowsFromRowGroups(std::vector<IndexType> const& rowGroupToRowIndexMapping, bool insertDiagonalEntries) const {
            // First, we need to count how many non-zero entries the resulting matrix will have and reserve space for
            // diagonal entries if requested.
            IndexType subEntries = 0;
            for (IndexType rowGroupIndex = 0, rowGroupIndexEnd = rowGroupToRowIndexMapping.size(); rowGroupIndex < rowGroupIndexEnd; ++rowGroupIndex) {
                // Determine which row we need to select from the current row group.
                IndexType rowToCopy = rowGroupIndices[rowGroupIndex] + rowGroupToRowIndexMapping[rowGroupIndex];
                
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
            for (IndexType rowGroupIndex = 0, rowGroupIndexEnd = rowGroupToRowIndexMapping.size(); rowGroupIndex < rowGroupIndexEnd; ++rowGroupIndex) {
                // Determine which row we need to select from the current row group.
                IndexType rowToCopy = rowGroupIndices[rowGroupIndex] + rowGroupToRowIndexMapping[rowGroupIndex];
                
                // Iterate through that row and copy the entries. This also inserts a zero element on the diagonal if
                // there is no entry yet.
                bool insertedDiagonalElement = false;
                for (const_iterator it = this->begin(rowToCopy), ite = this->end(rowToCopy); it != ite; ++it) {
                    if (it->getColumn() == rowGroupIndex) {
                        insertedDiagonalElement = true;
                    } else if (insertDiagonalEntries && !insertedDiagonalElement && it->getColumn() > rowGroupIndex) {
                        matrixBuilder.addNextValue(rowGroupIndex, rowGroupIndex, storm::utility::constantZero<ValueType>());
                        insertedDiagonalElement = true;
                    }
                    matrixBuilder.addNextValue(rowGroupIndex, it->getColumn(), it->getValue());
                }
                if (insertDiagonalEntries && !insertedDiagonalElement) {
                    matrixBuilder.addNextValue(rowGroupIndex, rowGroupIndex, storm::utility::constantZero<ValueType>());
                }
            }
            
            // Finalize created matrix and return result.
            return matrixBuilder.build();
        }
        
        template <typename ValueType>
        SparseMatrix<ValueType> SparseMatrix<ValueType>::transpose(bool joinGroups) const {
            IndexType rowCount = this->getColumnCount();
            IndexType columnCount = joinGroups ? this->getRowGroupCount() : this->getRowCount();
            IndexType entryCount = this->getEntryCount();
            
            std::vector<IndexType> rowIndications(rowCount + 1);
            std::vector<MatrixEntry<IndexType, ValueType>> columnsAndValues(entryCount);
            
            // First, we need to count how many entries each column has.
            for (IndexType group = 0; group < columnCount; ++group) {
                for (auto const& transition : joinGroups ? this->getRowGroup(group) : this->getRow(group)) {
                    if (transition.getValue() != storm::utility::constantZero<ValueType>()) {
                        ++rowIndications[transition.getColumn() + 1];
                    }
                }
            }
            
            // Now compute the accumulated offsets.
            for (IndexType i = 1; i < rowCount + 1; ++i) {
                rowIndications[i] = rowIndications[i - 1] + rowIndications[i];
            }
            
            // Create an array that stores the index for the next value to be added for
            // each row in the transposed matrix. Initially this corresponds to the previously
            // computed accumulated offsets.
            std::vector<IndexType> nextIndices = rowIndications;
            
            // Now we are ready to actually fill in the values of the transposed matrix.
            for (IndexType group = 0; group < columnCount; ++group) {
                for (auto const& transition : joinGroups ? this->getRowGroup(group) : this->getRow(group)) {
                    if (transition.getValue() != storm::utility::constantZero<ValueType>()) {
                        columnsAndValues[nextIndices[transition.getColumn()]] = std::make_pair(group, transition.getValue());
                        nextIndices[transition.getColumn()]++;
                    }
                }
            }
            
            std::vector<IndexType> rowGroupIndices(rowCount + 1);
            for (IndexType i = 0; i <= rowCount; ++i) {
                rowGroupIndices[i] = i;
            }
            
            storm::storage::SparseMatrix<ValueType> transposedMatrix(columnCount, std::move(rowIndications), std::move(columnsAndValues), std::move(rowGroupIndices));
                        
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
            ValueType one = storm::utility::constantOne<ValueType>();
            bool foundDiagonalElement = false;
            for (IndexType group = 0; group < this->getRowGroupCount(); ++group) {
                for (auto& entry : this->getRowGroup(group)) {
                    if (entry.getColumn() == group) {
                        entry.setValue(one - entry.getValue());
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
            for (IndexType group = 0; group < this->getRowGroupCount(); ++group) {
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
            for (IndexType group = 0; group < this->getRowGroupCount(); ++group) {
                for (auto& entry : this->getRowGroup(group)) {
                    if (entry.getColumn() == group) {
                        entry.setValue(storm::utility::constantZero<ValueType>());
                    }
                }
            }
        }
        
        template<typename ValueType>
        typename std::pair<storm::storage::SparseMatrix<ValueType>, storm::storage::SparseMatrix<ValueType>> SparseMatrix<ValueType>::getJacobiDecomposition() const {
            if (rowCount != columnCount) {
                throw storm::exceptions::InvalidArgumentException() << "Illegal call to SparseMatrix::invertDiagonal: matrix is non-square.";
            }
            storm::storage::SparseMatrix<ValueType> resultLU(*this);
            resultLU.deleteDiagonalEntries();
            
            SparseMatrixBuilder<ValueType> dInvBuilder(rowCount, columnCount, rowCount);
            
            // Copy entries to the appropriate matrices.
            for (IndexType rowNumber = 0; rowNumber < rowCount; ++rowNumber) {
                
                // Because the matrix may have several entries on the diagonal, we need to sum them before we are able
                // to invert the entry.
                ValueType diagonalValue = storm::utility::constantZero<ValueType>();
                for (const_iterator it = this->begin(rowNumber), ite = this->end(rowNumber); it != ite; ++it) {
                    if (it->getColumn() == rowNumber) {
                        diagonalValue += it->getValue();
                    } else if (it->getColumn() > rowNumber) {
                        break;
                    }
                }
                dInvBuilder.addNextValue(rowNumber, rowNumber, storm::utility::constantOne<ValueType>() / diagonalValue);
            }
            
            return std::make_pair(std::move(resultLU), dInvBuilder.build());
        }
        
        template<typename ValueType>
        std::vector<ValueType> SparseMatrix<ValueType>::getPointwiseProductRowSumVector(storm::storage::SparseMatrix<ValueType> const& otherMatrix) const {
            std::vector<ValueType> result(rowCount, storm::utility::constantZero<ValueType>());
            
            // Iterate over all elements of the current matrix and either continue with the next element in case the
            // given matrix does not have a non-zero element at this column position, or multiply the two entries and
            // add the result to the corresponding position in the vector.
            for (IndexType row = 0; row < rowCount && row < otherMatrix.rowCount; ++row) {
                for (const_iterator it1 = this->begin(row), ite1 = this->end(row), it2 = otherMatrix.begin(row), ite2 = otherMatrix.end(row); it1 != ite1 && it2 != ite2; ++it1) {
                    if (it1->getColumn() < it2->getColumn()) {
                        continue;
                    } else {
                        // If the precondition of this method (i.e. that the given matrix is a submatrix
                        // of the current one) was fulfilled, we know now that the two elements are in
                        // the same column, so we can multiply and add them to the row sum vector.
                        result[row] += it2->getValue() * it1->getValue();
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
            typename std::vector<IndexType>::const_iterator rowIterator = rowIndications.begin();
            typename std::vector<ValueType>::iterator resultIterator = result.begin();
            typename std::vector<ValueType>::iterator resultIteratorEnd = result.end();
            
            for (; resultIterator != resultIteratorEnd; ++rowIterator, ++resultIterator) {
                *resultIterator = storm::utility::constantZero<ValueType>();
                
                for (ite = this->begin() + *(rowIterator + 1); it != ite; ++it) {
                    *resultIterator += it->getValue() * vector[it->getColumn()];
                }
            }
        }

#ifdef STORM_HAVE_INTELTBB
        template<typename ValueType>
        void SparseMatrix<ValueType>::multiplyWithVectorParallel(std::vector<ValueType> const& vector, std::vector<ValueType>& result) const {
            tbb::parallel_for(tbb::blocked_range<IndexType>(0, result.size(), 10),
                              [&] (tbb::blocked_range<IndexType> const& range) {
                                  IndexType startRow = range.begin();
                                  IndexType endRow = range.end();
                                  const_iterator it = this->begin(startRow);
                                  const_iterator ite;
                                  std::vector<IndexType>::const_iterator rowIterator = this->rowIndications.begin() + startRow;
                                  std::vector<IndexType>::const_iterator rowIteratorEnd = this->rowIndications.begin() + endRow;
                                  typename std::vector<ValueType>::iterator resultIterator = result.begin() + startRow;
                                  typename std::vector<ValueType>::iterator resultIteratorEnd = result.begin() + endRow;
                                  
                                  for (; resultIterator != resultIteratorEnd; ++rowIterator, ++resultIterator) {
                                      *resultIterator = storm::utility::constantZero<ValueType>();
                                      
                                      for (ite = this->begin() + *(rowIterator + 1); it != ite; ++it) {
                                          *resultIterator += it->getValue() * vector[it->getColumn()];
                                      }
                                  }
                              });
        }
#endif

        template<typename ValueType>
        uint_fast64_t SparseMatrix<ValueType>::getSizeInMemory() const {
            uint_fast64_t size = sizeof(*this);
            
            // Add size of columns and values.
            size += sizeof(MatrixEntry<IndexType, ValueType>) * columnsAndValues.capacity();
            
            // Add row_indications size.
            size += sizeof(uint_fast64_t) * rowIndications.capacity();
            
            return size;
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::const_rows SparseMatrix<ValueType>::getRows(IndexType startRow, IndexType endRow) const {
            return const_rows(this->columnsAndValues.begin() + this->rowIndications[startRow], this->rowIndications[endRow + 1] - this->rowIndications[startRow]);
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::rows SparseMatrix<ValueType>::getRows(IndexType startRow, IndexType endRow) {
            return rows(this->columnsAndValues.begin() + this->rowIndications[startRow], this->rowIndications[endRow + 1] - this->rowIndications[startRow]);
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::const_rows SparseMatrix<ValueType>::getRow(IndexType row) const {
            return getRows(row, row);
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::rows SparseMatrix<ValueType>::getRow(IndexType row) {
            return getRows(row, row);
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::const_rows SparseMatrix<ValueType>::getRowGroup(IndexType rowGroup) const {
            return getRows(rowGroupIndices[rowGroup], rowGroupIndices[rowGroup + 1] - 1);
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::rows SparseMatrix<ValueType>::getRowGroup(IndexType rowGroup) {
            return getRows(rowGroupIndices[rowGroup], rowGroupIndices[rowGroup + 1] - 1);
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::const_iterator SparseMatrix<ValueType>::begin(IndexType row) const {
            return this->columnsAndValues.begin() + this->rowIndications[row];
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::iterator SparseMatrix<ValueType>::begin(IndexType row)  {
            return this->columnsAndValues.begin() + this->rowIndications[row];
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::const_iterator SparseMatrix<ValueType>::end(IndexType row) const {
            return this->columnsAndValues.begin() + this->rowIndications[row + 1];
        }
        
        template<typename ValueType>
        typename SparseMatrix<ValueType>::iterator SparseMatrix<ValueType>::end(IndexType row)  {
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
        ValueType SparseMatrix<ValueType>::getRowSum(IndexType row) const {
            ValueType sum = storm::utility::constantZero<ValueType>();
            for (const_iterator it = this->begin(row), ite = this->end(row); it != ite; ++it) {
                sum += it->getValue();
            }
            return sum;
        }
        
        template<typename ValueType>
        bool SparseMatrix<ValueType>::isSubmatrixOf(SparseMatrix<ValueType> const& matrix) const {
            // Check for matching sizes.
            if (this->getRowCount() != matrix.getRowCount()) return false;
            if (this->getColumnCount() != matrix.getColumnCount()) return false;
            if (this->getRowGroupIndices() != matrix.getRowGroupIndices()) return false;

            // Check the subset property for all rows individually.
            for (IndexType row = 0; row < this->getRowCount(); ++row) {
                for (const_iterator it1 = this->begin(row), ite1 = this->end(row), it2 = matrix.begin(row), ite2 = matrix.end(row); it1 != ite1; ++it1) {
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
            for (typename SparseMatrix<ValueType>::IndexType i = 0; i < matrix.getColumnCount(); ++i) {
                out << i << "\t";
            }
            out << std::endl;
            
            // Iterate over all row groups.
            for (typename SparseMatrix<ValueType>::IndexType group = 0; group < matrix.getRowGroupCount(); ++group) {
                out << "\t---- group " << group << "/" << (matrix.getRowGroupCount() - 1) << " ---- " << std::endl;
                for (typename SparseMatrix<ValueType>::IndexType i = matrix.getRowGroupIndices()[group]; i < matrix.getRowGroupIndices()[group + 1]; ++i) {
                    typename SparseMatrix<ValueType>::IndexType nextIndex = matrix.rowIndications[i];
                    
                    // Print the actual row.
                    out << i << "\t(\t";
                    typename SparseMatrix<ValueType>::IndexType currentRealIndex = 0;
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
            for (typename SparseMatrix<ValueType>::IndexType i = 0; i < matrix.getColumnCount(); ++i) {
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
        template class MatrixEntry<typename SparseMatrix<double>::IndexType, double>;
        template class SparseMatrixBuilder<double>;
        template class SparseMatrix<double>;
        template std::ostream& operator<<(std::ostream& out, SparseMatrix<double> const& matrix);
        template class MatrixEntry<typename SparseMatrix<int>::IndexType, int>;
        template class SparseMatrixBuilder<int>;
        template class SparseMatrix<int>;
        template std::ostream& operator<<(std::ostream& out, SparseMatrix<int> const& matrix);
        
    } // namespace storage
} // namespace storm



