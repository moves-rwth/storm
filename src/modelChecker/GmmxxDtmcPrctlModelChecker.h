/*
 * GmmxxDtmcPrctlModelChecker.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef GMMXXDTMCPRCTLMODELCHECKER_H_
#define GMMXXDTMCPRCTLMODELCHECKER_H_

#include "src/utility/vector.h"

#include "src/models/dtmc.h"
#include "src/solver/GraphAnalyzer.h"

#include "gmm/gmm_matrix.h"
#include "gmm/gmm_iter_solvers.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

log4cplus::Logger logger;

namespace mrmc {

namespace modelChecker {

/*
 * A model checking engine that makes use of the gmm++ backend.
 */
template <class T>
class GmmxxDtmcPrctlModelChecker : public DtmcPrctlModelChecker<T> {

public:
	explicit GmmxxDtmcPrctlModelChecker(const mrmc::models::Dtmc<T>* dtmc) : DtmcPrctlModelChecker(dtmc) { }

	virtual ~GmmxxDtmcPrctlModelChecker() { }

	virtual std::vector<T>* checkBoundedUntil(mrmc::formula::BoundedUntil<T>& formula) {
		// First, we need to compute the states that satisfy the sub-formulas of the until-formula.
		mrmc::storage::BitVector* leftStates = this->check(formula.getLeft());
		mrmc::storage::BitVector* rightStates = this->check(formula.getRight());

		// Copy the matrix before we make any changes.
		mrmc::storage::SquareSparseMatrix<T>* tmpMatrix(dtmc.getTransitionProbabilityMatrix());

		// Make all rows absorbing that violate both sub-formulas or satisfy the second sub-formula.
		tmpMatrix.makeRowsAbsorbing((~leftStates & rightStates) | rightStates);

		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<double>* gmmxxMatrix = tmpMatrix.toGMMXXSparseMatrix();

		// Create the vector with which to multiply.
		std::vector<T>* result = new st::vector<T>(dtmc.getNumberOfStates());
		mrmc::utility::setVectorValue(result, *rightStates, 1);

		// Now perform matrix-vector multiplication as long as we meet the bound of the formula.
		for (uint_fast64_t i = 0; i < formula.getBound(); ++i) {
			gmm::mult(*gmmxxMatrix, *result, *result);
		}

		// Delete intermediate results.
		delete leftStates;
		delete rightStates;

		return result;
	}

	virtual std::vector<T>* checkNext(const mrmc::formula::Next<T>& formula) {
		// First, we need to compute the states that satisfy the sub-formula of the next-formula.
		mrmc::storage::BitVector* nextStates = this->check(formula.getChild());

		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<double>* gmmxxMatrix = dtmc.getTransitionProbabilityMatrix()->toGMMXXSparseMatrix();

		// Create the vector with which to multiply and initialize it correctly.
		std::vector<T> x(dtmc.getNumberOfStates());
		mrmc::utility::setVectorValue(x, nextStates, 1);

		// Delete not needed next states bit vector.
		delete nextStates;

		// Create resulting vector.
		std::vector<T>* result = new std::vector<T>(dtmc.getNumberOfStates());

		// Perform the actual computation.
		gmm::mult(*gmmxxMatrix, x, *result);

		// Delete temporary matrix and return result.
		delete gmmxxMatrix;
		return result;
	}

	virtual std::vector<T>* checkUntil(const mrmc::formula::Until<T>& formula) {
		// First, we need to compute the states that satisfy the sub-formulas of the until-formula.
		mrmc::storage::BitVector* leftStates = this->check(formula.getLeft());
		mrmc::storage::BitVector* rightStates = this->check(formula.getRight());

		// Then, we need to identify the states which have to be taken out of the matrix, i.e.
		// all states that have probability 0 and 1 of satisfying the until-formula.
		mrmc::storage::BitVector notExistsPhiUntilPsiStates(dtmc.getNumberOfStates());
		mrmc::storage::BitVector alwaysPhiUntilPsiStates(dtmc.getNumberOfStates());
		mrmc::solver::GraphAnalyzer::getPhiUntilPsiStates<double>(dtmc, *leftStates, *rightStates, &notExistsPhiUntilPsiStates, &alwaysPhiUntilPsiStates);
		notExistsPhiUntilPsiStates->complement();

		delete leftStates;
		delete rightStates;

		LOG4CPLUS_INFO(logger, "Found " << notExistsPhiUntilPsiStates.getNumberOfSetBits() << " 'no' states.");
		LOG4CPLUS_INFO(logger, "Found " << alwaysPhiUntilPsiStates.getNumberOfSetBits() << " 'yes' states.");
		mrmc::storage::BitVector maybeStates = ~(notExistsPhiUntilPsiStates | alwaysPhiUntilPsiStates);
		LOG4CPLUS_INFO(logger, "Found " << maybeStates.getNumberOfSetBits() << " maybe states.");

		// Only try to solve system if there are states for which the probability is unknown.
		if (maybeStates.getNumberOfSetBits() > 0) {
			// Now we can eliminate the rows and columns from the original transition probability matrix.
			mrmc::storage::SquareSparseMatrix<double>* submatrix = dtmc.getTransitionProbabilityMatrix()->getSubmatrix(maybeStates);
			// Converting the matrix to the form needed for the equation system. That is, we go from
			// x = A*x + b to (I-A)x = b.
			submatrix->convertToEquationSystem();

			// Transform the submatrix to the gmm++ format to use its solvers.
			gmm::csr_matrix<double>* gmmxxMatrix = submatrix->toGMMXXSparseMatrix();

			// Initialize the x vector with 0.5 for each element. This is the initial guess for
			// the iterative solvers. It should be safe as for all 'maybe' states we know that the
			// probability is strictly larger than 0.
			std::vector<T>* x = new std::vector<T>(maybeStates.getNumberOfSetBits(), 0.5);

			// Prepare the right-hand side of the equation system. For entry i this corresponds to
			// the accumulated probability of going from state i to some 'yes' state.
			std::vector<double> b(maybeStates.getNumberOfSetBits());
			dtmc.getTransitionProbabilityMatrix()->getConstrainedRowCountVector(maybeStates, alwaysPhiUntilPsiStates, &x);

			// Set up the precondition of the iterative solver.
			gmm::ilu_precond<gmm::csr_matrix<double>> P(*gmmxxMatrix);
			// Prepare an iteration object that determines the accuracy, maximum number of iterations
			// and the like.
			gmm::iteration iter(0.000001);

			// Now do the actual solving.
			LOG4CPLUS_INFO(logger, "Starting iterations...");
			gmm::bicgstab(*gmmxxMatrix, x, b, P, iter);
			LOG4CPLUS_INFO(logger, "Done with iterations.");

			// Create resulting vector and set values accordingly.
			std::vector<T>* result = new std::vector<T>(dtmc.getNumberOfStates());
			mrmc::utility::setVectorValues<std::vector<T>>(result, maybeStates, x);

			// Delete temporary matrix and return result.
			delete x;
			delete gmmxxMatrix;
		}

		mrmc::utility::setVectorValue<std::vector<T>>(result, notExistsPhiUntilPsiStates, 0);
		mrmc::utility::setVectorValue<std::vector<T>>(result, alwaysPhiUntilPsiStates, 1);

		return result;
	}
};

#endif /* GMMXXDTMCPRCTLMODELCHECKER_H_ */
