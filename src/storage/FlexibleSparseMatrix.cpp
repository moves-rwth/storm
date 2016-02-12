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
        std::ostream& operator<<(std::ostream& out, FlexibleSparseMatrix<ValueType> const& matrix) {
            // Print column numbers in header.
            out << "\t\t";
            for (typename FlexibleSparseMatrix<ValueType>::index_type i = 0; i < matrix.getColumnCount(); ++i) {
                out << i << "\t";
            }
            out << std::endl;
            
            typename FlexibleSparseMatrix<ValueType>::index_type endIndex = matrix.hasNontrivialRowGrouping() ? matrix.getRowGroupCount() : matrix.getRowCount();
            // Iterate over all rows.
            for (typename SparseMatrix<ValueType>::index_type i = 0; i < endIndex; ++i) {
                if (matrix.hasNontrivialRowGrouping()) {
                    out << "\t---- group " << i << "/" << (matrix.getRowGroupCount() - 1) << " ---- " << std::endl;
                }
                typename FlexibleSparseMatrix<ValueType>::index_type startRow = matrix.hasNontrivialRowGrouping() ? matrix.rowGroupIndices[i] : i;
                typename FlexibleSparseMatrix<ValueType>::index_type endRow = matrix.hasNontrivialRowGrouping() ? matrix.rowGroupIndices[i + 1] : i+1;
                for (typename FlexibleSparseMatrix<ValueType>::index_type rowIndex = startRow; rowIndex < endRow; ++rowIndex) {
                    // Print the actual row.
                    out << i << "\t(\t";
                    typename FlexibleSparseMatrix<ValueType>::row_type row = matrix.getRow(rowIndex);
                    typename FlexibleSparseMatrix<ValueType>::index_type columnIndex = 0;
                    for (auto const& entry : row) {
                        //Insert zero between entries.
                        while (columnIndex < entry.getColumn()) {
                            out << "0\t";
                            ++columnIndex;
                        }
                        ++columnIndex;
                        out << entry.getValue() << "\t";
                    }
                    //Insert remaining zeros.
                    while (columnIndex++ <= matrix.getColumnCount()) {
                        out << "0\t";
                    }
                    out << "\t)\t" << i << std::endl;
                }
            }
            
            // Print column numbers in footer.
            out << "\t\t";
            for (typename FlexibleSparseMatrix<ValueType>::index_type i = 0; i < matrix.getColumnCount(); ++i) {
                out << i << "\t";
            }
            out << std::endl;
            return out;
        }

        // Explicitly instantiate the matrix.
        template class FlexibleSparseMatrix<double>;
#ifdef STORM_HAVE_CARL
        template class FlexibleSparseMatrix<storm::RationalFunction>;
#endif

    } // namespace storage
} // namespace storm