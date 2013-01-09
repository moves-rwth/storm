#ifndef STORM_STORAGE_JACOBIDECOMPOSITION_H_
#define STORM_STORAGE_JACOBIDECOMPOSITION_H_

#include "boost/integer/integer_mask.hpp"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {

namespace storage {

/*!
 * Forward declaration against Cycle
 */
template <class T>
class SparseMatrix;


/*!
 * A simple container for a sparse Jacobi decomposition
 */
template <class T>
class JacobiDecomposition {

public:
	JacobiDecomposition(storm::storage::SparseMatrix<T> * const jacobiLuMatrix, storm::storage::SparseMatrix<T> * const jacobiDInvMatrix) : jacobiLuMatrix(jacobiLuMatrix), jacobiDInvMatrix(jacobiDInvMatrix) {
	}

	~JacobiDecomposition() {
		delete this->jacobiDInvMatrix;
		delete this->jacobiLuMatrix;
	}

	/*!
	 * Accessor for the internal LU Matrix.
	 * Ownership stays with this class.
	 * @return A reference to the Jacobi LU Matrix
	 */
	storm::storage::SparseMatrix<T>& getJacobiLUReference() {
		return *(this->jacobiLuMatrix);
	}

	/*!
	 * Accessor for the internal D^{-1} Matrix.
	 * Ownership stays with this class.
	 * @return A reference to the Jacobi D^{-1} Matrix
	 */
	storm::storage::SparseMatrix<T>& getJacobiDInvReference() {
		return *(this->jacobiDInvMatrix);
	}

	/*!
	 * Accessor for the internal LU Matrix.
	 * Ownership stays with this class.
	 * @return A pointer to the Jacobi LU Matrix
	 */
	storm::storage::SparseMatrix<T>* getJacobiLU() {
		return this->jacobiLuMatrix;
	}

	/*!
	 * Accessor for the internal D^{-1} Matrix.
	 * Ownership stays with this class.
	 * @return A pointer to the Jacobi D^{-1} Matrix
	 */
	storm::storage::SparseMatrix<T>* getJacobiDInv() {
		return this->jacobiDInvMatrix;
	}

private:

	/*!
	 * The copy constructor is disabled for this class.
	 */
	//JacobiDecomposition(const JacobiDecomposition<T>& that) = delete; // not possible in VS2012
	JacobiDecomposition(const JacobiDecomposition<T>& that) {}

	/*!
	 * Pointer to the LU Matrix
	 */
	storm::storage::SparseMatrix<T> *jacobiLuMatrix;
	
	/*!
	 * Pointer to the D^{-1} Matrix
	 */
	storm::storage::SparseMatrix<T> *jacobiDInvMatrix;
};

} // namespace storage

} // namespace storm

#endif // STORM_STORAGE_JACOBIDECOMPOSITION_H_
