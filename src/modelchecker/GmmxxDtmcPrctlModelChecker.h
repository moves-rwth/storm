/*
 * GmmxxDtmcPrctlModelChecker.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_MODELCHECKER_GMMXXDTMCPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_GMMXXDTMCPRCTLMODELCHECKER_H_

#include <cmath>

#include "src/models/Dtmc.h"
#include "src/modelchecker/DtmcPrctlModelChecker.h"
#include "src/utility/Vector.h"
#include "src/utility/ConstTemplates.h"
#include "src/utility/Settings.h"
#include "src/adapters/GmmxxAdapter.h"
#include "src/exceptions/InvalidPropertyException.h"
#include "src/storage/JacobiDecomposition.h"

#include "gmm/gmm_matrix.h"
#include "gmm/gmm_iter_solvers.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {

namespace modelChecker {

/*
 * A model checking engine that makes use of the gmm++ backend.
 */
template <class Type>
class GmmxxDtmcPrctlModelChecker : public DtmcPrctlModelChecker<Type> {

public:
	explicit GmmxxDtmcPrctlModelChecker(storm::models::Dtmc<Type>& dtmc) : DtmcPrctlModelChecker<Type>(dtmc) {
		// Intentionally left empty.
	}

	virtual ~GmmxxDtmcPrctlModelChecker() {

	}

	/*!
	 * Returns the name of this module.
	 * @return The name of this module.
	 */
	static std::string getModuleName() {
		return "gmm++";
	}

	/*!
	 * Returns a trigger such that if the option "matrixlib" is set to "gmm++", this model checker
	 * is to be used.
	 * @return An option trigger for this module.
	 */
	static std::pair<std::string, std::string> getOptionTrigger() {
		return std::pair<std::string, std::string>("matrixlib", "gmm++");
	}

	/*!
	 * Registers all options associated with the gmm++ matrix library.
	 */
	static void putOptions(boost::program_options::options_description* desc) {
		desc->add_options()("lemethod", boost::program_options::value<std::string>()->default_value("bicgstab")->notifier(&validateLeMethod), "Sets the method used for linear equation solving. Must be in {bicgstab, qmr, jacobi}.");
		desc->add_options()("maxiter", boost::program_options::value<unsigned>()->default_value(10000), "Sets the maximal number of iterations for iterative equation solving.");
		desc->add_options()("precision", boost::program_options::value<double>()->default_value(1e-6), "Sets the precision for iterative equation solving.");
		desc->add_options()("precond", boost::program_options::value<std::string>()->default_value("ilu")->notifier(&validatePreconditioner), "Sets the preconditioning technique for linear equation solving. Must be in {ilu, diagonal, ildlt, none}.");
		desc->add_options()("relative", boost::program_options::value<bool>()->default_value(true), "Sets whether the relative or absolute error is considered for detecting convergence.");
	}

	/*!
	 * Validates whether the given lemethod matches one of the available ones.
	 * Throws an exception of type InvalidSettings in case the selected method is illegal.
	 */
	static void validateLeMethod(const std::string& lemethod) {
		if ((lemethod != "bicgstab") && (lemethod != "qmr") && (lemethod != "jacobi")) {
			throw exceptions::InvalidSettingsException() << "Argument " << lemethod << " for option 'lemethod' is invalid.";
		}
	}

	/*!
	 * Validates whether the given preconditioner matches one of the available ones.
	 * Throws an exception of type InvalidSettings in case the selected preconditioner is illegal.
	 */
	static void validatePreconditioner(const std::string& preconditioner) {
		if ((preconditioner != "ilu") && (preconditioner != "diagonal") && (preconditioner != "ildlt") && (preconditioner != "none")) {
			throw exceptions::InvalidSettingsException() << "Argument " << preconditioner << " for option 'precond' is invalid.";
		}
	}

private:

	virtual void performMatrixVectorMultiplication(storm::storage::SparseMatrix<Type> const& matrix, std::vector<Type>** vector, std::vector<Type>* summand, uint_fast64_t repetitions = 1) const {
		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(matrix);

		// Now perform matrix-vector multiplication as long as we meet the bound.
		std::vector<Type>* swap = nullptr;
		std::vector<Type>* tmpResult = new std::vector<Type>(this->getModel().getNumberOfStates());
		for (uint_fast64_t i = 0; i < repetitions; ++i) {
			gmm::mult(*gmmxxMatrix, **vector, *tmpResult);
			swap = tmpResult;
			tmpResult = *vector;
			*vector = swap;

			// If requested, add an offset to the current result vector.
			if (summand != nullptr) {
				gmm::add(*summand, **vector);
			}
		}
		delete tmpResult;
		delete gmmxxMatrix;
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

		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<Type>* A = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(matrix);

		// Prepare an iteration object that determines the accuracy, maximum number of iterations
		// and the like.
		gmm::iteration iter(s->get<double>("precision"), 0, s->get<unsigned>("maxiter"));

		// Now do the actual solving.
		LOG4CPLUS_INFO(logger, "Starting iterative solver.");
		const std::string& precond = s->getString("precond");
		if (precond == "ilu") {
			LOG4CPLUS_INFO(logger, "Using ILU preconditioner.");
		} else if (precond == "diagonal") {
			LOG4CPLUS_INFO(logger, "Using diagonal preconditioner.");
		} else if (precond == "ildlt") {
			LOG4CPLUS_INFO(logger, "Using ILDLT preconditioner.");
		} else if (precond == "none") {
			LOG4CPLUS_INFO(logger, "Using no preconditioner.");
		}

		if (s->getString("lemethod") == "bicgstab") {
			LOG4CPLUS_INFO(logger, "Using BiCGStab method.");
			if (precond == "ilu") {
				gmm::bicgstab(*A, **vector, b, gmm::ilu_precond<gmm::csr_matrix<Type>>(*A), iter);
			} else if (precond == "diagonal") {
				gmm::bicgstab(*A, **vector, b, gmm::diagonal_precond<gmm::csr_matrix<Type>>(*A), iter);
			} else if (precond == "ildlt") {
				gmm::bicgstab(*A, **vector, b, gmm::ildlt_precond<gmm::csr_matrix<Type>>(*A), iter);
			} else if (precond == "none") {
				gmm::bicgstab(*A, **vector, b, gmm::identity_matrix(), iter);
			}

			// Check if the solver converged and issue a warning otherwise.
			if (iter.converged()) {
				LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iter.get_iteration() << " iterations.");
			} else {
				LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
			}
		} else if (s->getString("lemethod") == "qmr") {
			LOG4CPLUS_INFO(logger, "Using QMR method.");
			if (precond == "ilu") {
				gmm::qmr(*A, **vector, b, gmm::ilu_precond<gmm::csr_matrix<Type>>(*A), iter);
			} else if (precond == "diagonal") {
				gmm::qmr(*A, **vector, b, gmm::diagonal_precond<gmm::csr_matrix<Type>>(*A), iter);
			} else if (precond == "ildlt") {
				gmm::qmr(*A, **vector, b, gmm::ildlt_precond<gmm::csr_matrix<Type>>(*A), iter);
			} else if (precond == "none") {
				gmm::qmr(*A, **vector, b, gmm::identity_matrix(), iter);
			}

			// Check if the solver converged and issue a warning otherwise.
			if (iter.converged()) {
				LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iter.get_iteration() << " iterations.");
			} else {
				LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
			}
		} else if (s->getString("lemethod") == "jacobi") {
			LOG4CPLUS_INFO(logger, "Using Jacobi method.");
			solveLinearEquationSystemWithJacobi(*A, **vector, b);
		}

		delete A;
	}

	/*!
	 * Solves the linear equation system Ax=b with the given parameters
	 * using the Jacobi Method and therefor the Jacobi Decomposition of A.
	 *
	 * @param A The matrix A specifying the coefficients of the linear equations.
	 * @param x The vector x for which to solve the equations. The initial value of the elements of
	 * this vector are used as the initial guess and might thus influence performance and convergence.
	 * @param b The vector b specifying the values on the right-hand-sides of the equations.
	 * @return The solution of the system of linear equations in form of the elements of the vector
	 * x.
	 */
	void solveLinearEquationSystemWithJacobi(gmm::csr_matrix<Type> const& A, std::vector<Type>& x, std::vector<Type> const& b) const {
		// Get the settings object to customize linear solving.
		storm::settings::Settings* s = storm::settings::instance();

		double precision = s->get<double>("precision");
		if (precision <= 0) {
			LOG4CPLUS_ERROR(logger, "Selected precision for linear equation solving must be strictly greater than zero for Jacobi method.");
		}

		// Convert the Source Matrix to Storm format for Decomposition
		storm::storage::SparseMatrix<Type>* stormFormatA = storm::adapters::GmmxxAdapter::fromGmmxxSparseMatrix(A);

		// Get a Jacobi Decomposition of the Input Matrix A
		storm::storage::JacobiDecomposition<Type>* jacobiDecomposition = stormFormatA->getJacobiDecomposition();

		// The Storm Version is not needed after the decomposition step
		delete stormFormatA;

		// Convert the Diagonal matrix to GMM format
		gmm::csr_matrix<Type>* gmmxxDiagonalInverted = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(jacobiDecomposition->getJacobiDInvReference());
		// Convert the LU Matrix to GMM format
		gmm::csr_matrix<Type>* gmmxxLU = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(jacobiDecomposition->getJacobiLUReference());


		LOG4CPLUS_INFO(logger, "Starting iterative Jacobi Solver.");
		
		// x_(k + 1) = D^-1 * (b  - R * x_k)
		// In x we keep a copy of the result for swapping in the loop (e.g. less copy-back)
		std::vector<Type>* xNext = new std::vector<Type>(x.size());
		const std::vector<Type>* xCopy = xNext;
		std::vector<Type>* xCurrent = &x;

		// Target vector for precision calculation
		std::vector<Type>* residuum = new std::vector<Type>(x.size());

		uint_fast64_t iterationCount = 0;
		do {
			// R * x_k (xCurrent is x_k) -> xNext
			gmm::mult(*gmmxxLU, *xCurrent, *xNext);
			// b - R * x_k (xNext contains R * x_k) -> xNext
			gmm::add(b, gmm::scaled(*xNext, -1.0), *xNext);
			// D^-1 * (b - R * x_k) -> xNext
			gmm::mult(*gmmxxDiagonalInverted, *xNext, *xNext);
			
			// swap xNext with xCurrent so that the next iteration can use xCurrent again without having to copy the vector
			std::vector<Type>* swap = xNext;
			xNext = xCurrent;
			xCurrent = swap;

			++iterationCount;
			// Precision calculation via ||A * x_k - b|| < precision
			gmm::mult(A, *xCurrent, *residuum);
			gmm::add(gmm::scaled(*residuum, -1.0), b, *residuum);
		} while (gmm::vect_norminf(*residuum) > precision);

		// If the last iteration did not write to the original x
		// we have to swith them
		if (xCurrent == xCopy) {
			x.swap(*xCurrent);
		}

		// xCopy always points to the Swap-Copy of x we created
		delete xCopy;
		// Delete the residuum vector
		delete residuum;
		// Delete the decompositions
		delete jacobiDecomposition;
		// and the GMM Matrices
		delete gmmxxDiagonalInverted;
		delete gmmxxLU;

		LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iterationCount << " iterations.");
	}
};

} //namespace modelChecker

} //namespace storm

#endif /* STORM_MODELCHECKER_GMMXXDTMCPRCTLMODELCHECKER_H_ */
