/*
 * StormAdapter.h
 *
 *  Created on: 02.03.2013
 *      Author: Philipp Berger
 */

#ifndef STORM_ADAPTERS_STORMADAPTER_H_
#define STORM_ADAPTERS_STORMADAPTER_H_

#include "gmm/gmm_matrix.h"
#include "Eigen/Sparse"
#include "src/storage/SparseMatrix.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {

namespace adapters {

class StormAdapter {
public:
	/*!
	 * Converts a sparse matrix into a sparse matrix in the storm format.
	 * @return A pointer to a row-major sparse matrix in storm format.
	 */
	template<class T>
	static storm::storage::SparseMatrix<T>* toStormSparseMatrix(gmm::csr_matrix<T> const& matrix) {
		uint_fast64_t realNonZeros = gmm::nnz(matrix);
		LOG4CPLUS_DEBUG(logger, "Converting matrix with " << realNonZeros << " non-zeros from gmm++ format into Storm.");

		// Prepare the resulting matrix.
		storm::storage::SparseMatrix<T>* result = new storm::storage::SparseMatrix<T>(matrix.ncols(), matrix.jc, matrix.ir, matrix.pr);

		LOG4CPLUS_DEBUG(logger, "Done converting matrix to storm format.");

		return result;
	}

	/*!
	 * Helper function to determine whether the given Eigen matrix is in RowMajor
	 * format. Always returns true, but is overloaded, so the compiler will
	 * only call it in case the Eigen matrix is in RowMajor format.
	 * @return True.
	 */
	template<typename _Scalar, typename _Index>
	static bool isEigenRowMajor(Eigen::SparseMatrix<_Scalar, Eigen::RowMajor, _Index>) {
		return true;
	}

	/*!
	 * Helper function to determine whether the given Eigen matrix is in RowMajor
	 * format. Always returns false, but is overloaded, so the compiler will
	 * only call it in case the Eigen matrix is in ColMajor format.
	 * @return False.
	 */
	template<typename _Scalar, typename _Index>
	static bool isEigenRowMajor(
			Eigen::SparseMatrix<_Scalar, Eigen::ColMajor, _Index>) {
		return false;
	}

	/*!
	 * Converts a sparse matrix in the eigen format to Storm Sparse Matrix format.
	 * @return A pointer to a row-major sparse matrix in our format.
	 */
	template<class T, int _Options, typename _Index>
	static storm::storage::SparseMatrix<T>* toStormSparseMatrix(Eigen::SparseMatrix<T, _Options, _Index> const& matrix) {
		uint_fast64_t realNonZeros = matrix.nonZeros();
		LOG4CPLUS_DEBUG(logger, "Converting matrix with " << realNonZeros << " non-zeros from Eigen3 format into Storm.");

		// Throw an error in case the matrix is not in compressed format.
		if (!matrix.isCompressed()) {
			LOG4CPLUS_ERROR(logger, "Trying to convert from an Eigen matrix that is not in compressed form.");
			return nullptr;
		}

		/*
		 * Try to prepare the internal storage and throw an error in case of
		 * failure.
		 */

		// Get necessary pointers to the contents of the Eigen matrix.
		const T* valuePtr = matrix.valuePtr();
		const _Index* indexPtr = matrix.innerIndexPtr();
		const _Index* outerPtr = matrix.outerIndexPtr();

		const uint_fast64_t outerSize = matrix.outerSize();

		storm::storage::SparseMatrix<T>* result = nullptr;

		// If the given matrix is in RowMajor format, copying can simply
		// be done by adding all values in order.
		// Direct copying is, however, prevented because we have to
		// separate the diagonal entries from others.
		if (isEigenRowMajor(matrix)) {
			/* Because of the RowMajor format outerSize evaluates to the
			 * number of rows.
			 * Prepare the resulting matrix.
			 */
			result = new storm::storage::SparseMatrix<T>(matrix.outerSize(), matrix.innerSize());
		
			// Set internal NonZero Counter
			result->nonZeroEntryCount = realNonZeros;
			result->setState(result->Initialized);

			if (!result->prepareInternalStorage(false)) {
				LOG4CPLUS_ERROR(logger, "Unable to allocate internal storage while converting Eigen3 RM Matrix to Storm.");
				delete result;
				return nullptr;
			} else {
			
				// Copy Row Indications
				std::copy(outerPtr, outerPtr + outerSize, std::back_inserter(result->rowIndications));
				// Copy Columns Indications
				std::copy(indexPtr, indexPtr + realNonZeros, std::back_inserter(result->columnIndications));
				// And do the same thing with the actual values.
				std::copy(valuePtr, valuePtr + realNonZeros, std::back_inserter(result->valueStorage));

				// This is our Sentinel Element.
				result->rowIndications.push_back(realNonZeros);

				result->currentSize = realNonZeros;
				result->lastRow = outerSize - 1;
			}
		} else {
			/* Because of the RowMajor format outerSize evaluates to the
			 * number of columns.
			 * Prepare the resulting matrix.
			 */
			const uint_fast64_t colCount = matrix.outerSize();
			result = new storm::storage::SparseMatrix<T>(matrix.innerSize(), colCount);
		
			// Set internal NonZero Counter
			result->nonZeroEntryCount = realNonZeros;
			result->setState(result->Initialized);
			if (!result->prepareInternalStorage()) {
				LOG4CPLUS_ERROR(logger, "Unable to allocate internal storage while converting Eigen3 CM Matrix to Storm.");
				delete result;
				return nullptr;
			} else {
				// Set internal NonZero Counter
				result->nonZeroEntryCount = realNonZeros;
				result->setState(result->Initialized);

				// Create an array to remember which elements have to still
				// be searched in each column and initialize it with the starting
				// index for every column.
				_Index* positions = new _Index[colCount]();
				for (_Index i = 0; i < colCount; ++i) {
					positions[i] = outerPtr[i];
				}

				// Now copy the elements. As the matrix is in ColMajor format,
				// we need to iterate over the columns to find the next non-zero
				// entry.
				uint_fast64_t i = 0;
				int currentRow = 0;
				int currentColumn = 0;
				while (i < realNonZeros) {
					// If the current element belongs the the current column,
					// add it in case it is also in the current row.
					if ((positions[currentColumn] < outerPtr[currentColumn + 1])
							&& (indexPtr[positions[currentColumn]] == currentRow)) {
						result->addNextValue(currentRow, currentColumn,	valuePtr[positions[currentColumn]]);
						// Remember that we found one more non-zero element.
						++i;
						// Mark this position as "used".
						++positions[currentColumn];
					}

					// Now we can advance to the next column and also row,
					// in case we just iterated through the last column.
					++currentColumn;
					if (currentColumn == colCount) {
						currentColumn = 0;
						++currentRow;
					}
				}
				delete[] positions;
			}
		}
		result->setState(result->Initialized);
		result->finalize();

		LOG4CPLUS_DEBUG(logger, "Done converting matrix to storm format.");

		return result;
	}
};

} //namespace adapters

} //namespace storm

#endif /* STORM_ADAPTERS_STORMADAPTER_H_ */
