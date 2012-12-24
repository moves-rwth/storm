/*
 * GmmxxAdapter.h
 *
 *  Created on: 24.12.2012
 *      Author: Christian Dehnert
 */

#ifndef GMMXXADAPTER_H_
#define GMMXXADAPTER_H_

#include "src/storage/SquareSparseMatrix.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace mrmc {

namespace adapters {

class GmmxxAdapter {
public:
	/*!
	 * Converts a sparse matrix into the sparse matrix in the gmm++ format.
	 * @return A pointer to a column-major sparse matrix in gmm++ format.
	 */
	template<class T>
	static gmm::csr_matrix<T>* toGmmxxSparseMatrix(mrmc::storage::SquareSparseMatrix<T> const& matrix) {
		uint_fast64_t realNonZeros = matrix.getNonZeroEntryCount() + matrix.getDiagonalNonZeroEntryCount();
		LOG4CPLUS_DEBUG(logger, "Converting matrix with " << realNonZeros << " non-zeros to gmm++ format.");

		// Prepare the resulting matrix.
		gmm::csr_matrix<T>* result = new gmm::csr_matrix<T>(matrix.rowCount, matrix.rowCount);

		// Reserve enough elements for the row indications.
		result->jc.reserve(matrix.rowCount + 1);

		// For the column indications and the actual values, we have to gather
		// the values in a temporary array first, as we have to integrate
		// the values from the diagonal. For the row indications, we can just count the number of
		// inserted diagonal elements and add it to the previous value.
		uint_fast64_t* tmpColumnIndicationsArray = new uint_fast64_t[realNonZeros];
		T* tmpValueArray = new T[realNonZeros];
		T zero(0);
		uint_fast64_t currentPosition = 0;
		uint_fast64_t insertedDiagonalElements = 0;
		for (uint_fast64_t i = 0; i < matrix.rowCount; ++i) {
			// Compute correct start index of row.
			result->jc[i] = matrix.rowIndications[i] + insertedDiagonalElements;

			// If the current row has no non-zero which is not on the diagonal, we have to check the
			// diagonal element explicitly.
			if (matrix.rowIndications[i + 1] - matrix.rowIndications[i] == 0) {
				if (matrix.diagonalStorage[i] != zero) {
					tmpColumnIndicationsArray[currentPosition] = i;
					tmpValueArray[currentPosition] = matrix.diagonalStorage[i];
					++currentPosition; ++insertedDiagonalElements;
				}
			} else {
				// Otherwise, we can just enumerate the non-zeros which are not on the diagonal
				// and fit in the diagonal element where appropriate.
				bool includedDiagonal = false;
				for (uint_fast64_t j = matrix.rowIndications[i]; j < matrix.rowIndications[i + 1]; ++j) {
					if (matrix.diagonalStorage[i] != zero && !includedDiagonal && matrix.columnIndications[j] > i) {
						includedDiagonal = true;
						tmpColumnIndicationsArray[currentPosition] = i;
						tmpValueArray[currentPosition] = matrix.diagonalStorage[i];
						++currentPosition; ++insertedDiagonalElements;
					}
					tmpColumnIndicationsArray[currentPosition] = matrix.columnIndications[j];
					tmpValueArray[currentPosition] = matrix.valueStorage[j];
					++currentPosition;
				}

				// If the diagonal element is non-zero and was not inserted until now (i.e. all
				// off-diagonal elements in the row are before the diagonal element.
				if (!includedDiagonal && matrix.diagonalStorage[i] != zero) {
					tmpColumnIndicationsArray[currentPosition] = i;
					tmpValueArray[currentPosition] = matrix.diagonalStorage[i];
					++currentPosition; ++insertedDiagonalElements;
				}
			}
		}
		// Fill in sentinel element at the end.
		result->jc[matrix.rowCount] = static_cast<unsigned int>(realNonZeros);

		// Now, we can copy the temporary array to the GMMXX format.
		result->ir.resize(realNonZeros);
		std::copy(tmpColumnIndicationsArray, tmpColumnIndicationsArray + realNonZeros, result->ir.begin());
		delete[] tmpColumnIndicationsArray;

		// And do the same thing with the actual values.
		result->pr.resize(realNonZeros);
		std::copy(tmpValueArray, tmpValueArray + realNonZeros, result->pr.begin());
		delete[] tmpValueArray;

		LOG4CPLUS_DEBUG(logger, "Done converting matrix to gmm++ format.");

		return result;
	}
};

} //namespace adapters

} //namespace mrmc

#endif /* GMMXXADAPTER_H_ */
