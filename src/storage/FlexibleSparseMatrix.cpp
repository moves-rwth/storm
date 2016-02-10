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
        FlexibleSparseMatrix<ValueType>::FlexibleSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix, bool setAllValuesToOne) : data(matrix.getRowCount()), columnCount(matrix.getColumnCount()), nonzeroEntryCount(matrix.getNonzeroEntryCount()) {
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
            for (uint_fast64_t index = 0; index < matrix->data.size(); ++index) {
                out << index << " - ";
                for (auto const& element : matrix->getRow(index)) {
                    out << "(" << element.getColumn() << ", " << element.getValue() << ") ";
                }
                return out;
            }
        }

        // Explicitly instantiate the matrix.
        template class FlexibleSparseMatrix<double>;
#ifdef STORM_HAVE_CARL
        template class FlexibleSparseMatrix<storm::RationalFunction>;
#endif

    } // namespace storage
} // namespace storm