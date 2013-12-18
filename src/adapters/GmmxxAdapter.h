/*
 * GmmxxAdapter.h
 *
 *  Created on: 24.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_ADAPTERS_GMMXXADAPTER_H_
#define STORM_ADAPTERS_GMMXXADAPTER_H_

#include <new>
#include <utility>

#include "gmm/gmm_matrix.h"

#include "src/storage/SparseMatrix.h"
#include "src/utility/ConversionHelper.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

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
		LOG4CPLUS_DEBUG(logger, "Converting matrix with " << realNonZeros << " non-zeros to gmm++ format.");

		// Prepare the resulting matrix.
        std::unique_ptr<gmm::csr_matrix<T>> result(new gmm::csr_matrix<T>(matrix.getRowCount(), matrix.getColumnCount()));

		// Copy Row Indications
		std::copy(matrix.rowIndications.begin(), matrix.rowIndications.end(), result->jc.begin());
        
        // Copy columns and values.
        std::vector<T> values;
        values.reserve(matrix.getEntryCount());
        std::vector<uint_fast64_t> columns;
        columns.reserve(matrix.getEntryCount());
        
        for (auto const& entry : matrix) {
            columns.emplace_back(entry.first);
            values.emplace_back(entry.second);
        }
        
        std::swap(result->ir, columns);
        std::swap(result->pr, values);
        
		LOG4CPLUS_DEBUG(logger, "Done converting matrix to gmm++ format.");

		return result;
	}

	/*!
	 * Converts a sparse matrix into a sparse matrix in the gmm++ format.
	 * @return A pointer to a row-major sparse matrix in gmm++ format.
	 */
	template<class T>
	static std::unique_ptr<gmm::csr_matrix<T>> toGmmxxSparseMatrix(storm::storage::SparseMatrix<T>&& matrix) {
		uint_fast64_t realNonZeros = matrix.getEntryCount();
		LOG4CPLUS_DEBUG(logger, "Converting matrix with " << realNonZeros << " non-zeros to gmm++ format.");

		// Prepare the resulting matrix.
        std::unique_ptr<gmm::csr_matrix<T>> result(new gmm::csr_matrix<T>(matrix.getRowCount(), matrix.getColumnCount()));

		typedef unsigned long long IND_TYPE;
        typedef std::vector<IND_TYPE> vectorType_ull_t;
        typedef std::vector<T> vectorType_T_t; 

        // Copy columns and values. It is absolutely necessary to do so before moving the row indications vector.
        std::vector<T> values;
        values.reserve(matrix.getEntryCount());
        std::vector<uint_fast64_t> columns;
        columns.reserve(matrix.getEntryCount());
        
        for (auto const& entry : matrix) {
            columns.emplace_back(entry.first);
            values.emplace_back(entry.second);
        }

        // Move Row Indications
        result->jc = std::move(matrix.rowIndications);
        
        std::swap(result->ir, columns);
        std::swap(result->pr, values);

		LOG4CPLUS_DEBUG(logger, "Done converting matrix to gmm++ format.");

		return result;
	}
};

} //namespace adapters

} //namespace storm

#endif /* STORM_ADAPTERS_GMMXXADAPTER_H_ */
