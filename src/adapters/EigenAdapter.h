#pragma once

#include <memory>

#include "src/utility/eigen.h"

#include "src/storage/SparseMatrix.h"

namespace storm {
    namespace adapters {
        
        class EigenAdapter {
        public:
            /*!
             * Converts a sparse matrix into a sparse matrix in the gmm++ format.
             * @return A pointer to a row-major sparse matrix in gmm++ format.
             */
            template<class ValueType>
            static std::unique_ptr<Eigen::SparseMatrix<ValueType>> toEigenSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix) {
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
        };
        
    }
}