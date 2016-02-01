#include "src/storage/FlexibleSparseMatrix.h"

#include "src/storage/SparseMatrix.h"
#include "src/storage/BitVector.h"

namespace storm {
    namespace storage {
        template<typename ValueType>
        FlexibleSparseMatrix<ValueType>::FlexibleSparseMatrix(index_type rows) : data(rows) {
            // Intentionally left empty.
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
        typename FlexibleSparseMatrix<ValueType>::index_type FlexibleSparseMatrix<ValueType>::getNumberOfRows() const {
            return this->data.size();
        }
        
        template<typename ValueType>
        bool FlexibleSparseMatrix<ValueType>::hasSelfLoop(storm::storage::sparse::state_type state) {
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
        void FlexibleSparseMatrix<ValueType>::print() const {
            for (uint_fast64_t index = 0; index < this->data.size(); ++index) {
                std::cout << index << " - ";
                for (auto const& element : this->getRow(index)) {
                    std::cout << "(" << element.getColumn() << ", " << element.getValue() << ") ";
                }
                std::cout << std::endl;
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
        void FlexibleSparseMatrix<ValueType>::filter(storm::storage::BitVector const& rowFilter, storm::storage::BitVector const& columnFilter) {
            for (uint_fast64_t rowIndex = 0; rowIndex < this->data.size(); ++rowIndex) {
                auto& row = this->data[rowIndex];
                if (!rowFilter.get(rowIndex)) {
                    row.clear();
                    row.shrink_to_fit();
                    continue;
                }
                row_type newRow;
                for (auto const& element : row) {
                    if (columnFilter.get(element.getColumn())) {
                        newRow.push_back(element);
                    }
                }
                row = std::move(newRow);
            }
        }

    }
}