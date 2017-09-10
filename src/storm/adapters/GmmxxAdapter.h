#pragma once

#include <memory>

#include "storm/utility/gmm.h"

#include "storm/storage/SparseMatrix.h"

#include "storm/utility/macros.h"

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
        
        template<class T>
        class GmmxxMultiplier {
        public:
            static void multAdd(gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result);
            static void multAddGaussSeidelBackward(gmm::csr_matrix<T> const& matrix, std::vector<T>& x, std::vector<T> const* b);

            static void multAddReduce(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result, std::vector<uint64_t>* choices = nullptr);
            static void multAddReduceGaussSeidel(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<T> const& matrix, std::vector<T>& x, std::vector<T> const* b, std::vector<uint64_t>* choices = nullptr);
            
#ifdef STORM_HAVE_INTELTBB
            static void multAddParallel(gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const& b, std::vector<T>& result);
#endif
            
        private:
            static void multAddReduceHelper(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result, std::vector<uint64_t>* choices = nullptr);
        };
        
    }
}
