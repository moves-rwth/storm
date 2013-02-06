/*
 * EigenAdapter.h
 *
 *  Created on: 21.01.2013
 *      Author: Philipp Berger
 */

#ifndef STORM_ADAPTERS_EIGENADAPTER_H_
#define STORM_ADAPTERS_EIGENADAPTER_H_

#include "src/storage/SparseMatrix.h"
#include "Eigen/Sparse"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {

namespace adapters {

class EigenAdapter {
public:
	/*!
	 * Converts a sparse matrix into the sparse matrix in the eigen format.
	 * @return A pointer to a row-major sparse matrix in eigen format.
	 */
	template<class T>
	static Eigen::SparseMatrix<T, Eigen::RowMajor, int_fast32_t>* toEigenSparseMatrix(storm::storage::SparseMatrix<T> const& matrix) {
		uint_fast64_t realNonZeros = matrix.getNonZeroEntryCount();
		LOG4CPLUS_DEBUG(logger, "Converting matrix with " << realNonZeros << " non-zeros to Eigen format.");

		// Prepare the resulting matrix.
		Eigen::SparseMatrix<T, Eigen::RowMajor, int_fast32_t>* result = new Eigen::SparseMatrix<T, Eigen::RowMajor, int_fast32_t>(matrix.rowCount, matrix.colCount);

		result->resizeNonZeros(realNonZeros);
		//result->reserve(realNonZeros);

		// Copy Row Indications
		std::copy(matrix.rowIndications.begin(), matrix.rowIndications.end(), (result->outerIndexPtr()));
		// Copy Columns Indications
		std::copy(matrix.columnIndications.begin(), matrix.columnIndications.end(), (result->innerIndexPtr()));
		// And do the same thing with the actual values.
		std::copy(matrix.valueStorage.begin(), matrix.valueStorage.end(), (result->valuePtr()));

		LOG4CPLUS_DEBUG(logger, "Done converting matrix to Eigen format.");

		return result;
	}
};

} //namespace adapters

} //namespace storm

#endif /* STORM_ADAPTERS_GMMXXADAPTER_H_ */
