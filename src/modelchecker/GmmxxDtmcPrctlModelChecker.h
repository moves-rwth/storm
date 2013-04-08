/*
 * GmmxxDtmcPrctlModelChecker.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_MODELCHECKER_GMMXXDTMCPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_GMMXXDTMCPRCTLMODELCHECKER_H_

#include "src/modelchecker/SparseDtmcPrctlModelChecker.h"
#include "src/adapters/GmmxxAdapter.h"
#include "src/storage/JacobiDecomposition.h"
#include "src/utility/ConstTemplates.h"
#include "src/utility/Settings.h"

#include "gmm/gmm_matrix.h"
#include "gmm/gmm_iter_solvers.h"

#include <cmath>

namespace storm {
namespace modelchecker {

/*
 * An implementation of the SparseDtmcPrctlModelChecker interface that uses gmm++ as the solving backend.
 */
template <class Type>
class GmmxxDtmcPrctlModelChecker : public SparseDtmcPrctlModelChecker<Type> {

public:
	/*!
	 * Constructs a GmmxxDtmcPrctlModelChecker with the given model.
	 *
	 * @param model The DTMC to be checked.
	 */
	explicit GmmxxDtmcPrctlModelChecker(storm::models::Dtmc<Type>& dtmc) : SparseDtmcPrctlModelChecker<Type>(dtmc) {
		// Intentionally left empty.
	}

	/*!
	 * Copy constructs a GmmxxDtmcPrctlModelChecker from the given model checker. In particular, this means that the newly
	 * constructed model checker will have the model of the given model checker as its associated model.
	 */
	explicit GmmxxDtmcPrctlModelChecker(storm::modelchecker::GmmxxDtmcPrctlModelChecker<Type> const& modelchecker) : SparseDtmcPrctlModelChecker<Type>(modelchecker) {
		// Intentionally left empty.
	}

	/*!
	 * Virtual destructor. Needs to be virtual, because this class has virtual methods.
	 */
	virtual ~GmmxxDtmcPrctlModelChecker() {
		// Intentionally left empty.
	}

	/*!
	 * Returns the name of this module.
	 * @returns The name of this module.
	 */
	static std::string getModuleName() {
		return "gmm++det";
	}

	/*!
	 * Returns a trigger such that if the option "matrixlib" is set to "gmm++", this model checker
	 * is to be used.
	 * @returns An option trigger for this module.
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

	virtual void performMatrixVectorMultiplication(storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type>* b, uint_fast64_t n = 1) const {
		// Transform the transition probability A to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(A);

		// Set up some temporary variables so that we can just swap pointers instead of copying the result after each
		// iteration.
		std::vector<Type>* swap = nullptr;
		std::vector<Type>* currentVector = &x;
		std::vector<Type>* tmpVector = new std::vector<Type>(this->getModel().getNumberOfStates());

		// Now perform matrix-vector multiplication as long as we meet the bound.
		for (uint_fast64_t i = 0; i < n; ++i) {
			gmm::mult(*gmmxxMatrix, *currentVector, *tmpVector);
			swap = tmpVector;
			tmpVector = currentVector;
			currentVector = swap;

			// If requested, add an offset to the current result vector.
			if (b != nullptr) {
				gmm::add(*b, *currentVector);
			}
		}

		// If we performed an odd number of repetitions, we need to swap the contents of currentVector and x, because
		// the output is supposed to be stored in x.
		if (n % 2 == 1) {
			std::swap(x, *currentVector);
			delete currentVector;
		} else {
			delete tmpVector;
		}

		delete gmmxxMatrix;
	}

	virtual void solveEquationSystem(storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type> const& b) const {
		// Get the settings object to customize linear solving.
		storm::settings::Settings* s = storm::settings::instance();

		// Prepare an iteration object that determines the accuracy, maximum number of iterations
		// and the like.
		gmm::iteration iter(s->get<double>("precision"), 0, s->get<unsigned>("maxiter"));

		// Print some information about the used preconditioner.
		const std::string& precond = s->getString("precond");
		LOG4CPLUS_INFO(logger, "Starting iterative solver.");
		if (s->getString("lemethod") == "jacobi") {
			if (precond != "none") {
				LOG4CPLUS_WARN(logger, "Requested preconditioner '" << precond << "', which is unavailable for the Jacobi method. Dropping preconditioner.");
			}
		} else {
			if (precond == "ilu") {
				LOG4CPLUS_INFO(logger, "Using ILU preconditioner.");
			} else if (precond == "diagonal") {
				LOG4CPLUS_INFO(logger, "Using diagonal preconditioner.");
			} else if (precond == "ildlt") {
				LOG4CPLUS_INFO(logger, "Using ILDLT preconditioner.");
			} else if (precond == "none") {
				LOG4CPLUS_INFO(logger, "Using no preconditioner.");
			}
		}

		// Now do the actual solving.
		if (s->getString("lemethod") == "bicgstab") {
			LOG4CPLUS_INFO(logger, "Using BiCGStab method.");
			// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
			gmm::csr_matrix<Type>* gmmA = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(A);
			if (precond == "ilu") {
				gmm::bicgstab(*gmmA, x, b, gmm::ilu_precond<gmm::csr_matrix<Type>>(*gmmA), iter);
			} else if (precond == "diagonal") {
				gmm::bicgstab(*gmmA, x, b, gmm::diagonal_precond<gmm::csr_matrix<Type>>(*gmmA), iter);
			} else if (precond == "ildlt") {
				gmm::bicgstab(*gmmA, x, b, gmm::ildlt_precond<gmm::csr_matrix<Type>>(*gmmA), iter);
			} else if (precond == "none") {
				gmm::bicgstab(*gmmA, x, b, gmm::identity_matrix(), iter);
			}

			// Check if the solver converged and issue a warning otherwise.
			if (iter.converged()) {
				LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iter.get_iteration() << " iterations.");
			} else {
				LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
			}
			delete gmmA;
		} else if (s->getString("lemethod") == "qmr") {
			LOG4CPLUS_INFO(logger, "Using QMR method.");
			// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
			gmm::csr_matrix<Type>* gmmA = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(A);
			if (precond == "ilu") {
				gmm::qmr(*gmmA, x, b, gmm::ilu_precond<gmm::csr_matrix<Type>>(*gmmA), iter);
			} else if (precond == "diagonal") {
				gmm::qmr(*gmmA, x, b, gmm::diagonal_precond<gmm::csr_matrix<Type>>(*gmmA), iter);
			} else if (precond == "ildlt") {
				gmm::qmr(*gmmA, x, b, gmm::ildlt_precond<gmm::csr_matrix<Type>>(*gmmA), iter);
			} else if (precond == "none") {
				gmm::qmr(*gmmA, x, b, gmm::identity_matrix(), iter);
			}

			// Check if the solver converged and issue a warning otherwise.
			if (iter.converged()) {
				LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iter.get_iteration() << " iterations.");
			} else {
				LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
			}
			delete gmmA;
		} else if (s->getString("lemethod") == "jacobi") {
			LOG4CPLUS_INFO(logger, "Using Jacobi method.");
			uint_fast64_t iterations = solveLinearEquationSystemWithJacobi(A, x, b);

			// Check if the solver converged and issue a warning otherwise.
			if (iterations < s->get<unsigned>("maxiter")) {
				LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iterations << " iterations.");
			} else {
				LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
			}
		}
	}

	/*!
	 * Solves the linear equation system A*x = b given by the parameters using the Jacobi method.
	 *
	 * @param A The matrix specifying the coefficients of the linear equations.
	 * @param x The solution vector x. The initial values of x represent a guess of the real values to the solver, but
	 * may be ignored.
	 * @param b The right-hand side of the equation system.
	 * @returns The solution vector x of the system of linear equations as the content of the parameter x.
	 * @returns The number of iterations needed until convergence.
	 */
	uint_fast64_t solveLinearEquationSystemWithJacobi(storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type> const& b) const {
		// Get the settings object to customize linear solving.
		storm::settings::Settings* s = storm::settings::instance();

		double precision = s->get<double>("precision");
		uint_fast64_t maxIterations = s->get<unsigned>("maxiter");
		bool relative = s->get<bool>("relative");

		// Get a Jacobi decomposition of the matrix A.
		storm::storage::JacobiDecomposition<Type>* jacobiDecomposition = A.getJacobiDecomposition();

		// Convert the (inverted) diagonal matrix to gmm++'s format.
		gmm::csr_matrix<Type>* gmmxxDiagonalInverted = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(jacobiDecomposition->getJacobiDInvReference());
		// Convert the LU matrix to gmm++'s format.
		gmm::csr_matrix<Type>* gmmxxLU = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(jacobiDecomposition->getJacobiLUReference());

		LOG4CPLUS_INFO(logger, "Starting iterative Jacobi Solver.");
		
		// x_(k + 1) = D^-1 * (b  - R * x_k)
		// In x we keep a copy of the result for swapping in the loop (e.g. less copy-back).
		std::vector<Type>* xNext = new std::vector<Type>(x.size());
		const std::vector<Type>* xCopy = xNext;
		std::vector<Type>* xCurrent = &x;

		// Target vector for precision calculation.
		std::vector<Type>* residuum = new std::vector<Type>(x.size());

		// Set up additional environment variables.
		uint_fast64_t iterationCount = 0;
		bool converged = false;

		while (!converged && iterationCount < maxIterations) {
			// R * x_k (xCurrent is x_k) -> xNext
			gmm::mult(*gmmxxLU, *xCurrent, *xNext);
			// b - R * x_k (xNext contains R * x_k) -> xNext
			gmm::add(b, gmm::scaled(*xNext, -1.0), *xNext);
			// D^-1 * (b - R * x_k) -> xNext
			gmm::mult(*gmmxxDiagonalInverted, *xNext, *xNext);
			
			// Swap xNext with xCurrent so that the next iteration can use xCurrent again without having to copy the
			// vector.
			std::vector<Type> *const swap = xNext;
			xNext = xCurrent;
			xCurrent = swap;

			// Now check if the process already converged within our precision.
			converged = storm::utility::equalModuloPrecision(*xCurrent, *xNext, precision, relative);

			// Increase iteration count so we can abort if convergence is too slow.
			++iterationCount;
		}

		// If the last iteration did not write to the original x we have to swap the contents, because the output has to
		// be written to the parameter x.
		if (xCurrent == xCopy) {
			std::swap(x, *xCurrent);
		}

		// As xCopy always points to the copy of x used for swapping, we can safely delete it.
		delete xCopy;

		// Also delete the other dynamically allocated variables.
		delete residuum;
		delete jacobiDecomposition;
		delete gmmxxDiagonalInverted;
		delete gmmxxLU;

		return iterationCount;
	}
};

} // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_GMMXXDTMCPRCTLMODELCHECKER_H_ */
