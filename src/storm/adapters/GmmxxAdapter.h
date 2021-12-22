#pragma once

#include <memory>

#include "storm/adapters/gmm.h"

#include "storm/storage/SparseMatrix.h"

namespace storm {
namespace adapters {

template<typename T>
class GmmxxAdapter {
   public:
    /*!
     * Converts a sparse matrix into a sparse matrix in the gmm++ format.
     * @return A pointer to a row-major sparse matrix in gmm++ format.
     */
    static std::unique_ptr<gmm::csr_matrix<T>> toGmmxxSparseMatrix(storm::storage::SparseMatrix<T> const& matrix);
};

}  // namespace adapters
}  // namespace storm
