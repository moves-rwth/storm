#include "storm/adapters/GmmxxAdapter.h"

#include <algorithm>

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/utility/constants.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace adapters {
     
        template<typename T>
        std::unique_ptr<gmm::csr_matrix<T>> GmmxxAdapter<T>::toGmmxxSparseMatrix(storm::storage::SparseMatrix<T> const& matrix) {
            uint_fast64_t realNonZeros = matrix.getEntryCount();
            STORM_LOG_DEBUG("Converting " << matrix.getRowCount() << "x" << matrix.getColumnCount() << " matrix with " << realNonZeros << " non-zeros to gmm++ format.");
            
            // Prepare the resulting matrix.
            std::unique_ptr<gmm::csr_matrix<T>> result(new gmm::csr_matrix<T>(matrix.getRowCount(), matrix.getColumnCount()));
            
            // Copy Row Indications
            std::copy(matrix.rowIndications.begin(), matrix.rowIndications.end(), result->jc.begin());
            
            // Copy columns and values.
            std::vector<T> values;
            values.reserve(matrix.getEntryCount());
            
            // To match the correct vector type for gmm, we create the vector with the exact same type.
            decltype(result->ir) columns;
            columns.reserve(matrix.getEntryCount());
            
            for (auto const& entry : matrix) {
                columns.emplace_back(entry.getColumn());
                values.emplace_back(entry.getValue());
            }
            
            std::swap(result->ir, columns);
            std::swap(result->pr, values);
            
            STORM_LOG_DEBUG("Done converting matrix to gmm++ format.");
            
            return result;
        }
        
        template<typename T>
        void GmmxxMultiplier<T>::multAdd(gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result) {
            if (b) {
                gmm::mult_add(matrix, x, *b, result);
            } else {
                gmm::mult(matrix, x, result);
            }
        }
        
        template<typename T>
        void GmmxxMultiplier<T>::multAddGaussSeidelBackward(gmm::csr_matrix<T> const& matrix, std::vector<T>& x, std::vector<T> const* b) {
            STORM_LOG_ASSERT(matrix.nr == matrix.nc, "Expecting square matrix.");
            if (b) {
                gmm::mult_add_by_row_bwd(matrix, x, *b, x, gmm::abstract_dense());
            } else {
                gmm::mult_by_row_bwd(matrix, x, x, gmm::abstract_dense());
            }
        }
        
        template<typename T>
        void GmmxxMultiplier<T>::multAddReduce(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result, std::vector<uint64_t>* choices) {
            std::vector<T>* target = &result;
            std::unique_ptr<std::vector<T>> temporary;
            if (&x == &result) {
                STORM_LOG_WARN("Using temporary in 'multAddReduce'.");
                temporary = std::make_unique<std::vector<T>>(x.size());
                target = temporary.get();
            }
            
            multAddReduceHelper(dir, rowGroupIndices, matrix, x, b, *target, choices);
        }
        
        template<typename T>
        void GmmxxMultiplier<T>::multAddReduceGaussSeidel(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<T> const& matrix, std::vector<T>& x, std::vector<T> const* b, std::vector<uint64_t>* choices) {
            multAddReduceHelper(dir, rowGroupIndices, matrix, x, b, x, choices);
        }
        
        template<typename T>
        void GmmxxMultiplier<T>::multAddReduceHelper(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result, std::vector<uint64_t>* choices) {
            typedef std::vector<T> VectorType;
            typedef gmm::csr_matrix<T> MatrixType;
            
            typename gmm::linalg_traits<VectorType>::const_iterator add_it, add_ite;
            if (b) {
                add_it = gmm::vect_end(*b) - 1;
                add_ite = gmm::vect_begin(*b) - 1;
            }
            typename gmm::linalg_traits<VectorType>::iterator target_it = gmm::vect_end(result) - 1;
            typename gmm::linalg_traits<MatrixType>::const_row_iterator itr = mat_row_const_end(matrix) - 1;
            typename std::vector<uint64_t>::iterator choice_it;
            if (choices) {
                choice_it = choices->end() - 1;
            }
            
            uint64_t choice;
            for (auto row_group_it = rowGroupIndices.end() - 2, row_group_ite = rowGroupIndices.begin() - 1; row_group_it != row_group_ite; --row_group_it, --choice_it, --target_it) {
                T currentValue = b ? *add_it : storm::utility::zero<T>();
                currentValue += vect_sp(gmm::linalg_traits<MatrixType>::row(itr), x);
                
                if (choices) {
                    choice = *(row_group_it + 1) - 1 - *row_group_it;
                    *choice_it = choice;
                }
                
                --itr;
                if (b) {
                    --add_it;
                }
                
                for (uint64_t row = *row_group_it + 1, rowEnd = *(row_group_it + 1); row < rowEnd; ++row, --itr) {
                    T newValue = b ? *add_it : storm::utility::zero<T>();
                    newValue += vect_sp(gmm::linalg_traits<MatrixType>::row(itr), x);
                    
                    if (choices) {
                        --choice;
                    }
                    
                    if ((dir == OptimizationDirection::Minimize && newValue < currentValue) || (dir == OptimizationDirection::Maximize && newValue > currentValue)) {
                        currentValue = newValue;
                        if (choices) {
                            *choice_it = choice;
                        }
                    }
                    if (b) {
                        --add_it;
                    }
                }
                
                // Write back final value.
                *target_it = currentValue;
            }
        }
        
        template<>
        void GmmxxMultiplier<storm::RationalFunction>::multAddReduceHelper(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<storm::RationalFunction> const& matrix, std::vector<storm::RationalFunction> const& x, std::vector<storm::RationalFunction> const* b, std::vector<storm::RationalFunction>& result, std::vector<uint64_t>* choices) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Operation not supported for this data type.");
        }
        
#ifdef STORM_HAVE_INTELTBB
        template<typename T>
        void GmmxxMultiplier<T>::multAddParallel(gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result) {
            if (b) {
                gmm::mult_add_parallel(matrix, x, *b, result);
            } else {
                gmm::mult_parallel(matrix, x, result);
            }
        }
#endif

        template class GmmxxAdapter<double>;
        template class GmmxxMultiplier<double>;
        
#ifdef STORM_HAVE_CARL
        template class GmmxxAdapter<storm::RationalNumber>;
        template class GmmxxAdapter<storm::RationalFunction>;

        template class GmmxxMultiplier<storm::RationalNumber>;
        template class GmmxxMultiplier<storm::RationalFunction>;
#endif
        
    }
}
