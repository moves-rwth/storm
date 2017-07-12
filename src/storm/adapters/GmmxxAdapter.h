#ifndef STORM_ADAPTERS_GMMXXADAPTER_H_
#define STORM_ADAPTERS_GMMXXADAPTER_H_

#include <algorithm>
#include <memory>

#include "storm/utility/gmm.h"

#include "storm/storage/SparseMatrix.h"

#include "storm/utility/macros.h"

namespace storm {
    
    namespace adapters {
        
        class GmmxxAdapter {
        public:
            /*!
             * Converts a sparse matrix into a sparse matrix in the gmm++ format.
             * @return A pointer to a row-major sparse matrix in gmm++ format.
             */
            template<class T>
            static std::unique_ptr<gmm::csr_matrix<T>> toGmmxxSparseMatrix(storm::storage::SparseMatrix<T> const& matrix) {
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
        };
        
    } // namespace adapters
} // namespace storm

#endif /* STORM_ADAPTERS_GMMXXADAPTER_H_ */
