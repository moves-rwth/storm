/*
 * GmmxxDtmcPrctlModelChecker.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_MODELCHECKER_GMMXXMDPPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_GMMXXMDPPRCTLMODELCHECKER_H_

#include "src/modelchecker/SparseMdpPrctlModelChecker.h"
#include "src/adapters/GmmxxAdapter.h"

#include "gmm/gmm_matrix.h"
#include "gmm/gmm_iter_solvers.h"

#include <cmath>

namespace storm {
namespace modelchecker {

/*
 * An implementation of the SparseMdpPrctlModelChecker interface that uses gmm++ as the solving backend.
 */
template <class Type>
class GmmxxMdpPrctlModelChecker : public SparseMdpPrctlModelChecker<Type> {

public:
	/*!
	 * Constructs a GmmxxMdpPrctlModelChecker with the given model.
	 *
	 * @param model The MDP to be checked.
	 */
	explicit GmmxxMdpPrctlModelChecker(storm::models::Mdp<Type> const& model) : SparseMdpPrctlModelChecker<Type>(model) {
		// Intentionally left empty.
	}

	/*!
	 * Copy constructs a GmmxxDtmcPrctlModelChecker from the given model checker. In particular, this means that the newly
	 * constructed model checker will have the model of the given model checker as its associated model.
	 */
	explicit GmmxxMdpPrctlModelChecker(storm::modelchecker::GmmxxMdpPrctlModelChecker<Type> const& modelchecker) : SparseMdpPrctlModelChecker<Type>(modelchecker) {
		// Intentionally left empty.
	}

	/*!
	 * Virtual destructor. Needs to be virtual, because this class has virtual methods.
	 */
	virtual ~GmmxxMdpPrctlModelChecker() {
		// Intentionally left empty.
	}

private:

	virtual void performMatrixVectorMultiplication(storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type>* b = nullptr, uint_fast64_t n = 1) const {
		// Get the starting row indices for the non-deterministic choices to reduce the resulting
		// vector properly.
		std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = *this->getModel().getNondeterministicChoiceIndices();


		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(A);

		// Create vector for result of multiplication, which is reduced to the result vector after
		// each multiplication.
		std::vector<Type> multiplyResult(A.getRowCount());

		// Now perform matrix-vector multiplication as long as we meet the bound of the formula.
		for (uint_fast64_t i = 0; i < n; ++i) {
			gmm::mult(*gmmxxMatrix, x, multiplyResult);
			if (b != nullptr) {
				gmm::add(*b, multiplyResult);
			}
			if (this->minimumOperatorStack.top()) {
				storm::utility::reduceVectorMin(multiplyResult, &x, nondeterministicChoiceIndices);
			} else {
				storm::utility::reduceVectorMax(multiplyResult, &x, nondeterministicChoiceIndices);
			}
		}

		// Delete intermediate results and return result.
		delete gmmxxMatrix;
	}

	/*!
	 * Solves the given equation system under the given parameters using the power method.
	 *
	 * @param A The matrix A specifying the coefficients of the equations.
	 * @param x The vector x for which to solve the equations. The initial value of the elements of
	 * this vector are used as the initial guess and might thus influence performance and convergence.
	 * @param b The vector b specifying the values on the right-hand-sides of the equations.
	 * @return The solution of the system of linear equations in form of the elements of the vector
	 * x.
	 */
	void solveEquationSystem(storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices) const {
		// Get the settings object to customize solving.
		storm::settings::Settings* s = storm::settings::instance();

		// Get relevant user-defined settings for solving the equations.
		double precision = s->get<double>("precision");
		unsigned maxIterations = s->get<unsigned>("maxiter");
		bool relative = s->get<bool>("relative");


		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(A);

		// Set up the environment for the power method.
		std::vector<Type> multiplyResult(A.getRowCount());
		std::vector<Type>* currentX = &x;
		std::vector<Type>* newX = new std::vector<Type>(x.size());
		std::vector<Type>* swap = nullptr;
		uint_fast64_t iterations = 0;
		bool converged = false;

		// Proceed with the iterations as long as the method did not converge or reach the user-specified maximum number
		// of iterations.
		while (!converged && iterations < maxIterations) {
			// Compute x' = A*x + b.
			gmm::mult(*gmmxxMatrix, *currentX, multiplyResult);
			gmm::add(b, multiplyResult);

			// Reduce the vector x' by applying min/max for all non-deterministic choices.
			if (this->minimumOperatorStack.top()) {
				storm::utility::reduceVectorMin(multiplyResult, newX, nondeterministicChoiceIndices);
			} else {
				storm::utility::reduceVectorMax(multiplyResult, newX, nondeterministicChoiceIndices);
			}

			// Determine whether the method converged.
			converged = storm::utility::equalModuloPrecision(*currentX, *newX, precision, relative);

			// Update environment variables.
			swap = currentX;
			currentX = newX;
			newX = swap;
			++iterations;
		}

		// If we performed an odd number of iterations, we need to swap the x and currentX, because the newest result
		// is currently stored in currentX, but x is the output vector.
		if (iterations % 2 == 1) {
			std::swap(x, *currentX);
			delete currentX;
		} else {
			delete newX;
		}
		delete gmmxxMatrix;

		// Check if the solver converged and issue a warning otherwise.
		if (converged) {
			LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iterations << " iterations.");
		} else {
			LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
		}
	}
};

} //namespace modelchecker
} //namespace storm

#endif /* STORM_MODELCHECKER_GMMXXMDPPRCTLMODELCHECKER_H_ */
