#include <boost/functional/hash.hpp>

#include "src/storage/SparseMatrix.h"
#include "src/exceptions/InvalidStateException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
    namespace storage {
        
        template<typename T>
        template<typename ValueType>
        SparseMatrix<T>::BaseIterator<ValueType>::BaseIterator(ValueType* valuePtr, uint_fast64_t const* columnPtr) : valuePtr(valuePtr), columnPtr(columnPtr) {
            // Intentionally left empty.
        }

        template<typename T>
        template<typename ValueType>
        SparseMatrix<T>::BaseIterator<ValueType>::BaseIterator(SparseMatrix<T>::BaseIterator<ValueType> const& other) : valuePtr(other.valuePtr), columnPtr(other.columnPtr) {
            // Intentionally left empty.
        }

        template<typename T>
        template<typename ValueType>
        typename SparseMatrix<T>::template BaseIterator<ValueType>& SparseMatrix<T>::BaseIterator<ValueType>::operator=(BaseIterator<ValueType> const& other) {
            if (this != &other) {
                valuePtr = other.valuePtr,
                columnPtr = other.columnPtr;
            }
            return *this;
        }

        template<typename T>
        template<typename ValueType>
        SparseMatrix<T>::BaseIterator<ValueType>& SparseMatrix<T>::BaseIterator<ValueType>::operator++() {
            this->valuePtr++;
            this->columnPtr++;
            return *this;
        }
        
        template<typename T>
        template<typename ValueType>
        SparseMatrix<T>::BaseIterator<ValueType>& SparseMatrix<T>::BaseIterator<ValueType>::operator*() {
            return *this;
        }
        
        template<typename T>
        template<typename ValueType>
        bool SparseMatrix<T>::BaseIterator<ValueType>::operator!=(BaseIterator<ValueType> const& other) const {
            return this->valuePtr != other.valuePtr;
        }
        
        template<typename T>
        template<typename ValueType>
        bool SparseMatrix<T>::BaseIterator<ValueType>::operator==(BaseIterator<ValueType> const& other) const {
            return this->valuePtr == other.valuePtr;
        }

        template<typename T>
        template<typename ValueType>
        uint_fast64_t SparseMatrix<T>::BaseIterator<ValueType>::column() const {
            return *columnPtr;
        }
        
        template<typename T>
        template<typename ValueType>
        ValueType& SparseMatrix<T>::BaseIterator<ValueType>::value() const {
            return *valuePtr;
        }
        
        template<typename T>
        SparseMatrix<T>::rows::rows(T* valuePtr, uint_fast64_t const* columnPtr, uint_fast64_t entryCount) : valuePtr(valuePtr), columnPtr(columnPtr), entryCount(entryCount) {
            // Intentionally left empty.
        }
        
        template<typename T>
        typename SparseMatrix<T>::iterator SparseMatrix<T>::rows::begin() {
            return iterator(valuePtr, columnPtr);
        }
        
        template<typename T>
        typename SparseMatrix<T>::iterator SparseMatrix<T>::rows::end() {
            return iterator(valuePtr + entryCount, columnPtr + entryCount);
        }

        template<typename T>
        SparseMatrix<T>::const_rows::const_rows(T const* valuePtr, uint_fast64_t const* columnPtr, uint_fast64_t entryCount) : valuePtr(valuePtr), columnPtr(columnPtr), entryCount(entryCount) {
            // Intentionally left empty.
        }
        
        template<typename T>
        typename SparseMatrix<T>::const_iterator SparseMatrix<T>::const_rows::begin() const {
            return const_iterator(valuePtr, columnPtr);
        }
        
        template<typename T>
        typename SparseMatrix<T>::const_iterator SparseMatrix<T>::const_rows::end() const {
            return const_iterator(valuePtr + entryCount, columnPtr + entryCount);
        }
        
        template<typename T>
        SparseMatrix<T>::SparseMatrix(uint_fast64_t rows, uint_fast64_t columns, uint_fast64_t entries) : rowCount(rows), columnCount(columns), entryCount(entries), valueStorage(), columnIndications(), rowIndications(), internalStatus(UNINITIALIZED), currentEntryCount(0), lastRow(0), lastColumn(0) {
            storagePreallocated = rows != 0 && columns != 0 && entries != 0;
            prepareInternalStorage();
        }
        
        template<typename T>
        SparseMatrix<T>::SparseMatrix(uint_fast64_t size, uint_fast64_t entries) : SparseMatrix(size, size, entries) {
            // Intentionally left empty.
        }
        
        template<typename T>
        SparseMatrix<T>::SparseMatrix(SparseMatrix<T> const& other) : rowCount(other.rowCount), columnCount(other.columnCount), entryCount(other.entryCount), storagePreallocated(other.storagePreallocated), valueStorage(other.valueStorage), columnIndications(other.columnIndications), rowIndications(other.rowIndications), internalStatus(other.internalStatus), currentEntryCount(other.currentEntryCount), lastRow(other.lastRow), lastColumn(other.lastColumn) {
            // Intentionally left empty.
        }

        template<typename T>
        SparseMatrix<T>::SparseMatrix(SparseMatrix<T>&& other) : rowCount(other.rowCount), columnCount(other.columnCount), entryCount(other.entryCount), storagePreallocated(other.storagePreallocated), valueStorage(std::move(other.valueStorage)), columnIndications(std::move(other.columnIndications)), rowIndications(std::move(other.rowIndications)), internalStatus(other.internalStatus), currentEntryCount(other.currentEntryCount), lastRow(other.lastRow), lastColumn(other.lastColumn) {
            // Now update the source matrix
            other.rowCount = 0;
            other.columnCount = 0;
            other.entryCount = 0;
            other.storagePreallocated = false;
            other.internalStatus = MatrixStatus::UNINITIALIZED;
            other.currentEntryCount = 0;
            other.lastRow = 0;
            other.lastColumn = 0;
        }

        template<typename T>
        SparseMatrix<T>::SparseMatrix(uint_fast64_t columnCount, std::vector<uint_fast64_t> const& rowIndications, std::vector<uint_fast64_t> const& columnIndications, std::vector<T> const& values) : rowCount(rowIndications.size() - 1), columnCount(columnCount), entryCount(values.size()), valueStorage(values), columnIndications(columnIndications), rowIndications(rowIndications), internalStatus(INITIALIZED), currentEntryCount(0), lastRow(0), lastColumn(0) {
            // Intentionally left empty.
        }
        
        template<typename T>
        SparseMatrix<T>::SparseMatrix(uint_fast64_t columnCount, std::vector<uint_fast64_t>&& rowIndications, std::vector<uint_fast64_t>&& columnIndications, std::vector<T>&& values) : rowCount(rowIndications.size() - 1), columnCount(columnCount), entryCount(values.size()), valueStorage(std::move(values)), columnIndications(std::move(columnIndications)), rowIndications(std::move(rowIndications)), internalStatus(INITIALIZED), currentEntryCount(0), lastRow(0), lastColumn(0) {
            // Intentionally left empty.
        }
        
        template<typename T>
        SparseMatrix<T>& SparseMatrix<T>::operator=(SparseMatrix<T> const& other) {
            // Only perform assignment if source and target are not the same.
            if (this != &other) {
                rowCount = other.rowCount;
                columnCount = other.columnCount;
                entryCount = other.entryCount;
                
                valueStorage = other.valueStorage;
                columnIndications = other.columnIndications;
                rowIndications = other.rowIndications;
                
                internalStatus = other.internalStatus;
                currentEntryCount = other.currentEntryCount;
                lastRow = other.lastRow;
                lastColumn = other.lastColumn;
            }
            
            return *this;
        }
        
        template<typename T>
        SparseMatrix<T>& SparseMatrix<T>::operator=(SparseMatrix<T>&& other) {
            // Only perform assignment if source and target are not the same.
            if (this != &other) {
                rowCount = other.rowCount;
                columnCount = other.columnCount;
                entryCount = other.entryCount;
                
                valueStorage = std::move(other.valueStorage);
                columnIndications = std::move(other.columnIndications);
                rowIndications = std::move(other.rowIndications);
                
                internalStatus = other.internalStatus;
                currentEntryCount = other.currentEntryCount;
                lastRow = other.lastRow;
                lastColumn = other.lastColumn;
            }
            
            return *this;
        }
        
        template<typename T>
        void SparseMatrix<T>::addNextValue(uint_fast64_t row, uint_fast64_t column, T const& value) {
            // Depending on whether the internal data storage was preallocated or not, adding the value is done somewhat
            // differently.
            if (storagePreallocated) {
                // Check whether the given row and column positions are valid and throw error otherwise.
                if (row > rowCount || column > columnCount) {
                    throw storm::exceptions::OutOfRangeException() << "Illegal call to SparseMatrix::addNextValue: adding entry at out-of-bounds position (" << row << ", " << column << ") in matrix of size (" << rowCount << ", " << columnCount << ").";
                }
            }
            
            // Check that we did not move backwards wrt. the row.
            if (row < lastRow) {
                throw storm::exceptions::InvalidArgumentException() << "Illegal call to SparseMatrix::addNextValue: adding an element in row " << row << ", but an element in row " << lastRow << " has already been added.";
            }
            
            // Check that we did not move backwards wrt. to column.
            if (row == lastRow && column < lastColumn) {
                throw storm::exceptions::InvalidArgumentException() << "Illegal call to SparseMatrix::addNextValue: adding an element in column " << column << " in row " << row << ", but an element in column " << lastColumn << " has already been added in that row.";
            }
            
            // If we switched to another row, we have to adjust the missing entries in the row indices vector.
            if (row != lastRow) {
                if (storagePreallocated) {
                    // If the storage was preallocated, we can access the elements in the vectors with the subscript
                    // operator.
                    for (uint_fast64_t i = lastRow + 1; i <= row; ++i) {
                        rowIndications[i] = currentEntryCount;
                    }
                } else {
                    // Otherwise, we need to push the correct values to the vectors, which might trigger reallocations.
                    for (uint_fast64_t i = lastRow + 1; i <= row; ++i) {
                        rowIndications.push_back(currentEntryCount);
                    }
                }
                lastRow = row;
            }
            
            lastColumn = column;
            
            // Finally, set the element and increase the current size.
            if (storagePreallocated) {
                valueStorage[currentEntryCount] = value;
                columnIndications[currentEntryCount] = column;
                ++currentEntryCount;
            } else {
                valueStorage.push_back(value);
                columnIndications.push_back(column);
                columnCount = column + 1;
                rowCount = row + 1;
                ++currentEntryCount;
            }
        }
        
        template<typename T>
        void SparseMatrix<T>::finalize() {
            // Check whether it's safe to finalize the matrix and throw error otherwise.
            if (internalStatus == INITIALIZED) {
                throw storm::exceptions::InvalidStateException() << "Illegal call to SparseMatrix::finalize: finalizing an initialized matrix is forbidden.";
            } else if (storagePreallocated && currentEntryCount != entryCount) {
                throw storm::exceptions::InvalidStateException() << "Illegal call to SparseMatrix::finalize: expected " << entryCount << " entries, but got " << currentEntryCount << " instead.";
            } else {
                // Fill in the missing entries in the row indices array, as there may be empty rows at the end.
                if (storagePreallocated) {
                    for (uint_fast64_t i = lastRow + 1; i < rowCount; ++i) {
                        rowIndications[i] = currentEntryCount;
                    }
                } else {
                    for (uint_fast64_t i = lastRow + 1; i < rowCount; ++i) {
                        rowIndications.push_back(currentEntryCount);
                    }
                }
                
                // We put a sentinel element at the last position of the row indices array. This eases iteration work,
                // as now the indices of row i are always between rowIndications[i] and rowIndications[i + 1], also for
                // the first and last row.
                if (storagePreallocated) {
                    rowIndications[rowCount] = entryCount;
                } else {
                    rowIndications.push_back(entryCount);
                }

                internalStatus = INITIALIZED;
            }
        }
        
        template<typename T>
        uint_fast64_t SparseMatrix<T>::getRowCount() const {
            return rowCount;
        }
        
        template<typename T>
        uint_fast64_t SparseMatrix<T>::getColumnCount() const {
            return columnCount;
        }
        
        template<typename T>
        bool SparseMatrix<T>::isInitialized() {
            return internalStatus == INITIALIZED;
        }
        
        template<typename T>
        uint_fast64_t SparseMatrix<T>::getEntryCount() const {
            return entryCount;
        }
        
        template<typename T>
        void SparseMatrix<T>::makeRowsAbsorbing(storm::storage::BitVector const& rows) {
            for (auto row : rows) {
                makeRowAbsorbing(row, row);
            }
        }
        
        template<typename T>
        void SparseMatrix<T>::makeRowsAbsorbing(storm::storage::BitVector const& rowGroupConstraint, std::vector<uint_fast64_t> const& rowGroupIndices) {
            for (auto rowGroup : rowGroupConstraint) {
                for (uint_fast64_t row = rowGroupIndices[rowGroup]; row < rowGroupIndices[rowGroup + 1]; ++row) {
                    makeRowAbsorbing(row, rowGroup);
                }
            }
        }
        
        template<typename T>
        void SparseMatrix<T>::makeRowAbsorbing(const uint_fast64_t row, const uint_fast64_t column) {
            if (row > rowCount) {
                throw storm::exceptions::OutOfRangeException() << "Illegal call to SparseMatrix::makeRowAbsorbing: access to row " << row << " is out of bounds.";
            }
            
            // Iterate over the elements in the row that are not on the diagonal and set them to zero.
            T* valuePtr = valueStorage.data() + rowIndications[row];
            T* valuePtrEnd = valueStorage.data() + rowIndications[row + 1];
            uint_fast64_t* columnPtr = columnIndications.data() + rowIndications[row];
            
            // If the row has no elements in it, we cannot make it absorbing, because we would need to move all elements
            // in the vector of nonzeros otherwise.
            if (valuePtr >= valuePtrEnd) {
                throw storm::exceptions::InvalidStateException() << "Illegal call to SparseMatrix::makeRowAbsorbing: cannot make row " << row << " absorbing, but there is no entry in this row.";
            }
            
            // If there is at least one entry in this row, we can just set it to one, modify its column value to the
            // one given by the parameter and set all subsequent elements of this row to zero.
            *valuePtr = storm::utility::constantOne<T>();
            *columnPtr = column;
            ++valuePtr;
            ++columnPtr;
            for (; valuePtr != valuePtrEnd; ++valuePtr) {
                *valuePtr = storm::utility::constantZero<T>();
                *columnPtr = 0;
            }
        }
        
        template<typename T>
        T SparseMatrix<T>::getConstrainedRowSum(uint_fast64_t row, storm::storage::BitVector const& constraint) const {
            T result(0);
            for (uint_fast64_t i = rowIndications[row]; i < rowIndications[row + 1]; ++i) {
                if (constraint.get(columnIndications[i])) {
                    result += valueStorage[i];
                }
            }
            return result;
        }
        
        template<typename T>
        std::vector<T> SparseMatrix<T>::getConstrainedRowSumVector(storm::storage::BitVector const& rowConstraint, storm::storage::BitVector const& columnConstraint) const {
            std::vector<T> result(rowConstraint.getNumberOfSetBits());
            uint_fast64_t currentRowCount = 0;
            for (auto row : rowConstraint) {
                result[currentRowCount++] = getConstrainedRowSum(row, columnConstraint);
            }
            return result;
        }
        
        template<typename T>
        std::vector<T> SparseMatrix<T>::getConstrainedRowSumVector(storm::storage::BitVector const& rowGroupConstraint, std::vector<uint_fast64_t> const& rowGroupIndices, storm::storage::BitVector const& columnConstraint) const {
            std::vector<T> result;
            result.reserve(rowGroupConstraint.getNumberOfSetBits());
            for (auto rowGroup : rowGroupConstraint) {
                for (uint_fast64_t row = rowGroupIndices[rowGroup]; row < rowGroupIndices[rowGroup + 1]; ++row) {
                    result.push_back(getConstrainedRowSum(row, columnConstraint));
                }
            }
            return result;
        }
        
        template<typename T>
        SparseMatrix<T> SparseMatrix<T>::getSubmatrix(storm::storage::BitVector const& constraint) const {
            // Check whether we select at least some rows and columns.
            if (constraint.getNumberOfSetBits() == 0) {
                throw storm::exceptions::InvalidArgumentException() << "Illegal call to SparseMatrix::getSubmatrix: cannot create empty submatrix.";
            }
            
            // First, we need to determine the number of entries of the submatrix.
            uint_fast64_t subEntries = 0;
            for (auto rowIndex : constraint) {
                for (uint_fast64_t i = rowIndications[rowIndex]; i < rowIndications[rowIndex + 1]; ++i) {
                    if (constraint.get(columnIndications[i])) {
                        ++subEntries;
                    }
                }
            }
            
            // Create and initialize resulting matrix.
            SparseMatrix result(constraint.getNumberOfSetBits(), subEntries);
            
            // Create a temporary vecotr that stores for each index whose bit is set to true the number of bits that
            // were set before that particular index.
            std::vector<uint_fast64_t> bitsSetBeforeIndex;
            bitsSetBeforeIndex.reserve(columnCount);
            
            // Compute the information to fill this vector.
            uint_fast64_t lastIndex = 0;
            uint_fast64_t currentNumberOfSetBits = 0;
            for (auto index : constraint) {
                while (lastIndex <= index) {
                    bitsSetBeforeIndex.push_back(currentNumberOfSetBits);
                    ++lastIndex;
                }
                ++currentNumberOfSetBits;
            }
            
            // Copy over selected entries and use the previously computed vector to get the column offset.
            uint_fast64_t rowCount = 0;
            for (auto rowIndex : constraint) {
                for (uint_fast64_t i = rowIndications[rowIndex]; i < rowIndications[rowIndex + 1]; ++i) {
                    if (constraint.get(columnIndications[i])) {
                        result.addNextValue(rowCount, bitsSetBeforeIndex[columnIndications[i]], valueStorage[i]);
                    }
                }
                
                ++rowCount;
            }
            
            // Finalize submatrix and return result.
            result.finalize();
            return result;
        }
        
        template<typename T>
        SparseMatrix<T> SparseMatrix<T>::getSubmatrix(storm::storage::BitVector const& rowGroupConstraint, std::vector<uint_fast64_t> const& rowGroupIndices, bool insertDiagonalEntries) const {
            return getSubmatrix(rowGroupConstraint, rowGroupConstraint, rowGroupIndices, insertDiagonalEntries);
        }
        
        template<typename T>
        SparseMatrix<T> SparseMatrix<T>::getSubmatrix(storm::storage::BitVector const& rowGroupConstraint, storm::storage::BitVector const& columnConstraint, std::vector<uint_fast64_t> const& rowGroupIndices, bool insertDiagonalEntries) const {
            // First, we need to determine the number of entries and the number of rows of the submatrix.
            uint_fast64_t subEntries = 0;
            uint_fast64_t subRows = 0;
            for (auto index : rowGroupConstraint) {
                subRows += rowGroupIndices[index + 1] - rowGroupIndices[index];
                for (uint_fast64_t i = rowGroupIndices[index]; i < rowGroupIndices[index + 1]; ++i) {
                    bool foundDiagonalElement = false;
                    
                    for (uint_fast64_t j = rowIndications[i]; j < rowIndications[i + 1]; ++j) {
                        if (columnConstraint.get(columnIndications[j])) {
                            ++subEntries;
                            
                            if (index == columnIndications[j]) {
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
            SparseMatrix result(subRows, columnConstraint.getNumberOfSetBits(), subEntries);
            
            // Create a temporary vector that stores for each index whose bit is set to true the number of bits that
            // were set before that particular index.
            std::vector<uint_fast64_t> bitsSetBeforeIndex;
            bitsSetBeforeIndex.reserve(columnCount);
            
            // Compute the information to fill this vector.
            uint_fast64_t lastIndex = 0;
            uint_fast64_t currentNumberOfSetBits = 0;
            
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
            uint_fast64_t rowCount = 0;
            for (auto index : rowGroupConstraint) {
                for (uint_fast64_t i = rowGroupIndices[index]; i < rowGroupIndices[index + 1]; ++i) {
                    bool insertedDiagonalElement = false;
                    
                    for (uint_fast64_t j = rowIndications[i]; j < rowIndications[i + 1]; ++j) {
                        if (columnConstraint.get(columnIndications[j])) {
                            if (index == columnIndications[j]) {
                                insertedDiagonalElement = true;
                            } else if (insertDiagonalEntries && !insertedDiagonalElement && columnIndications[j] > index) {
                                result.addNextValue(rowCount, bitsSetBeforeIndex[index], storm::utility::constantZero<T>());
                                insertedDiagonalElement = true;
                            }
                            result.addNextValue(rowCount, bitsSetBeforeIndex[columnIndications[j]], valueStorage[j]);
                        }
                    }
                    if (insertDiagonalEntries && !insertedDiagonalElement) {
                        result.addNextValue(rowCount, bitsSetBeforeIndex[index], storm::utility::constantZero<T>());
                    }
                    
                    ++rowCount;
                }
            }
            
            result.finalize();
            return result;
        }
        
        template<typename T>
        SparseMatrix<T> SparseMatrix<T>::getSubmatrix(std::vector<uint_fast64_t> const& rowGroupToRowIndexMapping, std::vector<uint_fast64_t> const& rowGroupIndices, bool insertDiagonalEntries) const {
            // First, we need to count how many non-zero entries the resulting matrix will have and reserve space for
            // diagonal entries if requested.
            uint_fast64_t subEntries = 0;
            for (uint_fast64_t rowGroupIndex = 0, rowGroupIndexEnd = rowGroupToRowIndexMapping.size(); rowGroupIndex < rowGroupIndexEnd; ++rowGroupIndex) {
                // Determine which row we need to select from the current row group.
                uint_fast64_t rowToCopy = rowGroupIndices[rowGroupIndex] + rowGroupToRowIndexMapping[rowGroupIndex];
                
                // Iterate through that row and count the number of slots we have to reserve for copying.
                bool foundDiagonalElement = false;
                for (uint_fast64_t i = rowIndications[rowToCopy], rowEnd = rowIndications[rowToCopy + 1]; i < rowEnd; ++i) {
                    if (columnIndications[i] == rowGroupIndex) {
                        foundDiagonalElement = true;
                    }
                    ++subEntries;
                }
                if (insertDiagonalEntries && !foundDiagonalElement) {
                    ++subEntries;
                }
            }
            
            // Now create the matrix to be returned with the appropriate size.
            SparseMatrix<T> submatrix(rowGroupIndices.size() - 1, columnCount, subEntries);
            
            // Copy over the selected lines from the source matrix.
            for (uint_fast64_t rowGroupIndex = 0, rowGroupIndexEnd = rowGroupToRowIndexMapping.size(); rowGroupIndex < rowGroupIndexEnd; ++rowGroupIndex) {
                // Determine which row we need to select from the current row group.
                uint_fast64_t rowToCopy = rowGroupIndices[rowGroupIndex] + rowGroupToRowIndexMapping[rowGroupIndex];
                
                // Iterate through that row and copy the entries. This also inserts a zero element on the diagonal if
                // there is no entry yet.
                bool insertedDiagonalElement = false;
                for (uint_fast64_t i = rowIndications[rowToCopy], rowEnd = rowIndications[rowToCopy + 1]; i < rowEnd; ++i) {
                    if (columnIndications[i] == rowGroupIndex) {
                        insertedDiagonalElement = true;
                    } else if (insertDiagonalEntries && !insertedDiagonalElement && columnIndications[i] > rowGroupIndex) {
                        submatrix.addNextValue(rowGroupIndex, rowGroupIndex, storm::utility::constantZero<T>());
                        insertedDiagonalElement = true;
                    }
                    submatrix.addNextValue(rowGroupIndex, columnIndications[i], valueStorage[i]);
                }
                if (insertDiagonalEntries && !insertedDiagonalElement) {
                    submatrix.addNextValue(rowGroupIndex, rowGroupIndex, storm::utility::constantZero<T>());
                }
            }
            
            // Finalize created matrix and return result.
            submatrix.finalize();
            return submatrix;
        }
        
        template <typename T>
        SparseMatrix<T> SparseMatrix<T>::transpose() const {
            
            uint_fast64_t rowCount = this->columnCount;
            uint_fast64_t columnCount = this->rowCount;
            uint_fast64_t entryCount = this->entryCount;
            
            std::vector<uint_fast64_t> rowIndications(rowCount + 1);
            std::vector<uint_fast64_t> columnIndications(entryCount);
            std::vector<T> values(entryCount);
            
            // First, we need to count how many entries each column has.
            for (uint_fast64_t i = 0; i < this->rowCount; ++i) {
                for (auto const& transition : this->getRow(i)) {
                    if (transition.value() > 0) {
                        ++rowIndications[transition.column() + 1];
                    }
                }
            }
            
            // Now compute the accumulated offsets.
            for (uint_fast64_t i = 1; i < rowCount + 1; ++i) {
                rowIndications[i] = rowIndications[i - 1] + rowIndications[i];
            }
            
            // Create an array that stores the index for the next value to be added for
            // each row in the transposed matrix. Initially this corresponds to the previously
            // computed accumulated offsets.
            std::vector<uint_fast64_t> nextIndices = rowIndications;
            
            // Now we are ready to actually fill in the values of the transposed matrix.
            for (uint_fast64_t i = 0; i < this->rowCount; ++i) {
                for (auto& transition : this->getRow(i)) {
                    if (transition.value() > 0) {
                        values[nextIndices[transition.column()]] = transition.value();
                        columnIndications[nextIndices[transition.column()]++] = i;
                    }
                }
            }
            
            storm::storage::SparseMatrix<T> transposedMatrix(columnCount, std::move(rowIndications), std::move(columnIndications), std::move(values));
            
            return transposedMatrix;
        }
        
        template<typename T>
        void SparseMatrix<T>::convertToEquationSystem() {
            invertDiagonal();
            negateAllNonDiagonalEntries();
        }
        
        template<typename T>
        void SparseMatrix<T>::invertDiagonal() {
            // Check if the matrix is square, because only then it makes sense to perform this
            // transformation.
            if (this->getRowCount() != this->getColumnCount()) {
                throw storm::exceptions::InvalidArgumentException() << "SparseMatrix::invertDiagonal requires the Matrix to be square!";
            }
            
            // Now iterate over all rows and set the diagonal elements to the inverted value.
            // If there is a row without the diagonal element, an exception is thrown.
            T one = storm::utility::constantOne<T>();
            bool foundDiagonalElement = false;
            for (uint_fast64_t row = 0; row < rowCount; ++row) {
                uint_fast64_t rowStart = rowIndications[row];
                uint_fast64_t rowEnd = rowIndications[row + 1];
                foundDiagonalElement = false;
                while (rowStart < rowEnd) {
                    if (columnIndications[rowStart] == row) {
                        valueStorage[rowStart] = one - valueStorage[rowStart];
                        foundDiagonalElement = true;
                        break;
                    }
                    ++rowStart;
                }
                
                // Throw an exception if a row did not have an element on the diagonal.
                if (!foundDiagonalElement) {
                    throw storm::exceptions::InvalidArgumentException() << "SparseMatrix::invertDiagonal requires the Matrix to contain all diagonal entries!";
                }
            }
        }
        
        template<typename T>
        void SparseMatrix<T>::negateAllNonDiagonalEntries() {
            // Check if the matrix is square, because only then it makes sense to perform this transformation.
            if (this->getRowCount() != this->getColumnCount()) {
                throw storm::exceptions::InvalidArgumentException() << "Illegal call to SparseMatrix::invertDiagonal: matrix is non-square.";
            }
            
            // Iterate over all rows and negate all the elements that are not on the diagonal.
            for (uint_fast64_t row = 0; row < rowCount; ++row) {
                uint_fast64_t rowStart = rowIndications[row];
                uint_fast64_t rowEnd = rowIndications[row + 1];
                while (rowStart < rowEnd) {
                    if (columnIndications[rowStart] != row) {
                        valueStorage[rowStart] = -valueStorage[rowStart];
                    }
                    ++rowStart;
                }
            }
        }
        
        template<typename T>
        void SparseMatrix<T>::deleteDiagonalEntries() {
            // Check if the matrix is square, because only then it makes sense to perform this transformation.
            if (this->getRowCount() != this->getColumnCount()) {
                throw storm::exceptions::InvalidArgumentException() << "Illegal call to SparseMatrix::deleteDiagonalEntries: matrix is non-square.";
            }
            
            // Iterate over all rows and negate all the elements that are not on the diagonal.
            for (uint_fast64_t row = 0; row < rowCount; ++row) {
                uint_fast64_t rowStart = rowIndications[row];
                uint_fast64_t rowEnd = rowIndications[row + 1];
                while (rowStart < rowEnd) {
                    if (columnIndications[rowStart] == row) {
                        valueStorage[rowStart] = storm::utility::constantZero<T>();
                    }
                    ++rowStart;
                }
            }
        }
        
        template<typename T>
        typename std::pair<storm::storage::SparseMatrix<T>, storm::storage::SparseMatrix<T>> SparseMatrix<T>::getJacobiDecomposition() const {
            if (rowCount != columnCount) {
                throw storm::exceptions::InvalidArgumentException() << "Illegal call to SparseMatrix::invertDiagonal: matrix is non-square.";
            }
            storm::storage::SparseMatrix<T> resultLU(*this);
            resultLU.deleteDiagonalEntries();
            
            storm::storage::SparseMatrix<T> resultDinv(rowCount, columnCount, rowCount);
            
            // Copy entries to the appropriate matrices.
            for (uint_fast64_t rowNumber = 0; rowNumber < rowCount; ++rowNumber) {
                
                // Because the matrix may have several entries on the diagonal, we need to sum them before we are able
                // to invert the entry.
                T diagonalValue = storm::utility::constantZero<T>();
                for (const_iterator rowIt = this->begin(rowNumber), rowIte = this->end(rowNumber); rowIt != rowIte; ++rowIt) {
                    if (rowIt.column() == rowNumber) {
                        diagonalValue += rowIt.value();
                    } else if (rowIt.column() > rowNumber) {
                        break;
                    }
                }
                resultDinv.addNextValue(rowNumber, rowNumber, storm::utility::constantOne<T>() / diagonalValue);
            }
            resultDinv.finalize();
            
            return std::make_pair(std::move(resultLU), std::move(resultDinv));
        }
        
        template<typename T>
        std::vector<T> SparseMatrix<T>::getPointwiseProductRowSumVector(storm::storage::SparseMatrix<T> const& otherMatrix) const {
            std::vector<T> result(rowCount, storm::utility::constantZero<T>());
            
            // Iterate over all elements of the current matrix and either continue with the next element in case the
            // given matrix does not have a non-zero element at this column position, or multiply the two entries and
            // add the result to the corresponding position in the vector.
            for (uint_fast64_t row = 0; row < rowCount && row < otherMatrix.rowCount; ++row) {
                for (uint_fast64_t element = rowIndications[row], nextOtherElement = otherMatrix.rowIndications[row]; element < rowIndications[row + 1] && nextOtherElement < otherMatrix.rowIndications[row + 1]; ++element) {
                    if (columnIndications[element] < otherMatrix.columnIndications[nextOtherElement]) {
                        continue;
                    } else {
                        // If the precondition of this method (i.e. that the given matrix is a submatrix
                        // of the current one) was fulfilled, we know now that the two elements are in
                        // the same column, so we can multiply and add them to the row sum vector.
                        result[row] += otherMatrix.valueStorage[nextOtherElement] * valueStorage[element];
                        ++nextOtherElement;
                    }
                }
            }
            
            return result;
        }
        
        
        template<typename T>
        void SparseMatrix<T>::multiplyWithVector(std::vector<T> const& vector, std::vector<T>& result) const {
#ifdef STORM_HAVE_INTELTBB
            tbb::parallel_for(tbb::blocked_range<uint_fast64_t>(0, result.size()), tbbHelper_MatrixRowVectorScalarProduct<storm::storage::SparseMatrix<T>, std::vector<T>, T>(this, &vector, &result));
#else
            typename std::vector<T>::const_iterator valueIterator = valueStorage.begin();
            typename std::vector<T>::const_iterator valueIteratorEnd;
            typename std::vector<uint_fast64_t>::const_iterator columnIterator = columnIndications.begin();
            typename std::vector<uint_fast64_t>::const_iterator rowIterator = rowIndications.begin();
            typename std::vector<uint_fast64_t>::const_iterator rowIteratorEnd = rowIndications.end();
            typename std::vector<T>::iterator resultIterator = result.begin();
            typename std::vector<T>::iterator resultIteratorEnd = result.end();
            
            for (; resultIterator != resultIteratorEnd; ++rowIterator, ++resultIterator) {
                *resultIterator = storm::utility::constantZero<T>();
                
                for (valueIteratorEnd = valueIterator + (*(rowIterator + 1) - *rowIterator); valueIterator != valueIteratorEnd; ++valueIterator, ++columnIterator) {
                    *resultIterator += *valueIterator * vector[*columnIterator];
                }
            }
#endif
        }
        
        template<typename T>
        uint_fast64_t SparseMatrix<T>::getSizeInMemory() const {
            uint_fast64_t size = sizeof(*this);
            
            // Add value_storage size.
            size += sizeof(T) * valueStorage.capacity();
            
            // Add column_indications size.
            size += sizeof(uint_fast64_t) * columnIndications.capacity();
            
            // Add row_indications size.
            size += sizeof(uint_fast64_t) * rowIndications.capacity();
            
            return size;
        }
        
        template<typename T>
        typename SparseMatrix<T>::const_rows SparseMatrix<T>::getRows(uint_fast64_t startRow, uint_fast64_t endRow) const {
            return const_rows(this->valueStorage.data() + this->rowIndications[startRow], this->columnIndications.data() + this->rowIndications[startRow], this->rowIndications[endRow + 1] - this->rowIndications[startRow]);
        }

        template<typename T>
        typename SparseMatrix<T>::rows SparseMatrix<T>::getRows(uint_fast64_t startRow, uint_fast64_t endRow) {
            return rows(this->valueStorage.data() + this->rowIndications[startRow], this->columnIndications.data() + this->rowIndications[startRow], this->rowIndications[endRow + 1] - this->rowIndications[startRow]);
        }

        template<typename T>
        typename SparseMatrix<T>::const_rows SparseMatrix<T>::getRow(uint_fast64_t row) const {
            return getRows(row, row);
        }

        template<typename T>
        typename SparseMatrix<T>::rows SparseMatrix<T>::getRow(uint_fast64_t row) {
            return getRows(row, row);
        }

        template<typename T>
        typename SparseMatrix<T>::const_iterator SparseMatrix<T>::begin(uint_fast64_t row) const {
            return const_iterator(this->valueStorage.data() + this->rowIndications[row], this->columnIndications.data() + this->rowIndications[row]);
        }

        template<typename T>
        typename SparseMatrix<T>::iterator SparseMatrix<T>::begin(uint_fast64_t row)  {
            return iterator(this->valueStorage.data() + this->rowIndications[row], this->columnIndications.data() + this->rowIndications[row]);
        }
        
        template<typename T>
        typename SparseMatrix<T>::const_iterator SparseMatrix<T>::end(uint_fast64_t row) const {
            return const_iterator(this->valueStorage.data() + this->rowIndications[row + 1], this->columnIndications.data() + this->rowIndications[row + 1]);
        }
        
        template<typename T>
        typename SparseMatrix<T>::iterator SparseMatrix<T>::end(uint_fast64_t row)  {
            return iterator(this->valueStorage.data() + this->rowIndications[row + 1], this->columnIndications.data() + this->rowIndications[row + 1]);
        }

        template<typename T>
        typename SparseMatrix<T>::const_iterator SparseMatrix<T>::end() const {
            return const_iterator(this->valueStorage.data() + this->rowIndications[rowCount], this->columnIndications.data() + this->rowIndications[rowCount]);
        }
        
        template<typename T>
        typename SparseMatrix<T>::iterator SparseMatrix<T>::end()  {
            return iterator(this->valueStorage.data() + this->rowIndications[rowCount], this->columnIndications.data() + this->rowIndications[rowCount]);
        }
                
        template<typename T>
        T SparseMatrix<T>::getRowSum(uint_fast64_t row) const {
            T sum = storm::utility::constantZero<T>();
            for (typename std::vector<T>::const_iterator valueIterator = valueStorage.begin() + rowIndications[row], valueIteratorEnd = valueStorage.begin() + rowIndications[row + 1]; valueIterator != valueIteratorEnd; ++valueIterator) {
                sum += *valueIterator;
            }
            return sum;
        }
        
        template<typename T>
        bool SparseMatrix<T>::isSubmatrixOf(SparseMatrix<T> const& matrix) const {
            // Check for matching sizes.
            if (this->getRowCount() != matrix.getRowCount()) return false;
            if (this->getColumnCount() != matrix.getColumnCount()) return false;
            
            // Check the subset property for all rows individually.
            for (uint_fast64_t row = 0; row < this->getRowCount(); ++row) {
                for (uint_fast64_t elem = rowIndications[row], elem2 = matrix.rowIndications[row]; elem < rowIndications[row + 1] && elem < matrix.rowIndications[row + 1]; ++elem) {
                    // Skip over all entries of the other matrix that are before the current entry in the current matrix.
                    while (elem2 < matrix.rowIndications[row + 1] && matrix.columnIndications[elem2] < columnIndications[elem]) {
                        ++elem2;
                    }
                    if (!(elem2 < matrix.rowIndications[row + 1]) || columnIndications[elem] != matrix.columnIndications[elem2]) {
                        return false;
                    }
                }
            }
            return true;
        }
        
        template<typename T>
        std::ostream& operator<<(std::ostream& out, SparseMatrix<T> const& matrix) {
            // Print column numbers in header.
            out << "\t\t";
            for (uint_fast64_t i = 0; i < matrix.columnCount; ++i) {
                out << i << "\t";
            }
            out << std::endl;
            
            // Iterate over all rows.
            for (uint_fast64_t i = 0; i < matrix.rowCount; ++i) {
                uint_fast64_t nextIndex = matrix.rowIndications[i];
                
                // Print the actual row.
                out << i << "\t(\t";
                uint_fast64_t currentRealIndex = 0;
                while (currentRealIndex < matrix.columnCount) {
                    if (nextIndex < matrix.rowIndications[i + 1] && currentRealIndex == matrix.columnIndications[nextIndex]) {
                        out << matrix.valueStorage[nextIndex] << "\t";
                        ++nextIndex;
                    } else {
                        out << "0\t";
                    }
                    ++currentRealIndex;
                }
                out << "\t)\t" << i << std::endl;
            }
            
            // Print column numbers in footer.
            out << "\t\t";
            for (uint_fast64_t i = 0; i < matrix.columnCount; ++i) {
                out << i << "\t";
            }
            out << std::endl;
            
            return out;
        }
        
        template<typename T>
        std::size_t SparseMatrix<T>::hash() const {
            std::size_t result = 0;
            
            boost::hash_combine(result, rowCount);
            boost::hash_combine(result, columnCount);
            boost::hash_combine(result, entryCount);
            boost::hash_combine(result, internalStatus);
            boost::hash_combine(result, boost::hash_range(valueStorage.begin(), valueStorage.end()));
            boost::hash_combine(result, boost::hash_range(columnIndications.begin(), columnIndications.end()));
            boost::hash_combine(result, boost::hash_range(rowIndications.begin(), rowIndications.end()));
            
            return result;
        }
        
        template<typename T>
        void SparseMatrix<T>::prepareInternalStorage() {
            // Only allocate the memory if the dimensions of the matrix are already known.
            if (storagePreallocated) {
                valueStorage = std::vector<T>(entryCount, storm::utility::constantZero<T>());
                columnIndications = std::vector<uint_fast64_t>(entryCount, 0);
                rowIndications = std::vector<uint_fast64_t>(rowCount + 1, 0);
            }
        }
        
        // Explicitly instantiate the matrix and the nested classes.
        template class SparseMatrix<double>::BaseIterator<double const>;
        template class SparseMatrix<double>::BaseIterator<double>;
        template class SparseMatrix<double>;
        template class SparseMatrix<int>::BaseIterator<int const>;
        template class SparseMatrix<int>::BaseIterator<int>;
        template class SparseMatrix<int>;
        
#ifdef STORM_HAVE_INTELTBB
        
        template <typename M, typename V, typename T>
        tbbHelper_MatrixRowVectorScalarProduct<typename M, typename V, typename T>::tbbHelper_MatrixRowVectorScalarProduct(M const* matrixA, V const* vectorX, V * resultVector) : matrixA(matrixA), vectorX(vectorX), resultVector(resultVector) {}
        
        template <typename M, typename V, typename T>
        void tbbHelper_MatrixRowVectorScalarProduct<typename M, typename V, typename T>::operator() (const tbb::blocked_range<uint_fast64_t>& r) const {
            for (uint_fast64_t row = r.begin(); row < r.end(); ++row) {
                uint_fast64_t index = matrixA->rowIndications.at(row);
                uint_fast64_t indexEnd = matrixA->rowIndications.at(row + 1);
                
                // Initialize the result to be 0.
                T element = storm::utility::constantZero<T>();
                
                for (; index != indexEnd; ++index) {
                    element += matrixA->valueStorage.at(index) * vectorX->at(matrixA->columnIndications.at(index));
                }
                
                // Write back to the result Vector
                resultVector->at(row) = element;
            }
        }
        
        // Explicit instanciations of specializations of this template here.
        template class tbbHelper_MatrixRowVectorScalarProduct<storm::storage::SparseMatrix<double>, std::vector<double>, double>;
        
#endif
        
        
    } // namespace storage
} // namespace storm



