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

#include "src/utility/OsDetection.h"

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
		Eigen::SparseMatrix<T, Eigen::RowMajor, int_fast32_t>* result = new Eigen::SparseMatrix<T, Eigen::RowMajor, int_fast32_t>(static_cast<int>(matrix.rowCount), static_cast<int>(matrix.colCount));

		result->resizeNonZeros(static_cast<int>(realNonZeros));
		//result->reserve(realNonZeros);
#ifdef WINDOWS
#	pragma warning(push)
#	pragma warning(disable: 4244) // possible loss of data
#endif
		// Copy Row Indications
		std::copy(matrix.rowIndications.begin(), matrix.rowIndications.end(), (result->outerIndexPtr()));
		// Copy Columns Indications
		std::copy(matrix.columnIndications.begin(), matrix.columnIndications.end(), (result->innerIndexPtr()));
		// And do the same thing with the actual values.
		std::copy(matrix.valueStorage.begin(), matrix.valueStorage.end(), (result->valuePtr()));

#ifdef WINDOWS
#	pragma warning(pop)
#endif

		LOG4CPLUS_DEBUG(logger, "Done converting matrix to Eigen format.");

		return result;
	}
};

} //namespace adapters

} //namespace storm

#endif /* STORM_ADAPTERS_EIGENADAPTER_H_ */
