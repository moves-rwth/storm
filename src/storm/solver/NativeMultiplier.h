#pragma once

#include "storm/utility/VectorHelper.h"

#include "storm/solver/OptimizationDirection.h"

namespace storm {
    namespace storage {
        template<typename ValueType>
        class SparseMatrix;
    }
    
    namespace solver {
        
        template<typename ValueType>
        class NativeMultiplier : public storm::utility::VectorHelper<ValueType> {
        public:
            NativeMultiplier();
            
            void multAdd(storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const;
            void multAddGaussSeidelBackward(storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType>& x, std::vector<ValueType> const* b) const;
            
            void multAddReduce(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint64_t>* choices = nullptr) const;
            void multAddReduceGaussSeidelBackward(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<uint64_t>* choices = nullptr) const;
            
            void multAddParallel(storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const;
            void multAddReduceParallel(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint64_t>* choices = nullptr) const;
            
            ValueType multiplyRow(storm::storage::SparseMatrix<ValueType> const& matrix, uint64_t const& rowIndex, std::vector<ValueType> const& x) const;
        };
        
    }
}
