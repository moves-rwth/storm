#pragma once

#include <memory>
#include <iostream>
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
            static std::unique_ptr<Eigen::SparseMatrix<ValueType>> toEigenSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix);
        };
        
    }
}
