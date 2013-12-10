#include <boost/functional/hash.hpp>

#include "src/storage/SparseMatrix.h"
#include "src/exceptions/InvalidStateException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
    namespace storage {
        
        template<typename T>
        SparseMatrixBuilder<T>::SparseMatrixBuilder(uint_fast64_t rows, uint_fast64_t columns, uint_fast64_t entries) : rowCountSet(rows != 0), rowCount(rows), columnCountSet(columns != 0), columnCount(columns), entryCount(entries), storagePreallocated(rows != 0 && columns != 0 && entries != 0), columnsAndValues(), rowIndications(), currentEntryCount(0), lastRow(0), lastColumn(0) {
            this->prepareInternalStorage();
        }
        
        template<typename T>
        void SparseMatrixBuilder<T>::addNextValue(uint_fast64_t row, uint_fast64_t column, T const& value) {
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
                columnsAndValues[currentEntryCount] = std::make_pair(column, value);
            } else {
                columnsAndValues.emplace_back(column, value);
                if (!columnCountSet) {
                    columnCount = column + 1;
                }
                if (!rowCountSet) {
                    rowCount = row + 1;
                }
            }
            ++currentEntryCount;
            
        }
        
        template<typename T>
        SparseMatrix<T> SparseMatrixBuilder<T>::build(uint_fast64_t overriddenRowCount, uint_fast64_t overriddenColumnCount) {            
            // Check whether it's safe to finalize the matrix and throw error otherwise.
            if (storagePreallocated && currentEntryCount != entryCount) {
                throw storm::exceptions::InvalidStateException() << "Illegal call to SparseMatrix::finalize: expected " << entryCount << " entries, but got " << currentEntryCount << " instead.";
            } else {
                // Fill in the missing entries in the row indices array, as there may be empty rows at the end.
                if (storagePreallocated) {
                    for (uint_fast64_t i = lastRow + 1; i < rowCount; ++i) {
                        rowIndications[i] = currentEntryCount;
                    }
                } else {
                    if (!rowCountSet) {
                        rowCount = std::max(overriddenRowCount, rowCount);
                    }
                    for (uint_fast64_t i = lastRow + 1; i < rowCount; ++i) {
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
            }
            
            return SparseMatrix<T>(columnCount, std::move(rowIndications), std::move(columnsAndValues));
        }
        
        template<typename T>
        void SparseMatrixBuilder<T>::prepareInternalStorage() {
            // Only allocate the memory if the dimensions of the matrix are already known.
            if (storagePreallocated) {
                columnsAndValues = std::vector<std::pair<uint_fast64_t, T>>(entryCount, std::make_pair(0, storm::utility::constantZero<T>()));
                rowIndications = std::vector<uint_fast64_t>(rowCount + 1, 0);
            } else {
                rowIndications.push_back(0);
            }
        }
        
        template<typename T>
        SparseMatrix<T>::rows::rows(iterator begin, uint_fast64_t entryCount) : beginIterator(begin), entryCount(entryCount) {
            // Intentionally left empty.
        }
        
        template<typename T>
        typename SparseMatrix<T>::iterator SparseMatrix<T>::rows::begin() {
            return beginIterator;
        }
        
        template<typename T>
        typename SparseMatrix<T>::iterator SparseMatrix<T>::rows::end() {
            return beginIterator + entryCount;
        }
        
        template<typename T>
        SparseMatrix<T>::const_rows::const_rows(const_iterator begin, uint_fast64_t entryCount) : beginIterator(begin), entryCount(entryCount) {
            // Intentionally left empty.
        }
        
        template<typename T>
        typename SparseMatrix<T>::const_iterator SparseMatrix<T>::const_rows::begin() const {
            return beginIterator;
        }
        
        template<typename T>
        typename SparseMatrix<T>::const_iterator SparseMatrix<T>::const_rows::end() const {
            return beginIterator + entryCount;
        }
        
        template<typename T>
        SparseMatrix<T>::SparseMatrix() : rowCount(0), columnCount(0), entryCount(0), columnsAndValues(), rowIndications() {
            // Intentionally left empty.
        }
        
        template<typename T>
        SparseMatrix<T>::SparseMatrix(SparseMatrix<T> const& other) : rowCount(other.rowCount), columnCount(other.columnCount), entryCount(other.entryCount), columnsAndValues(other.columnsAndValues), rowIndications(other.rowIndications) {
            // Intentionally left empty.
        }
        
        template<typename T>
        SparseMatrix<T>::SparseMatrix(SparseMatrix<T>&& other) : rowCount(other.rowCount), columnCount(other.columnCount), entryCount(other.entryCount), columnsAndValues(std::move(other.columnsAndValues)), rowIndications(std::move(other.rowIndications)) {
            // Now update the source matrix
            other.rowCount = 0;
            other.columnCount = 0;
            other.entryCount = 0;
        }
        
        template<typename T>
        SparseMatrix<T>::SparseMatrix(uint_fast64_t columnCount, std::vector<uint_fast64_t> const& rowIndications, std::vector<std::pair<uint_fast64_t, T>> const& columnsAndValues) : rowCount(rowIndications.size() - 1), columnCount(columnCount), entryCount(columnsAndValues.size()), columnsAndValues(columnsAndValues), rowIndications(rowIndications) {
            // Intentionally left empty.
        }
        
        template<typename T>
        SparseMatrix<T>::SparseMatrix(uint_fast64_t columnCount, std::vector<uint_fast64_t> const& rowIndications, std::vector<uint_fast64_t> const& columnIndications, std::vector<T> const& values) : rowCount(rowIndications.size() - 1), columnCount(columnCount), entryCount(values.size()), columnsAndValues(), rowIndications(rowIndications) {
            if (columnIndications.size() != values.size()) {
                throw storm::exceptions::InvalidArgumentException() << "Illegal call to SparseMatrix::SparseMatrix: value and column vector length mismatch.";
            }
            
            // Preserve enough storage to avoid reallocations.
            columnsAndValues.reserve(values.size());
            
            // Now zip the two vectors for columns and values.
            typename std::vector<uint_fast64_t>::const_iterator columnIt = columnIndications.begin();
            for (typename std::vector<T>::const_iterator valueIt = values.begin(), valueIte = values.end(); valueIt != valueIte; ++valueIt, ++columnIt) {
                columnsAndValues.emplace_back(*columnIt, *valueIt);
            }
        }
        
        template<typename T>
        SparseMatrix<T>::SparseMatrix(uint_fast64_t columnCount, std::vector<uint_fast64_t>&& rowIndications, std::vector<std::pair<uint_fast64_t, T>>&& columnsAndValues) : rowCount(rowIndications.size() - 1), columnCount(columnCount), entryCount(columnsAndValues.size()), columnsAndValues(std::move(columnsAndValues)), rowIndications(std::move(rowIndications)) {
            // Intentionally left empty.
        }
        
        template<typename T>
        SparseMatrix<T>::SparseMatrix(uint_fast64_t columnCount, std::vector<uint_fast64_t>&& rowIndications, std::vector<uint_fast64_t>&& columnIndications, std::vector<T>&& values) : rowCount(rowIndications.size() - 1), columnCount(columnCount), entryCount(values.size()), columnsAndValues(), rowIndications(std::move(rowIndications)) {
            if (columnIndications.size() != values.size()) {
                throw storm::exceptions::InvalidArgumentException() << "Illegal call to SparseMatrix::SparseMatrix: value and column vector length mismatch.";
            }
            
            // Preserve enough storage to avoid reallocations.
            columnsAndValues.reserve(values.size());
            
            // Now zip the two vectors for columns and values.
            typename std::vector<uint_fast64_t>::const_iterator columnIt = columnIndications.begin();
            for (typename std::vector<T>::const_iterator valueIt = values.begin(), valueIte = values.end(); valueIt != valueIte; ++valueIt, ++columnIt) {
                columnsAndValues.emplace_back(*columnIt, *valueIt);
            }
        }
        
        template<typename T>
        SparseMatrix<T>& SparseMatrix<T>::operator=(SparseMatrix<T> const& other) {
            // Only perform assignment if source and target are not the same.
            if (this != &other) {
                rowCount = other.rowCount;
                columnCount = other.columnCount;
                entryCount = other.entryCount;
                
                columnsAndValues = other.columnsAndValues;
                rowIndications = other.rowIndications;
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
                
                columnsAndValues = std::move(other.columnsAndValues);
                rowIndications = std::move(other.rowIndications);
            }
            
            return *this;
        }
        
        template<typename T>
        bool SparseMatrix<T>::operator==(SparseMatrix<T> const& other) const {
            if (this == &other) {
                return true;
            }
            
            bool equalityResult = true;
            
            equalityResult &= rowCount == other.rowCount;
            equalityResult &= columnCount == other.columnCount;
            
            // For the actual contents, we need to do a little bit more work, because we want to ignore elements that
            // are set to zero, please they may be represented implicitly in the other matrix.
            for (uint_fast64_t row = 0; row < this->getRowCount(); ++row) {
                for (const_iterator it1 = this->begin(row), ite1 = this->end(row), it2 = other.begin(row), ite2 = other.end(row); it1 != ite1 && it2 != ite2; ++it1, ++it2) {
                    // Skip over all zero entries in both matrices.
                    while (it1 != ite1 && it1->second == storm::utility::constantZero<T>()) {
                        ++it1;
                    }
                    while (it2 != ite2 && it2->second == storm::utility::constantZero<T>()) {
                        ++it2;
                    }
                    if ((it1 == ite1) || (it2 == ite2)) {
                        equalityResult = (it1 == ite1) ^ (it2 == ite2);
                        break;
                    } else {
                        if (it1->first != it2->first || it1->second != it2->second) {
                            equalityResult = false;
                            break;
                        }
                    }
                }
            }
            
            return equalityResult;
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
        void SparseMatrix<T>::makeRowAbsorbing(uint_fast64_t row, uint_fast64_t column) {
            if (row > rowCount) {
                throw storm::exceptions::OutOfRangeException() << "Illegal call to SparseMatrix::makeRowAbsorbing: access to row " << row << " is out of bounds.";
            }
            
            iterator columnValuePtr = this->begin(row);
            iterator columnValuePtrEnd = this->end(row);
            
            // If the row has no elements in it, we cannot make it absorbing, because we would need to move all elements
            // in the vector of nonzeros otherwise.
            if (columnValuePtr >= columnValuePtrEnd) {
                throw storm::exceptions::InvalidStateException() << "Illegal call to SparseMatrix::makeRowAbsorbing: cannot make row " << row << " absorbing, but there is no entry in this row.";
            }
            
            // If there is at least one entry in this row, we can just set it to one, modify its column value to the
            // one given by the parameter and set all subsequent elements of this row to zero.
            columnValuePtr->first = column;
            columnValuePtr->second = storm::utility::constantOne<T>();
            ++columnValuePtr;
            for (; columnValuePtr != columnValuePtrEnd; ++columnValuePtr) {
                columnValuePtr->first = 0;
                columnValuePtr->second = storm::utility::constantZero<T>();
            }
        }
        
        template<typename T>
        T SparseMatrix<T>::getConstrainedRowSum(uint_fast64_t row, storm::storage::BitVector const& constraint) const {
            T result(0);
            for (const_iterator it = this->begin(row), ite = this->end(row); it != ite; ++it) {
                if (constraint.get(it->first)) {
                    result += it->second;
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
            // Create a fake row grouping to reduce this to a call to a more general method.
            std::vector<uint_fast64_t> rowGroupIndices(rowCount + 1);
            uint_fast64_t i = 0;
            for (std::vector<uint_fast64_t>::iterator it = rowGroupIndices.begin(); it != rowGroupIndices.end(); ++it, ++i) {
                *it = i;
            }
            return getSubmatrix(constraint, constraint, rowGroupIndices);
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
                    
                    for (const_iterator it = this->begin(i), ite = this->end(i); it != ite; ++it) {
                        if (columnConstraint.get(it->first)) {
                            ++subEntries;
                            
                            if (index == it->first) {
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
            SparseMatrixBuilder<T> matrixBuilder(subRows, columnConstraint.getNumberOfSetBits(), subEntries);
            
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
                    
                    for (const_iterator it = this->begin(i), ite = this->end(i); it != ite; ++it) {
                        if (columnConstraint.get(it->first)) {
                            if (index == it->first) {
                                insertedDiagonalElement = true;
                            } else if (insertDiagonalEntries && !insertedDiagonalElement && it->first > index) {
                                matrixBuilder.addNextValue(rowCount, bitsSetBeforeIndex[index], storm::utility::constantZero<T>());
                                insertedDiagonalElement = true;
                            }
                            matrixBuilder.addNextValue(rowCount, bitsSetBeforeIndex[it->first], it->second);
                        }
                    }
                    if (insertDiagonalEntries && !insertedDiagonalElement) {
                        matrixBuilder.addNextValue(rowCount, bitsSetBeforeIndex[index], storm::utility::constantZero<T>());
                    }
                    
                    ++rowCount;
                }
            }
            
            return matrixBuilder.build();
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
                for (const_iterator it = this->begin(rowToCopy), ite = this->end(rowToCopy); it != ite; ++it) {
                    if (it->first == rowGroupIndex) {
                        foundDiagonalElement = true;
                    }
                    ++subEntries;
                }
                if (insertDiagonalEntries && !foundDiagonalElement) {
                    ++subEntries;
                }
            }
            
            // Now create the matrix to be returned with the appropriate size.
            SparseMatrixBuilder<T> matrixBuilder(rowGroupIndices.size() - 1, columnCount, subEntries);
            
            // Copy over the selected lines from the source matrix.
            for (uint_fast64_t rowGroupIndex = 0, rowGroupIndexEnd = rowGroupToRowIndexMapping.size(); rowGroupIndex < rowGroupIndexEnd; ++rowGroupIndex) {
                // Determine which row we need to select from the current row group.
                uint_fast64_t rowToCopy = rowGroupIndices[rowGroupIndex] + rowGroupToRowIndexMapping[rowGroupIndex];
                
                // Iterate through that row and copy the entries. This also inserts a zero element on the diagonal if
                // there is no entry yet.
                bool insertedDiagonalElement = false;
                for (const_iterator it = this->begin(rowToCopy), ite = this->end(rowToCopy); it != ite; ++it) {
                    if (it->first == rowGroupIndex) {
                        insertedDiagonalElement = true;
                    } else if (insertDiagonalEntries && !insertedDiagonalElement && it->first > rowGroupIndex) {
                        matrixBuilder.addNextValue(rowGroupIndex, rowGroupIndex, storm::utility::constantZero<T>());
                        insertedDiagonalElement = true;
                    }
                    matrixBuilder.addNextValue(rowGroupIndex, it->first, it->second);
                }
                if (insertDiagonalEntries && !insertedDiagonalElement) {
                    matrixBuilder.addNextValue(rowGroupIndex, rowGroupIndex, storm::utility::constantZero<T>());
                }
            }
            
            // Finalize created matrix and return result.
            return matrixBuilder.build();
        }
        
        template <typename T>
        SparseMatrix<T> SparseMatrix<T>::transpose() const {
            uint_fast64_t rowCount = this->columnCount;
            uint_fast64_t columnCount = this->rowCount;
            uint_fast64_t entryCount = this->entryCount;
            
            std::vector<uint_fast64_t> rowIndications(rowCount + 1);
            std::vector<std::pair<uint_fast64_t, T>> columnsAndValues(entryCount);
            
            // First, we need to count how many entries each column has.
            for (uint_fast64_t row = 0; row < this->rowCount; ++row) {
                for (const_iterator it = this->begin(row), ite = this->end(row); it != ite; ++it) {
                    if (it->second > 0) {
                        ++rowIndications[it->first + 1];
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
            for (uint_fast64_t row = 0; row < this->rowCount; ++row) {
                for (const_iterator it = this->begin(row), ite = this->end(row); it != ite; ++it) {
                    if (it->second > 0) {
                        columnsAndValues[nextIndices[it->first]] = std::make_pair(row, it->second);
                        nextIndices[it->first]++;
                    }
                }
            }
            
            storm::storage::SparseMatrix<T> transposedMatrix(columnCount, std::move(rowIndications), std::move(columnsAndValues));
            
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
                for (iterator it = this->begin(row), ite = this->end(row); it != ite; ++it) {
                    if (it->first == row) {
                        it->second = one - it->second;
                        foundDiagonalElement = true;
                    }
                }
                
                // Throw an exception if a row did not have an element on the diagonal.
                if (!foundDiagonalElement) {
                    throw storm::exceptions::InvalidArgumentException() << "Illegal call to SparseMatrix::invertDiagonal: matrix is missing diagonal entries.";
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
                for (iterator it = this->begin(row), ite = this->end(row); it != ite; ++it) {
                    if (it->first != row) {
                        it->second = -it->second;
                    }
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
                for (iterator it = this->begin(row), ite = this->end(row); it != ite; ++it) {
                    if (it->first == row) {
                        it->second = storm::utility::constantZero<T>();
                    }
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
            
            SparseMatrixBuilder<T> dInvBuilder(rowCount, columnCount, rowCount);
            
            // Copy entries to the appropriate matrices.
            for (uint_fast64_t rowNumber = 0; rowNumber < rowCount; ++rowNumber) {
                
                // Because the matrix may have several entries on the diagonal, we need to sum them before we are able
                // to invert the entry.
                T diagonalValue = storm::utility::constantZero<T>();
                for (const_iterator it = this->begin(rowNumber), ite = this->end(rowNumber); it != ite; ++it) {
                    if (it->first == rowNumber) {
                        diagonalValue += it->second;
                    } else if (it->first > rowNumber) {
                        break;
                    }
                }
                dInvBuilder.addNextValue(rowNumber, rowNumber, storm::utility::constantOne<T>() / diagonalValue);
            }
            
            return std::make_pair(std::move(resultLU), dInvBuilder.build());
        }
        
        template<typename T>
        std::vector<T> SparseMatrix<T>::getPointwiseProductRowSumVector(storm::storage::SparseMatrix<T> const& otherMatrix) const {
            std::vector<T> result(rowCount, storm::utility::constantZero<T>());
            
            // Iterate over all elements of the current matrix and either continue with the next element in case the
            // given matrix does not have a non-zero element at this column position, or multiply the two entries and
            // add the result to the corresponding position in the vector.
            for (uint_fast64_t row = 0; row < rowCount && row < otherMatrix.rowCount; ++row) {
                for (const_iterator it1 = this->begin(row), ite1 = this->end(row), it2 = otherMatrix.begin(row), ite2 = otherMatrix.end(row); it1 != ite1 && it2 != ite2; ++it1) {
                    if (it1->first < it2->first) {
                        continue;
                    } else {
                        // If the precondition of this method (i.e. that the given matrix is a submatrix
                        // of the current one) was fulfilled, we know now that the two elements are in
                        // the same column, so we can multiply and add them to the row sum vector.
                        result[row] += it2->second * it1->second;
                        ++it2;
                    }
                }
            }
            
            return result;
        }
        
        
        template<typename T>
        void SparseMatrix<T>::multiplyWithVector(std::vector<T> const& vector, std::vector<T>& result) const {
#ifdef STORM_HAVE_INTELTBB
            tbb::parallel_for(tbb::blocked_range<uint_fast64_t>(0, result.size()), TbbMatrixRowVectorScalarProduct<T>(*this, vector, result));
#else
            const_iterator it = this->begin();
            const_iterator ite = this->begin();
            typename std::vector<uint_fast64_t>::const_iterator rowIterator = rowIndications.begin();
            typename std::vector<uint_fast64_t>::const_iterator rowIteratorEnd = rowIndications.end();
            typename std::vector<T>::iterator resultIterator = result.begin();
            typename std::vector<T>::iterator resultIteratorEnd = result.end();
            
            for (; resultIterator != resultIteratorEnd; ++rowIterator, ++resultIterator) {
                *resultIterator = storm::utility::constantZero<T>();
                
                for (ite = it + (*(rowIterator + 1) - *rowIterator); it != ite; ++it) {
                    *resultIterator += it->second * vector[it->first];
                }
            }
#endif
        }
        
        template<typename T>
        uint_fast64_t SparseMatrix<T>::getSizeInMemory() const {
            uint_fast64_t size = sizeof(*this);
            
            // Add size of columns and values.
            size += sizeof(std::pair<uint_fast64_t, T>) * columnsAndValues.capacity();
            
            // Add row_indications size.
            size += sizeof(uint_fast64_t) * rowIndications.capacity();
            
            return size;
        }
        
        template<typename T>
        typename SparseMatrix<T>::const_rows SparseMatrix<T>::getRows(uint_fast64_t startRow, uint_fast64_t endRow) const {
            return const_rows(this->columnsAndValues.begin() + this->rowIndications[startRow], this->rowIndications[endRow + 1] - this->rowIndications[startRow]);
        }
        
        template<typename T>
        typename SparseMatrix<T>::rows SparseMatrix<T>::getRows(uint_fast64_t startRow, uint_fast64_t endRow) {
            return rows(this->columnsAndValues.begin() + this->rowIndications[startRow], this->rowIndications[endRow + 1] - this->rowIndications[startRow]);
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
            return this->columnsAndValues.begin() + this->rowIndications[row];
        }
        
        template<typename T>
        typename SparseMatrix<T>::iterator SparseMatrix<T>::begin(uint_fast64_t row)  {
            return this->columnsAndValues.begin() + this->rowIndications[row];
        }
        
        template<typename T>
        typename SparseMatrix<T>::const_iterator SparseMatrix<T>::end(uint_fast64_t row) const {
            return this->columnsAndValues.begin() + this->rowIndications[row + 1];
        }
        
        template<typename T>
        typename SparseMatrix<T>::iterator SparseMatrix<T>::end(uint_fast64_t row)  {
            return this->columnsAndValues.begin() + this->rowIndications[row + 1];
        }
        
        template<typename T>
        typename SparseMatrix<T>::const_iterator SparseMatrix<T>::end() const {
            return this->columnsAndValues.begin() + this->rowIndications[rowCount];
        }
        
        template<typename T>
        typename SparseMatrix<T>::iterator SparseMatrix<T>::end()  {
            return this->columnsAndValues.begin() + this->rowIndications[rowCount];
        }
        
        template<typename T>
        T SparseMatrix<T>::getRowSum(uint_fast64_t row) const {
            T sum = storm::utility::constantZero<T>();
            for (const_iterator it = this->begin(row), ite = this->end(row); it != ite; ++it) {
                sum += it->second;
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
                for (const_iterator it1 = this->begin(row), ite1 = this->end(row), it2 = matrix.begin(row), ite2 = matrix.end(row); it1 != ite1; ++it1) {
                    // Skip over all entries of the other matrix that are before the current entry in the current matrix.
                    while (it2 != ite2 && it2->first < it1->first) {
                        ++it2;
                    }
                    if (it2 == ite2 || it1->first != it2->first) {
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
                    if (nextIndex < matrix.rowIndications[i + 1] && currentRealIndex == matrix.columnsAndValues[nextIndex].first) {
                        out << matrix.columnsAndValues[nextIndex].second << "\t";
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
            boost::hash_combine(result, boost::hash_range(columnsAndValues.begin(), columnsAndValues.end()));
            boost::hash_combine(result, boost::hash_range(rowIndications.begin(), rowIndications.end()));
            
            return result;
        }
        
        // Explicitly instantiate the builder and the matrix.
        template class SparseMatrixBuilder<double>;
        template class SparseMatrix<double>;
        template std::ostream& operator<<(std::ostream& out, SparseMatrix<double> const& matrix);
        template class SparseMatrixBuilder<int>;
        template class SparseMatrix<int>;
        template std::ostream& operator<<(std::ostream& out, SparseMatrix<int> const& matrix);
        
#ifdef STORM_HAVE_INTELTBB
        template <typename ValueType>
        TbbMatrixRowVectorScalarProduct<ValueType>::TbbMatrixRowVectorScalarProduct(SparseMatrix<ValueType> const& matrix, std::vector<ValueType> const& vector, std::vector<ValueType>& result) : result(result), vector(vector), matrix(matrix) {
            // Intentionally left empty.
        }
        
        template <typename ValueType>
        void TbbMatrixRowVectorScalarProduct<ValueType>::operator() (tbb::blocked_range<uint_fast64_t> const& range) const {
            for (uint_fast64_t row = range.begin(); row < range.end(); ++row) {
                ValueType element = storm::utility::constantZero<ValueType>();

                for (typename SparseMatrix<ValueType>::const_iterator it = matrix.begin(row), ite = matrix.end(row); it != ite; ++it) {
                    element += it->second * vector[it->first];
                }
                
                result[row] = element;
            }
        }
        
        // Explicitly instantiate the helper class.
        template class TbbMatrixRowVectorScalarProduct<double>;
        
#endif
        
    } // namespace storage
} // namespace storm



