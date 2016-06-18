#include "src/adapters/EigenAdapter.h"

namespace storm {
    namespace adapters {
     
        template<typename ValueType>
        std::unique_ptr<Eigen::SparseMatrix<ValueType>> EigenAdapter::toEigenSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix) {
            // Build a list of triplets and let Eigen care about the insertion.
            std::vector<Eigen::Triplet<ValueType>> triplets;
            triplets.reserve(matrix.getNonzeroEntryCount());
            
            for (uint64_t row = 0; row < matrix.getRowCount(); ++row) {
                for (auto const& element : matrix.getRow(row)) {
                    triplets.emplace_back(row, element.getColumn(), element.getValue());
                }
            }
            
            std::unique_ptr<Eigen::SparseMatrix<ValueType>> result = std::make_unique<Eigen::SparseMatrix<ValueType>>(matrix.getRowCount(), matrix.getColumnCount());
            result->setFromTriplets(triplets.begin(), triplets.end());
            return result;
        }
        
        template std::unique_ptr<Eigen::SparseMatrix<double>> EigenAdapter::toEigenSparseMatrix(storm::storage::SparseMatrix<double> const& matrix);
    }
}