#include "storm/solver/NativeMultiplier.h"

#include "storm-config.h"

#include "storm/storage/SparseMatrix.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/utility/macros.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        NativeMultiplier<ValueType>::NativeMultiplier() : storm::utility::VectorHelper<ValueType>() {
            // Intentionally left empty.
        }

        template<typename ValueType>
        void NativeMultiplier<ValueType>::multAdd(storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
            std::vector<ValueType>* target = &result;
            std::unique_ptr<std::vector<ValueType>> temporary;
            if (&x == &result) {
                STORM_LOG_WARN("Using temporary in 'multAdd'.");
                temporary = std::make_unique<std::vector<ValueType>>(x.size());
                target = temporary.get();
            }
            
            if (this->parallelize()) {
                multAddParallel(matrix, x, b, result);
            } else {
                matrix.multiplyWithVector(x, result, b);
            }
            
            if (target == temporary.get()) {
                std::swap(result, *temporary);
            }
        }
        
        template<typename ValueType>
        void NativeMultiplier<ValueType>::multAddGaussSeidelBackward(storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType>& x, std::vector<ValueType> const* b) const {
            matrix.multiplyWithVectorBackward(x, x, b);
        }
        
        template<typename ValueType>
        void NativeMultiplier<ValueType>::multAddReduce(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint64_t>* choices) const {
            std::vector<ValueType>* target = &result;
            std::unique_ptr<std::vector<ValueType>> temporary;
            if (&x == &result) {
                STORM_LOG_WARN("Using temporary in 'multAddReduce'.");
                temporary = std::make_unique<std::vector<ValueType>>(x.size());
                target = temporary.get();
            }
            
            if (this->parallelize()) {
                multAddReduceParallel(dir, rowGroupIndices, matrix, x, b, *target, choices);
            } else {
                matrix.multiplyAndReduce(dir, rowGroupIndices, x, b, *target, choices);
            }
            
            if (target == temporary.get()) {
                std::swap(result, *temporary);
            }
        }
        
        template<typename ValueType>
        void NativeMultiplier<ValueType>::multAddReduceGaussSeidelBackward(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<uint64_t>* choices) const {
            matrix.multiplyAndReduceBackward(dir, rowGroupIndices, x, b, x, choices);
        }
                
        template<typename ValueType>
        void NativeMultiplier<ValueType>::multAddParallel(storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
#ifdef STORM_HAVE_INTELTBB
            matrix.multiplyWithVectorParallel(x, result, b);
#else
            STORM_LOG_WARN("Storm was built without support for Intel TBB, defaulting to sequential version.");
            multAdd(matrix, x, b, result);
#endif
        }
                
        template<typename ValueType>
        void NativeMultiplier<ValueType>::multAddReduceParallel(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint64_t>* choices) const {
#ifdef STORM_HAVE_INTELTBB
            matrix.multiplyAndReduceParallel(dir, rowGroupIndices, x, b, result, choices);
#else
            STORM_LOG_WARN("Storm was built without support for Intel TBB, defaulting to sequential version.");
            multAddReduce(dir, rowGroupIndices, x, b, result, choices);
#endif
        }


        template class NativeMultiplier<double>;
        
#ifdef STORM_HAVE_CARL
        template class NativeMultiplier<storm::RationalNumber>;
        template class NativeMultiplier<storm::RationalFunction>;
#endif
        
    }
}
