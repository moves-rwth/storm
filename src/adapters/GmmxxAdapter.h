/*
 * GmmxxAdapter.h
 *
 *  Created on: 24.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_ADAPTERS_GMMXXADAPTER_H_
#define STORM_ADAPTERS_GMMXXADAPTER_H_

#include "src/storage/SparseMatrix.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {

namespace adapters {

class GmmxxAdapter {
public:
	/*!
	 * Converts a sparse matrix into the sparse matrix in the gmm++ format.
	 * @return A pointer to a row-major sparse matrix in gmm++ format.
	 */
	template<class T>
	static gmm::csr_matrix<T>* toGmmxxSparseMatrix(storm::storage::SparseMatrix<T> const& matrix) {
		uint_fast64_t realNonZeros = matrix.getNonZeroEntryCount();
		LOG4CPLUS_DEBUG(logger, "Converting matrix with " << realNonZeros << " non-zeros to gmm++ format.");

		// Prepare the resulting matrix.
		gmm::csr_matrix<T>* result = new gmm::csr_matrix<T>(matrix.rowCount, matrix.colCount);

		// Copy Row Indications
		std::copy(matrix.rowIndications.begin(), matrix.rowIndications.end(), result->jc.begin());
		// Copy Columns Indications
		result->ir.resize(realNonZeros);
		std::copy(matrix.columnIndications.begin(), matrix.columnIndications.end(), result->ir.begin());
		// And do the same thing with the actual values.
		result->pr.resize(realNonZeros);
		std::copy(matrix.valueStorage.begin(), matrix.valueStorage.end(), result->pr.begin());

		LOG4CPLUS_DEBUG(logger, "Done converting matrix to gmm++ format.");

		return result;
	}
};

} //namespace adapters

} //namespace storm

#endif /* STORM_ADAPTERS_GMMXXADAPTER_H_ */
