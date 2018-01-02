#pragma once

#include "storm/utility/VectorHelper.h"

#include "storm/adapters/GmmxxAdapter.h"

#include "storm-config.h"

namespace storm {
    namespace solver {
        
        template<class T>
        class GmmxxMultiplier : public storm::utility::VectorHelper<T> {
        public:
            GmmxxMultiplier();
            
            void multAdd(gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result) const;
            void multAddGaussSeidelBackward(gmm::csr_matrix<T> const& matrix, std::vector<T>& x, std::vector<T> const* b) const;
            
            void multAddReduce(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result, std::vector<uint64_t>* choices = nullptr) const;
            void multAddReduceGaussSeidel(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<T> const& matrix, std::vector<T>& x, std::vector<T> const* b, std::vector<uint64_t>* choices = nullptr) const;
            
            void multAddParallel(gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result) const;
            void multAddReduceParallel(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result, std::vector<uint64_t>* choices = nullptr) const;

            T multiplyRow(gmm::csr_matrix<T> const& matrix, uint64_t const& rowIndex, std::vector<T> const& x) const;

        private:
            void multAddReduceHelper(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result, std::vector<uint64_t>* choices = nullptr) const;
        };
        
    }
}
