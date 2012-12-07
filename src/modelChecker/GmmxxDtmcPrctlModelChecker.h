/*
 * GmmxxDtmcPrctlModelChecker.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef GMMXXDTMCPRCTLMODELCHECKER_H_
#define GMMXXDTMCPRCTLMODELCHECKER_H_

#include "src/utility/vector.h"

#include "src/models/Dtmc.h"
#include "src/modelChecker/DtmcPrctlModelChecker.h"
#include "src/solver/GraphAnalyzer.h"

#include "gmm/gmm_matrix.h"
#include "gmm/gmm_iter_solvers.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace mrmc {

namespace modelChecker {

/*
 * A model checking engine that makes use of the gmm++ backend.
 */
template <class Type>
class GmmxxDtmcPrctlModelChecker : public DtmcPrctlModelChecker<Type> {

public:
	explicit GmmxxDtmcPrctlModelChecker(mrmc::models::Dtmc<Type>& dtmc) : DtmcPrctlModelChecker<Type>(dtmc) { }

	virtual ~GmmxxDtmcPrctlModelChecker() { }

	virtual mrmc::storage::BitVector* checkProbabilisticOperator(const mrmc::formula::ProbabilisticOperator<Type>& formula) const {
		std::vector<Type>* probabilisticResult = this->checkPathFormula(formula.getPathFormula());

		mrmc::storage::BitVector* result = new mrmc::storage::BitVector(this->getModel().getNumberOfStates());
		Type lower = formula.getLowerBound();
		Type upper = formula.getUpperBound();
		for (uint_fast64_t i = 0; i < this->getModel().getNumberOfStates(); ++i) {
			if ((*probabilisticResult)[i] >= lower && (*probabilisticResult)[i] <= upper) result->set(i, true);
		}

		delete probabilisticResult;

		return result;
	}

	virtual std::vector<Type>* checkBoundedUntil(const mrmc::formula::BoundedUntil<Type>& formula) const {
		// First, we need to compute the states that satisfy the sub-formulas of the until-formula.
		mrmc::storage::BitVector* leftStates = this->checkStateFormula(formula.getLeft());
		mrmc::storage::BitVector* rightStates = this->checkStateFormula(formula.getRight());

		// Copy the matrix before we make any changes.
		mrmc::storage::SquareSparseMatrix<Type> tmpMatrix(*this->getModel().getTransitionProbabilityMatrix());

		// Make all rows absorbing that violate both sub-formulas or satisfy the second sub-formula.
		tmpMatrix.makeRowsAbsorbing((~*leftStates & *rightStates) | *rightStates);

		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<double>* gmmxxMatrix = tmpMatrix.toGMMXXSparseMatrix();

		// Create the vector with which to multiply.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());
		mrmc::utility::setVectorValues(result, *rightStates, static_cast<double>(1.0));

		// Now perform matrix-vector multiplication as long as we meet the bound of the formula.
		for (uint_fast64_t i = 0; i < formula.getBound(); ++i) {
			gmm::mult(*gmmxxMatrix, *result, *result);
		}

		// Delete intermediate results.
		delete leftStates;
		delete rightStates;

		return result;
	}

	virtual std::vector<Type>* checkNext(const mrmc::formula::Next<Type>& formula) const {
		// First, we need to compute the states that satisfy the sub-formula of the next-formula.
		mrmc::storage::BitVector* nextStates = this->checkStateFormula(formula.getChild());

		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<double>* gmmxxMatrix = this->getModel().getTransitionProbabilityMatrix()->toGMMXXSparseMatrix();

		// Create the vector with which to multiply and initialize it correctly.
		std::vector<Type> x(this->getModel().getNumberOfStates());
		mrmc::utility::setVectorValues(&x, *nextStates, static_cast<double>(1.0));

		// Delete not needed next states bit vector.
		delete nextStates;

		// Create resulting vector.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());

		// Perform the actual computation.
		gmm::mult(*gmmxxMatrix, x, *result);

		// Delete temporary matrix and return result.
		delete gmmxxMatrix;
		return result;
	}

	virtual std::vector<Type>* checkUntil(const mrmc::formula::Until<Type>& formula) const {
		// First, we need to compute the states that satisfy the sub-formulas of the until-formula.
		mrmc::storage::BitVector* leftStates = this->checkStateFormula(formula.getLeft());
		mrmc::storage::BitVector* rightStates = this->checkStateFormula(formula.getRight());

		// Then, we need to identify the states which have to be taken out of the matrix, i.e.
		// all states that have probability 0 and 1 of satisfying the until-formula.
		mrmc::storage::BitVector notExistsPhiUntilPsiStates(this->getModel().getNumberOfStates());
		mrmc::storage::BitVector alwaysPhiUntilPsiStates(this->getModel().getNumberOfStates());
		mrmc::solver::GraphAnalyzer::getPhiUntilPsiStates<double>(this->getModel(), *leftStates, *rightStates, &notExistsPhiUntilPsiStates, &alwaysPhiUntilPsiStates);
		notExistsPhiUntilPsiStates.complement();

		delete leftStates;
		delete rightStates;

		LOG4CPLUS_INFO(logger, "Found " << notExistsPhiUntilPsiStates.getNumberOfSetBits() << " 'no' states.");
		LOG4CPLUS_INFO(logger, "Found " << alwaysPhiUntilPsiStates.getNumberOfSetBits() << " 'yes' states.");
		mrmc::storage::BitVector maybeStates = ~(notExistsPhiUntilPsiStates | alwaysPhiUntilPsiStates);
		LOG4CPLUS_INFO(logger, "Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");

		// Create resulting vector and set values accordingly.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());

		// Only try to solve system if there are states for which the probability is unknown.
		if (maybeStates.getNumberOfSetBits() > 0) {
			// Now we can eliminate the rows and columns from the original transition probability matrix.
			mrmc::storage::SquareSparseMatrix<double>* submatrix = this->getModel().getTransitionProbabilityMatrix()->getSubmatrix(maybeStates);
			// Converting the matrix to the form needed for the equation system. That is, we go from
			// x = A*x + b to (I-A)x = b.
			submatrix->convertToEquationSystem();

			// Transform the submatrix to the gmm++ format to use its solvers.
			gmm::csr_matrix<double>* gmmxxMatrix = submatrix->toGMMXXSparseMatrix();

			// Initialize the x vector with 0.5 for each element. This is the initial guess for
			// the iterative solvers. It should be safe as for all 'maybe' states we know that the
			// probability is strictly larger than 0.
			std::vector<Type> x(maybeStates.getNumberOfSetBits(), Type(0.5));

			// Prepare the right-hand side of the equation system. For entry i this corresponds to
			// the accumulated probability of going from state i to some 'yes' state.
			std::vector<double> b(maybeStates.getNumberOfSetBits());
			this->getModel().getTransitionProbabilityMatrix()->getConstrainedRowCountVector(maybeStates, alwaysPhiUntilPsiStates, &x);

			// Set up the precondition of the iterative solver.
			gmm::ilu_precond<gmm::csr_matrix<double>> P(*gmmxxMatrix);
			// Prepare an iteration object that determines the accuracy, maximum number of iterations
			// and the like.
			gmm::iteration iter(0.000001);

			// Now do the actual solving.
			LOG4CPLUS_INFO(logger, "Starting iterative solver.");
			gmm::bicgstab(*gmmxxMatrix, x, b, P, iter);
			LOG4CPLUS_INFO(logger, "Iterative solver converged.");

			// Set values of resulting vector according to result.
			mrmc::utility::setVectorValues<Type>(result, maybeStates, x);

			// Delete temporary matrix.
			delete gmmxxMatrix;
		}

		mrmc::utility::setVectorValues<Type>(result, notExistsPhiUntilPsiStates, static_cast<double>(0));
		mrmc::utility::setVectorValues<Type>(result, alwaysPhiUntilPsiStates, static_cast<double>(1.0));

		return result;
	}
};

} //namespace modelChecker

} //namespace mrmc

#endif /* GMMXXDTMCPRCTLMODELCHECKER_H_ */
