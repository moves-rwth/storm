#include "src/storage/FlexibleSparseMatrix.h"

#include "src/storage/SparseMatrix.h"
#include "src/storage/BitVector.h"
#include "src/adapters/CarlAdapter.h"
#include "src/utility/constants.h"

namespace storm {
    namespace storage {
        template<typename ValueType>
        FlexibleSparseMatrix<ValueType>::FlexibleSparseMatrix(index_type rows) : data(rows), columnCount(0), nonzeroEntryCount(0) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        FlexibleSparseMatrix<ValueType>::FlexibleSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix, bool setAllValuesToOne) : data(matrix.getRowCount()), columnCount(matrix.getColumnCount()), nonzeroEntryCount(matrix.getNonzeroEntryCount()), nontrivialRowGrouping(matrix.hasNontrivialRowGrouping()) {
            if (nontrivialRowGrouping) {
                rowGroupIndices = matrix.getRowGroupIndices();
            }
            for (index_type rowIndex = 0; rowIndex < matrix.getRowCount(); ++rowIndex) {
                typename storm::storage::SparseMatrix<ValueType>::const_rows row = matrix.getRow(rowIndex);
                reserveInRow(rowIndex, row.getNumberOfEntries());
                for (auto const& element : row) {
                    // If the probability is zero, we skip this entry.
                    if (storm::utility::isZero(element.getValue())) {
                        continue;
                    }
                    if (setAllValuesToOne) {
                        getRow(rowIndex).emplace_back(element.getColumn(), storm::utility::one<ValueType>());
                    } else {
                        getRow(rowIndex).emplace_back(element);
                    }
                }
            }
        }

        template<typename ValueType>
        void FlexibleSparseMatrix<ValueType>::reserveInRow(index_type row, index_type numberOfElements) {
            this->data[row].reserve(numberOfElements);
        }
        
        template<typename ValueType>
        typename FlexibleSparseMatrix<ValueType>::row_type& FlexibleSparseMatrix<ValueType>::getRow(index_type index) {
            return this->data[index];
        }
        
        template<typename ValueType>
        typename FlexibleSparseMatrix<ValueType>::row_type const& FlexibleSparseMatrix<ValueType>::getRow(index_type index) const {
            return this->data[index];
        }
        
        template<typename ValueType>
        typename FlexibleSparseMatrix<ValueType>::row_type& FlexibleSparseMatrix<ValueType>::getRow(index_type rowGroup, index_type offset) {
            assert(rowGroup < this->getRowGroupCount());
            assert(offset < this->getRowGroupSize(rowGroup));
            return getRow(rowGroupIndices[rowGroup] + offset);
        }
        
        template<typename ValueType>
        typename FlexibleSparseMatrix<ValueType>::row_type const& FlexibleSparseMatrix<ValueType>::getRow(index_type rowGroup, index_type offset) const {
            assert(rowGroup < this->getRowGroupCount());
            assert(offset < this->getRowGroupSize(rowGroup));
            return getRow(rowGroupIndices[rowGroup] + offset);
        }
        
        template<typename ValueType>
        std::vector<typename FlexibleSparseMatrix<ValueType>::index_type> const& FlexibleSparseMatrix<ValueType>::getRowGroupIndices() const {
            return rowGroupIndices;
        }
        
        template<typename ValueType>
        typename FlexibleSparseMatrix<ValueType>::index_type FlexibleSparseMatrix<ValueType>::getRowCount() const {
            return this->data.size();
        }
        
        template<typename ValueType>
        typename FlexibleSparseMatrix<ValueType>::index_type FlexibleSparseMatrix<ValueType>::getColumnCount() const {
            return columnCount;
        }
        
        template<typename ValueType>
        typename FlexibleSparseMatrix<ValueType>::index_type FlexibleSparseMatrix<ValueType>::getNonzeroEntryCount() const {
            return nonzeroEntryCount;
        }
        
        template<typename ValueType>
        typename FlexibleSparseMatrix<ValueType>::index_type FlexibleSparseMatrix<ValueType>::getRowGroupCount() const {
            return rowGroupIndices.size();
        }
        
        template<typename ValueType>
        typename FlexibleSparseMatrix<ValueType>::index_type FlexibleSparseMatrix<ValueType>::getRowGroupSize(index_type group) const {
            if (group == getRowGroupCount() - 1) {
                return getRowCount() - rowGroupIndices[group];
            } else {
                return rowGroupIndices[group + 1] - rowGroupIndices[group];
            }
        }
        
        template<typename ValueType>
        ValueType FlexibleSparseMatrix<ValueType>::getRowSum(index_type row) const {
            ValueType sum = storm::utility::zero<ValueType>();
            for (auto const& element : getRow(row)) {
                sum += element.getValue();
            }
            return sum;
        }

        template<typename ValueType>
        void FlexibleSparseMatrix<ValueType>::updateDimensions() {
            this->nonzeroEntryCount = 0;
            this->columnCount = 0;
            for (auto const& row : this->data) {
                for (auto const& element : row) {
                    assert(!storm::utility::isZero(element.getValue()));
                    ++this->nonzeroEntryCount;
                    this->columnCount = std::max(element.getColumn() + 1, this->columnCount);
                }
            }
        }

        template<typename ValueType>
        bool FlexibleSparseMatrix<ValueType>::empty() const {
            for (auto const& row : this->data) {
                if (!row.empty()) {
                    return false;
                }
            }
            return true;
        }
        
        template<typename ValueType>
        bool FlexibleSparseMatrix<ValueType>::hasNontrivialRowGrouping() const {
            return nontrivialRowGrouping;
        }

        template<typename ValueType>
        void FlexibleSparseMatrix<ValueType>::createSubmatrix(storm::storage::BitVector const& rowConstraint, storm::storage::BitVector const& columnConstraint) {
            for (uint_fast64_t rowIndex = 0; rowIndex < this->data.size(); ++rowIndex) {
                auto& row = this->data[rowIndex];
                if (!rowConstraint.get(rowIndex)) {
                    row.clear();
                    row.shrink_to_fit();
                    continue;
                }
                row_type newRow;
                for (auto const& element : row) {
                    if (columnConstraint.get(element.getColumn())) {
                        newRow.push_back(element);
                    }
                }
                row = std::move(newRow);
            }
        }
        
        template<typename ValueType>
        storm::storage::SparseMatrix<ValueType> FlexibleSparseMatrix<ValueType>::createSparseMatrix() {
            storm::storage::SparseMatrixBuilder<ValueType> matrixBuilder(getRowCount(), getColumnCount());
            for (uint_fast64_t rowIndex = 0; rowIndex < getRowCount(); ++rowIndex) {
                auto& row = this->data[rowIndex];
                for (auto const& entry : row) {
                    matrixBuilder.addNextValue(rowIndex, entry.getColumn(), entry.getValue());
                }
            }
            return matrixBuilder.build();
        }

        template<typename ValueType>
        bool FlexibleSparseMatrix<ValueType>::rowHasDiagonalElement(storm::storage::sparse::state_type state) {
            for (auto const& entry : this->getRow(state)) {
                if (entry.getColumn() < state) {
                    continue;
                } else if (entry.getColumn() > state) {
                    return false;
                } else if (entry.getColumn() == state) {
                    return true;
                }
            }
            return false;
        }
        
        template<typename ValueType>
        std::ostream& FlexibleSparseMatrix<ValueType>::printRow(std::ostream& out, index_type const& rowIndex) const {
            index_type columnIndex = 0;
            row_type row = this->getRow(rowIndex);
            for (index_type column = 0; column < this->getColumnCount(); ++column) {
                if (columnIndex < row.size() && row[columnIndex].getColumn() == column) {
                    // Insert entry
                    out << row[columnIndex].getValue() << "\t";
                    ++columnIndex;
                } else {
                    // Insert zero
                    out << "0\t";
                }
            }
            return out;
        }
        

        template<typename ValueType>
        std::ostream& operator<<(std::ostream& out, FlexibleSparseMatrix<ValueType> const& matrix) {
            typedef typename FlexibleSparseMatrix<ValueType>::index_type FlexibleIndex;
            
            // Print column numbers in header.
            out << "\t\t";
            for (FlexibleIndex i = 0; i < matrix.getColumnCount(); ++i) {
                out << i << "\t";
            }
            out << std::endl;
            
            if (matrix.hasNontrivialRowGrouping()) {
                // Iterate over all row groups
                FlexibleIndex rowGroupCount = matrix.getRowGroupCount();
                for (FlexibleIndex rowGroup = 0; rowGroup < rowGroupCount; ++rowGroup) {
                    out << "\t---- group " << rowGroup << "/" << (rowGroupCount - 1) << " ---- " << std::endl;
                    FlexibleIndex endRow = rowGroup+1 < rowGroupCount ? matrix.rowGroupIndices[rowGroup + 1] : matrix.getRowCount();
                    // Iterate over all rows.
                    for (FlexibleIndex row = matrix.rowGroupIndices[rowGroup]; row < endRow; ++row) {
                        // Print the actual row.
                        out << rowGroup << "\t(\t";
                        matrix.printRow(out, row);
                        out << "\t)\t" << rowGroup << std::endl;
                    }
                }

            } else {
                // Iterate over all rows
                for (FlexibleIndex row = 0; row < matrix.getRowCount(); ++row) {
                    // Print the actual row.
                    out << row << "\t(\t";
                    matrix.printRow(out, row);
                    out << "\t)\t" << row << std::endl;
                }
            }
            
            // Print column numbers in footer.
            out << "\t\t";
            for (FlexibleIndex i = 0; i < matrix.getColumnCount(); ++i) {
                out << i << "\t";
            }
            out << std::endl;
            return out;
        }

        // Explicitly instantiate the matrix.
        template class FlexibleSparseMatrix<double>;
        template std::ostream& operator<<(std::ostream& out, FlexibleSparseMatrix<double> const& matrix);
        
#ifdef STORM_HAVE_CARL
        template class FlexibleSparseMatrix<storm::RationalFunction>;
        template std::ostream& operator<<(std::ostream& out, FlexibleSparseMatrix<storm::RationalFunction> const& matrix);
#endif

    } // namespace storage
} // namespace storm