/*
 * GmmxxDtmcPrctlModelChecker.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef GMMXXDTMCPRCTLMODELCHECKER_H_
#define GMMXXDTMCPRCTLMODELCHECKER_H_

#include <cmath>

#include "src/models/Dtmc.h"
#include "src/modelChecker/DtmcPrctlModelChecker.h"
#include "src/solver/GraphAnalyzer.h"
#include "src/utility/vector.h"

#include "src/utility/settings.h"

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

	virtual std::vector<Type>* checkBoundedUntil(const mrmc::formula::BoundedUntil<Type>& formula) const {
		// First, we need to compute the states that satisfy the sub-formulas of the until-formula.
		mrmc::storage::BitVector* leftStates = this->checkStateFormula(formula.getLeft());
		mrmc::storage::BitVector* rightStates = this->checkStateFormula(formula.getRight());

		// Copy the matrix before we make any changes.
		mrmc::storage::SquareSparseMatrix<Type> tmpMatrix(*this->getModel().getTransitionProbabilityMatrix());

		// Make all rows absorbing that violate both sub-formulas or satisfy the second sub-formula.
		tmpMatrix.makeRowsAbsorbing(~(*leftStates & *rightStates) | *rightStates);

		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<Type>* gmmxxMatrix = tmpMatrix.toGMMXXSparseMatrix();

		// Create the vector with which to multiply.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());
		mrmc::utility::setVectorValues(result, *rightStates, static_cast<Type>(1.0));

		// Now perform matrix-vector multiplication as long as we meet the bound of the formula.
		for (uint_fast64_t i = 0; i < formula.getBound(); ++i) {
			gmm::mult(*gmmxxMatrix, *result, *result);
		}

		// Delete intermediate results and return result.
		delete leftStates;
		delete rightStates;
		return result;
	}

	virtual std::vector<Type>* checkNext(const mrmc::formula::Next<Type>& formula) const {
		// First, we need to compute the states that satisfy the sub-formula of the next-formula.
		mrmc::storage::BitVector* nextStates = this->checkStateFormula(formula.getChild());

		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<Type>* gmmxxMatrix = this->getModel().getTransitionProbabilityMatrix()->toGMMXXSparseMatrix();

		// Create the vector with which to multiply and initialize it correctly.
		std::vector<Type> x(this->getModel().getNumberOfStates());
		mrmc::utility::setVectorValues(&x, *nextStates, static_cast<Type>(1.0));

		// Delete obsolete sub-result.
		delete nextStates;

		// Create resulting vector.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());

		// Perform the actual computation, namely matrix-vector multiplication.
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
		mrmc::solver::GraphAnalyzer::getPhiUntilPsiStates(this->getModel(), *leftStates, *rightStates, &notExistsPhiUntilPsiStates, &alwaysPhiUntilPsiStates);
		notExistsPhiUntilPsiStates.complement();

		// Delete sub-results that are obsolete now.
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
			mrmc::storage::SquareSparseMatrix<Type>* submatrix = this->getModel().getTransitionProbabilityMatrix()->getSubmatrix(maybeStates);
			// Converting the matrix from the fixpoint notation to the form needed for the equation
			// system. That is, we go from x = A*x + b to (I-A)x = b.
			submatrix->convertToEquationSystem();

			// Transform the submatrix to the gmm++ format to use its solvers.
			gmm::csr_matrix<Type>* gmmxxMatrix = submatrix->toGMMXXSparseMatrix();
			delete submatrix;

			// Initialize the x vector with 0.5 for each element. This is the initial guess for
			// the iterative solvers. It should be safe as for all 'maybe' states we know that the
			// probability is strictly larger than 0.
			std::vector<Type> x(maybeStates.getNumberOfSetBits(), Type(0.5));

			// Prepare the right-hand side of the equation system. For entry i this corresponds to
			// the accumulated probability of going from state i to some 'yes' state.
			std::vector<Type> b(maybeStates.getNumberOfSetBits());
			this->getModel().getTransitionProbabilityMatrix()->getConstrainedRowCountVector(maybeStates, alwaysPhiUntilPsiStates, &b);

			// Get the settings object to customize linear solving.
			mrmc::settings::Settings* s = mrmc::settings::instance();

			// Prepare an iteration object that determines the accuracy, maximum number of iterations
			// and the like.
			gmm::iteration iter(s->get<double>("precision"), 0, s->get<unsigned>("lemaxiter"));

			// Now do the actual solving.
			LOG4CPLUS_INFO(logger, "Starting iterative solver.");
			const std::string& precond = s->getString("precond");
			if (s->getString("lemethod").compare("bicgstab") == 0) {
				if (precond.compare("ilu")) {
					gmm::bicgstab(*gmmxxMatrix, x, b, gmm::ilu_precond<gmm::csr_matrix<Type>>(*gmmxxMatrix), iter);
				} else if (precond.compare("diagonal") == 0) {
					gmm::bicgstab(*gmmxxMatrix, x, b, gmm::diagonal_precond<gmm::csr_matrix<Type>>(*gmmxxMatrix), iter);
				} else if (precond.compare("ildlt") == 0) {
					gmm::bicgstab(*gmmxxMatrix, x, b, gmm::ildlt_precond<gmm::csr_matrix<Type>>(*gmmxxMatrix), iter);
				} else if (precond.compare("none") == 0) {
					gmm::bicgstab(*gmmxxMatrix, x, b, gmm::identity_matrix(), iter);
				}
			// FIXME: gmres has been disabled, because it triggers gmm++ compilation errors
			/* } else if (s->getString("lemethod").compare("gmres") == 0) {
				if (precond.compare("ilu")) {
					gmm::gmres(*gmmxxMatrix, x, b, gmm::ilu_precond<gmm::csr_matrix<Type>>(*gmmxxMatrix), s->get<unsigned>("restart"), iter);
				} else if (precond.compare("diagonal") == 0) {
					gmm::gmres(*gmmxxMatrix, x, b, gmm::diagonal_precond<gmm::csr_matrix<Type>>(*gmmxxMatrix), s->get<unsigned>("restart"), iter);
				} else if (precond.compare("ildlt") == 0) {
					gmm::gmres(*gmmxxMatrix, x, b, gmm::ildlt_precond<gmm::csr_matrix<Type>>(*gmmxxMatrix), s->get<unsigned>("restart"), iter);
				} else if (precond.compare("none") == 0) {
					gmm::gmres(*gmmxxMatrix, x, b, gmm::identity_matrix(), s->get<unsigned>("restart"), iter);
				} */
			} else if (s->getString("lemethod").compare("qmr") == 0) {
				if (precond.compare("ilu")) {
					gmm::qmr(*gmmxxMatrix, x, b, gmm::ilu_precond<gmm::csr_matrix<Type>>(*gmmxxMatrix), iter);
				} else if (precond.compare("diagonal") == 0) {
					gmm::qmr(*gmmxxMatrix, x, b, gmm::diagonal_precond<gmm::csr_matrix<Type>>(*gmmxxMatrix), iter);
				} else if (precond.compare("ildlt") == 0) {
					gmm::qmr(*gmmxxMatrix, x, b, gmm::ildlt_precond<gmm::csr_matrix<Type>>(*gmmxxMatrix), iter);
				} else if (precond.compare("none") == 0) {
					gmm::qmr(*gmmxxMatrix, x, b, gmm::identity_matrix(), iter);
				}
			}

			// Check if the solver converged and issue a warning otherwise.
			if (iter.converged()) {
				LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iter.get_iteration() << " iterations.");
			} else {
				LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
			}

			// Set values of resulting vector according to result.
			mrmc::utility::setVectorValues<Type>(result, maybeStates, x);

			// Delete temporary matrix.
			delete gmmxxMatrix;
		}

		// Set values of resulting vector that are known exactly.
		mrmc::utility::setVectorValues<Type>(result, notExistsPhiUntilPsiStates, static_cast<Type>(0));
		mrmc::utility::setVectorValues<Type>(result, alwaysPhiUntilPsiStates, static_cast<Type>(1.0));

		return result;
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
		desc->add_options()("lemethod", boost::program_options::value<std::string>()->default_value("bicgstab")->notifier(&validateLeMethod), "Sets the method used for linear equation solving. Must be in {bicgstab, qmr}.");
		desc->add_options()("lemaxiter", boost::program_options::value<unsigned>()->default_value(10000), "Sets the maximal number of iterations for iterative linear equation solving.");
		desc->add_options()("precision", boost::program_options::value<double>()->default_value(10e-6), "Sets the precision for iterative linear equation solving.");
		desc->add_options()("precond", boost::program_options::value<std::string>()->default_value("ilu")->notifier(&validatePreconditioner), "Sets the preconditioning technique for linear equation solving. Must be in {ilu, diagonal, ildlt, none}.");
	}

	/*!
	 * Validates whether the given lemethod matches one of the available ones.
	 * Throws an exception of type InvalidSettings in case the selected method is illegal.
	 */
	static void validateLeMethod(const std::string& lemethod) {
		if (lemethod.compare("bicgstab") != 0 && lemethod.compare("qmr") != 0) {
			throw exceptions::InvalidSettings() << "Argument " << lemethod << " for option 'lemethod' is invalid.";
		}
	}

	/*!
	 * Validates whether the given preconditioner matches one of the available ones.
	 * Throws an exception of type InvalidSettings in case the selected preconditioner is illegal.
	 */
	static void validatePreconditioner(const std::string& preconditioner) {
		if (preconditioner.compare("ilu") != 0 && preconditioner.compare("diagonal") != 0 && preconditioner.compare("ildlt") && preconditioner.compare("none") != 0) {
			throw exceptions::InvalidSettings() << "Argument " << preconditioner << " for option 'precond' is invalid.";
		}
	}
};

} //namespace modelChecker

} //namespace mrmc

#endif /* GMMXXDTMCPRCTLMODELCHECKER_H_ */
