/*
 * EigenDtmcPrctlModelChecker.h
 *
 *  Created on: 07.12.2012
 *      Author: 
 */

#ifndef STORM_MODELCHECKER_EIGENDTMCPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_EIGENDTMCPRCTLMODELCHECKER_H_

#include "src/utility/Vector.h"

#include "src/models/Dtmc.h"
#include "src/modelchecker/DtmcPrctlModelChecker.h"
#include "src/utility/GraphAnalyzer.h"
#include "src/utility/ConstTemplates.h"
#include "src/exceptions/NoConvergenceException.h"

#include "Eigen/Sparse"
#include "Eigen/src/IterativeLinearSolvers/BiCGSTAB.h"
#include "src/adapters/EigenAdapter.h"

#include "gmm/gmm_matrix.h"
#include "gmm/gmm_iter_solvers.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {

namespace modelChecker {

/*
 * A model checking engine that makes use of the eigen backend.
 */
template <class Type>
class EigenDtmcPrctlModelChecker : public DtmcPrctlModelChecker<Type> {

typedef Eigen::Matrix<Type, -1, 1, 0, -1, 1> VectorType;
typedef Eigen::Map<VectorType> MapType;

public:
	explicit EigenDtmcPrctlModelChecker(storm::models::Dtmc<Type>& dtmc) : DtmcPrctlModelChecker<Type>(dtmc) {
		// Intentionally left empty.
	}

	virtual ~EigenDtmcPrctlModelChecker() {
		// Intentionally left empty.
	}

private:
	virtual void performMatrixVectorMultiplication(storm::storage::SparseMatrix<Type> const& matrix, std::vector<Type>** vector, std::vector<Type>* summand, uint_fast64_t repetitions = 1) const {
		// Transform the transition probability matrix to the eigen format to use its arithmetic.
		Eigen::SparseMatrix<Type, 1, int_fast32_t>* eigenMatrix = storm::adapters::EigenAdapter::toEigenSparseMatrix(matrix);

		Type* p = &((**vector)[0]); // get the address storing the data for result
		MapType vectorMap(p, (*vector)->size()); // vectorMap shares data

		p = &((*summand)[0]);
		MapType summandMap(p, summand->size());

		// Now perform matrix-vector multiplication as long as we meet the bound.
		std::vector<Type>* swap = nullptr;
		std::vector<Type>* tmpResult = new std::vector<Type>(this->getModel().getNumberOfStates());
		for (uint_fast64_t i = 0; i < repetitions; ++i) {
			vectorMap = (*eigenMatrix) * (vectorMap);

			// If requested, add an offset to the current result vector.
			if (summand != nullptr) {
				vectorMap = vectorMap + summandMap;
			}
		}
		delete eigenMatrix;
	}

	/*!
	 * Solves the linear equation system Ax=b with the given parameters.
	 *
	 * @param A The matrix A specifying the coefficients of the linear equations.
	 * @param x The vector x for which to solve the equations. The initial value of the elements of
	 * this vector are used as the initial guess and might thus influence performance and convergence.
	 * @param b The vector b specifying the values on the right-hand-sides of the equations.
	 * @return The solution of the system of linear equations in form of the elements of the vector
	 * x.
	 */
	virtual void solveEquationSystem(storm::storage::SparseMatrix<Type> const& matrix, std::vector<Type>** vector, std::vector<Type>& b) const {
		// Get the settings object to customize linear solving.
		storm::settings::Settings* s = storm::settings::instance();

		// Transform the submatric matrix to the eigen format to use its solvers
		Eigen::SparseMatrix<Type, 1, int_fast32_t>* eigenMatrix = storm::adapters::EigenAdapter::toEigenSparseMatrix<Type>(matrix);

		Eigen::BiCGSTAB<Eigen::SparseMatrix<Type, 1, int_fast32_t>> solver;
		solver.compute(*eigenMatrix);
		if(solver.info()!= Eigen::ComputationInfo::Success) {
			// decomposition failed
			LOG4CPLUS_ERROR(logger, "Decomposition of matrix failed!");
		}
		solver.setMaxIterations(s->get<unsigned>("maxiter"));
		solver.setTolerance(s->get<double>("precision"));

		std::cout << matrix.toString(nullptr) << std::endl;
		std::cout << **vector << std::endl;
		std::cout << b << std::endl;

		std::cout << *eigenMatrix << std::endl;

		// Map for x
		Type *px = &((**vector)[0]); // get the address storing the data for x
		MapType vectorX(px, (*vector)->size()); // vectorX shares data

		Type *pb = &(b[0]); // get the address storing the data for b
		MapType vectorB(pb, b.size()); // vectorB shares data

		vectorX = solver.solveWithGuess(vectorB, vectorX);

		std::cout << **vector << std::endl;

		if(solver.info() == Eigen::ComputationInfo::InvalidInput) {
			// solving failed
			LOG4CPLUS_ERROR(logger, "Solving of Submatrix failed: InvalidInput");
		} else if(solver.info() == Eigen::ComputationInfo::NoConvergence) {
			// NoConvergence
			throw storm::exceptions::NoConvergenceException() << "Failed to converge within " << solver.iterations() << " out of a maximum of " << solver.maxIterations() << " iterations.";
		} else if(solver.info() == Eigen::ComputationInfo::NumericalIssue) {
			// NumericalIssue
			LOG4CPLUS_ERROR(logger, "Solving of Submatrix failed: NumericalIssue");
		} else if(solver.info() == Eigen::ComputationInfo::Success) {
			// solving Success
			LOG4CPLUS_INFO(logger, "Solving of Submatrix succeeded: Success");
		}

		delete eigenMatrix;
	}
};

} //namespace modelChecker

} //namespace storm

#endif /* STORM_MODELCHECKER_EIGENDTMCPRCTLMODELCHECKER_H_ */
