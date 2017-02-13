#pragma once

#include <memory>

#include "storm/utility/eigen.h"

#include "storm/storage/SparseMatrix.h"

namespace storm {
    namespace adapters {
        
        class EigenAdapter {
        public:
            /*!
             * Converts a sparse matrix into a sparse matrix in the gmm++ format.
             * @return A pointer to a row-major sparse matrix in gmm++ format.
             */
            template<class ValueType>
            static std::unique_ptr<StormEigen::SparseMatrix<ValueType>> toEigenSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix);

            template <typename ValueType>
            static std::vector<ValueType> toStdVector(StormEigen::Matrix<ValueType, StormEigen::Dynamic, 1> const& v);

            template <typename ValueType>
            static StormEigen::Matrix<ValueType, StormEigen::Dynamic, 1> toEigenVector(std::vector<ValueType> const& v);
        };
        
    }
}
