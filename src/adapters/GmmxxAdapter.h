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
	 * Converts a sparse matrix into a sparse matrix in the gmm++ format.
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

	/*!
	 * Converts a sparse matrix into a sparse matrix in the gmm++ format.
	 * @return A pointer to a row-major sparse matrix in gmm++ format.
	 */
	template<class T>
	static gmm::csr_matrix<T>* toGmmxxSparseMatrix(storm::storage::SparseMatrix<T> && matrix) {
		uint_fast64_t realNonZeros = matrix.getNonZeroEntryCount();
		LOG4CPLUS_DEBUG(logger, "Converting matrix with " << realNonZeros << " non-zeros to gmm++ format.");

		// Prepare the resulting matrix.
		gmm::csr_matrix<T>* result = new gmm::csr_matrix<T>(matrix.rowCount, matrix.colCount);

		// Move Row Indications
		result->jc = std::vector<uint_fast64_t>(std::move(matrix.rowIndications));
		// Move Columns Indications
		result->ir = std::vector<uint_fast64_t>(std::move(matrix.columnIndications));
		// And do the same thing with the actual values.
		result->pr = std::vector<T>(std::move(matrix.valueStorage));

		LOG4CPLUS_DEBUG(logger, "Done converting matrix to gmm++ format.");

		return result;
	}

	/*!
	 * Converts a sparse matrix in the gmm++ format to Storm Sparse Matrix format.
	 * @return A pointer to a row-major sparse matrix in our format.
	 */
	template<class T>
	static storm::storage::SparseMatrix<T>* fromGmmxxSparseMatrix(gmm::csr_matrix<T> const& matrix) {
		uint_fast64_t realNonZeros = gmm::nnz(matrix);
		LOG4CPLUS_DEBUG(logger, "Converting matrix with " << realNonZeros << " non-zeros from gmm++ format into Storm.");

		// Prepare the resulting matrix.
		storm::storage::SparseMatrix<T>* result = new storm::storage::SparseMatrix<T>(matrix.nrows(), matrix.ncols());
		
		// Set internal NonZero Counter
		result->nonZeroEntryCount = realNonZeros;
		result->setState(result->Initialized);

		if (!result->prepareInternalStorage(false)) {
			LOG4CPLUS_ERROR(logger, "Unable to allocate internal storage while converting GMM++ Matrix to Storm.");
			delete result;
			return nullptr;
		} else {
			
			// Copy Row Indications
			std::copy(matrix.jc.begin(), matrix.jc.end(), std::back_inserter(result->rowIndications));
			// Copy Columns Indications
			std::copy(matrix.ir.begin(), matrix.ir.end(), std::back_inserter(result->columnIndications));
			// And do the same thing with the actual values.
			std::copy(matrix.pr.begin(), matrix.pr.end(), std::back_inserter(result->valueStorage));

			result->currentSize = realNonZeros;
			result->lastRow = matrix.nrows() - 1;
		}

		result->finalize();

		LOG4CPLUS_DEBUG(logger, "Done converting matrix to storm format.");

		return result;
	}

	/*!
	 * Converts a sparse matrix in the gmm++ format to Storm Sparse Matrix format.
	 * @return A pointer to a row-major sparse matrix in our format.
	 */
	template<class T>
	static storm::storage::SparseMatrix<T>* fromGmmxxSparseMatrix(gmm::csr_matrix<T> && matrix) {
		uint_fast64_t realNonZeros = gmm::nnz(matrix);
		LOG4CPLUS_DEBUG(logger, "Converting matrix with " << realNonZeros << " non-zeros from gmm++ format into Storm.");

		// Prepare the resulting matrix.
		storm::storage::SparseMatrix<T>* result = new storm::storage::SparseMatrix<T>(matrix.nrows(), matrix.ncols());
		
		// Set internal NonZero Counter
		result->nonZeroEntryCount = realNonZeros;
		result->setState(result->Initialized);

		// Move Row Indications
		result->rowIndications = std::vector<uint_fast64_t>(std::move(matrix.jc));
		// Move Columns Indications
		result->columnIndications = std::vector<uint_fast64_t>(std::move(matrix.ir));
		// And do the same thing with the actual values.
		result->valueStorage = std::vector<T>(std::move(matrix.pr));

		result->currentSize = realNonZeros;
		result->lastRow = matrix.nrows() - 1;

		result->finalize();

		LOG4CPLUS_DEBUG(logger, "Done converting matrix to storm format.");

		return result;
	}
};

} //namespace adapters

} //namespace storm

#endif /* STORM_ADAPTERS_GMMXXADAPTER_H_ */
