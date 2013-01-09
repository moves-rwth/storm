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
#include "src/modelChecker/DtmcPrctlModelChecker.h"
#include "src/solver/GraphAnalyzer.h"
#include "src/utility/Vector.h"
#include "src/utility/ConstTemplates.h"
#include "src/utility/Settings.h"
#include "src/adapters/GmmxxAdapter.h"
#include "src/exceptions/InvalidPropertyException.h"

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
	explicit GmmxxDtmcPrctlModelChecker(storm::models::Dtmc<Type>& dtmc) : DtmcPrctlModelChecker<Type>(dtmc) { }

	virtual ~GmmxxDtmcPrctlModelChecker() { }

	virtual std::vector<Type>* checkBoundedUntil(const storm::formula::BoundedUntil<Type>& formula) const {
		// First, we need to compute the states that satisfy the sub-formulas of the until-formula.
		storm::storage::BitVector* leftStates = this->checkStateFormula(formula.getLeft());
		storm::storage::BitVector* rightStates = this->checkStateFormula(formula.getRight());

		// Copy the matrix before we make any changes.
		storm::storage::SparseMatrix<Type> tmpMatrix(*this->getModel().getTransitionProbabilityMatrix());

		// Make all rows absorbing that violate both sub-formulas or satisfy the second sub-formula.
		tmpMatrix.makeRowsAbsorbing(~(*leftStates | *rightStates) | *rightStates);

		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(tmpMatrix);

		// Create the vector with which to multiply.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());
		storm::utility::setVectorValues(result, *rightStates, storm::utility::constGetOne<Type>());

		// Now perform matrix-vector multiplication as long as we meet the bound of the formula.
		std::vector<Type>* swap = nullptr;
		std::vector<Type>* tmpResult = new std::vector<Type>(this->getModel().getNumberOfStates());
		for (uint_fast64_t i = 0; i < formula.getBound(); ++i) {
			gmm::mult(*gmmxxMatrix, *result, *tmpResult);
			swap = tmpResult;
			tmpResult = result;
			result = swap;
		}
		delete tmpResult;

		// Delete intermediate results and return result.
		delete gmmxxMatrix;
		delete leftStates;
		delete rightStates;
		return result;
	}

	virtual std::vector<Type>* checkNext(const storm::formula::Next<Type>& formula) const {
		// First, we need to compute the states that satisfy the sub-formula of the next-formula.
		storm::storage::BitVector* nextStates = this->checkStateFormula(formula.getChild());

		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(*this->getModel().getTransitionProbabilityMatrix());

		// Create the vector with which to multiply and initialize it correctly.
		std::vector<Type> x(this->getModel().getNumberOfStates());
		storm::utility::setVectorValues(&x, *nextStates, storm::utility::constGetOne<Type>());

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

	virtual std::vector<Type>* checkUntil(const storm::formula::Until<Type>& formula) const {
		// First, we need to compute the states that satisfy the sub-formulas of the until-formula.
		storm::storage::BitVector* leftStates = this->checkStateFormula(formula.getLeft());
		storm::storage::BitVector* rightStates = this->checkStateFormula(formula.getRight());

		// Then, we need to identify the states which have to be taken out of the matrix, i.e.
		// all states that have probability 0 and 1 of satisfying the until-formula.
		storm::storage::BitVector notExistsPhiUntilPsiStates(this->getModel().getNumberOfStates());
		storm::storage::BitVector alwaysPhiUntilPsiStates(this->getModel().getNumberOfStates());
		storm::solver::GraphAnalyzer::getPhiUntilPsiStates(this->getModel(), *leftStates, *rightStates, &notExistsPhiUntilPsiStates, &alwaysPhiUntilPsiStates);
		notExistsPhiUntilPsiStates.complement();

		// Delete sub-results that are obsolete now.
		delete leftStates;
		delete rightStates;

		LOG4CPLUS_INFO(logger, "Found " << notExistsPhiUntilPsiStates.getNumberOfSetBits() << " 'no' states.");
		LOG4CPLUS_INFO(logger, "Found " << alwaysPhiUntilPsiStates.getNumberOfSetBits() << " 'yes' states.");
		storm::storage::BitVector maybeStates = ~(notExistsPhiUntilPsiStates | alwaysPhiUntilPsiStates);
		LOG4CPLUS_INFO(logger, "Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");

		// Create resulting vector.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());

		// Only try to solve system if there are states for which the probability is unknown.
		if (maybeStates.getNumberOfSetBits() > 0) {
			// Now we can eliminate the rows and columns from the original transition probability matrix.
			storm::storage::SparseMatrix<Type>* submatrix = this->getModel().getTransitionProbabilityMatrix()->getSubmatrix(maybeStates);
			// Converting the matrix from the fixpoint notation to the form needed for the equation
			// system. That is, we go from x = A*x + b to (I-A)x = b.
			submatrix->convertToEquationSystem();

			// Transform the submatrix to the gmm++ format to use its solvers.
			gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(*submatrix);
			delete submatrix;

			// Initialize the x vector with 0.5 for each element. This is the initial guess for
			// the iterative solvers. It should be safe as for all 'maybe' states we know that the
			// probability is strictly larger than 0.
			std::vector<Type> x(maybeStates.getNumberOfSetBits(), Type(0.5));

			// Prepare the right-hand side of the equation system. For entry i this corresponds to
			// the accumulated probability of going from state i to some 'yes' state.
			std::vector<Type> b(maybeStates.getNumberOfSetBits());
			this->getModel().getTransitionProbabilityMatrix()->getConstrainedRowCountVector(maybeStates, alwaysPhiUntilPsiStates, &b);

			// Solve the corresponding system of linear equations.
			this->solveLinearEquationSystem(*gmmxxMatrix, x, b);

			// Set values of resulting vector according to result.
			storm::utility::setVectorValues<Type>(result, maybeStates, x);

			// Delete temporary matrix.
			delete gmmxxMatrix;
		}

		// Set values of resulting vector that are known exactly.
		storm::utility::setVectorValues<Type>(result, notExistsPhiUntilPsiStates, storm::utility::constGetZero<Type>());
		storm::utility::setVectorValues<Type>(result, alwaysPhiUntilPsiStates, storm::utility::constGetOne<Type>());

		return result;
	}

	virtual std::vector<Type>* checkInstantaneousReward(const storm::formula::InstantaneousReward<Type>& formula) const {
		// Only compute the result if the model has a state-based reward model.
		if (!this->getModel().hasStateRewards()) {
			LOG4CPLUS_ERROR(logger, "Missing (state-based) reward model for formula.");
			throw storm::exceptions::InvalidPropertyException() << "Missing (state-based) reward model for formula.";
		}

		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(*this->getModel().getTransitionProbabilityMatrix());

		// Initialize result to state rewards of the model.
		std::vector<Type>* result = new std::vector<Type>(*this->getModel().getStateRewards());

		// Now perform matrix-vector multiplication as long as we meet the bound of the formula.
		std::vector<Type>* swap = nullptr;
		std::vector<Type>* tmpResult = new std::vector<Type>(this->getModel().getNumberOfStates());
		for (uint_fast64_t i = 0; i < formula.getBound(); ++i) {
			gmm::mult(*gmmxxMatrix, *result, *tmpResult);
			swap = tmpResult;
			tmpResult = result;
			result = swap;
		}

		// Delete temporary variables and return result.
		delete tmpResult;
		delete gmmxxMatrix;
		return result;
	}

	virtual std::vector<Type>* checkCumulativeReward(const storm::formula::CumulativeReward<Type>& formula) const {
		// Only compute the result if the model has at least one reward model.
		if (!this->getModel().hasStateRewards() && !this->getModel().hasTransitionRewards()) {
			LOG4CPLUS_ERROR(logger, "Missing reward model for formula.");
			throw storm::exceptions::InvalidPropertyException() << "Missing reward model for formula.";
		}

		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(*this->getModel().getTransitionProbabilityMatrix());

		// Compute the reward vector to add in each step based on the available reward models.
		std::vector<Type>* totalRewardVector = nullptr;
		if (this->getModel().hasTransitionRewards()) {
			totalRewardVector = this->getModel().getTransitionProbabilityMatrix()->getPointwiseProductRowSumVector(*this->getModel().getTransitionRewardMatrix());
			if (this->getModel().hasStateRewards()) {
				gmm::add(*this->getModel().getStateRewards(), *totalRewardVector);
			}
		} else {
			totalRewardVector = new std::vector<Type>(*this->getModel().getStateRewards());
		}

		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());

		// Now perform matrix-vector multiplication as long as we meet the bound of the formula.
		std::vector<Type>* swap = nullptr;
		std::vector<Type>* tmpResult = new std::vector<Type>(this->getModel().getNumberOfStates());
		for (uint_fast64_t i = 0; i < formula.getBound(); ++i) {
			gmm::mult(*gmmxxMatrix, *result, *tmpResult);
			swap = tmpResult;
			tmpResult = result;
			result = swap;

			// Add the reward vector to the result.
			gmm::add(*totalRewardVector, *result);
		}

		// Delete temporary variables and return result.
		delete tmpResult;
		delete gmmxxMatrix;
		delete totalRewardVector;
		return result;
	}

	virtual std::vector<Type>* checkReachabilityReward(const storm::formula::ReachabilityReward<Type>& formula) const {
		// Only compute the result if the model has at least one reward model.
		if (!this->getModel().hasStateRewards() && !this->getModel().hasTransitionRewards()) {
			LOG4CPLUS_ERROR(logger, "Missing reward model for formula. Skipping formula");
			throw storm::exceptions::InvalidPropertyException() << "Missing reward model for formula.";
		}

		// Determine the states for which the target predicate holds.
		storm::storage::BitVector* targetStates = this->checkStateFormula(formula.getChild());

		// Determine which states have a reward of infinity by definition.
		storm::storage::BitVector infinityStates(this->getModel().getNumberOfStates());
		storm::storage::BitVector trueStates(this->getModel().getNumberOfStates(), true);
		storm::solver::GraphAnalyzer::getAlwaysPhiUntilPsiStates(this->getModel(), trueStates, *targetStates, &infinityStates);
		infinityStates.complement();

		// Create resulting vector.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());

		// Check whether there are states for which we have to compute the result.
		storm::storage::BitVector maybeStates = ~(*targetStates) & ~infinityStates;
		if (maybeStates.getNumberOfSetBits() > 0) {
			// Now we can eliminate the rows and columns from the original transition probability matrix.
			storm::storage::SparseMatrix<Type>* submatrix = this->getModel().getTransitionProbabilityMatrix()->getSubmatrix(maybeStates);
			// Converting the matrix from the fixpoint notation to the form needed for the equation
			// system. That is, we go from x = A*x + b to (I-A)x = b.
			submatrix->convertToEquationSystem();

			// Transform the submatrix to the gmm++ format to use its solvers.
			gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(*submatrix);
			delete submatrix;

			// Initialize the x vector with 1 for each element. This is the initial guess for
			// the iterative solvers.
			std::vector<Type> x(maybeStates.getNumberOfSetBits(), storm::utility::constGetOne<Type>());

			// Prepare the right-hand side of the equation system.
			std::vector<Type>* b = new std::vector<Type>(maybeStates.getNumberOfSetBits());
			if (this->getModel().hasTransitionRewards()) {
				// If a transition-based reward model is available, we initialize the right-hand
				// side to the vector resulting from summing the rows of the pointwise product
				// of the transition probability matrix and the transition reward matrix.
				std::vector<Type>* pointwiseProductRowSumVector = this->getModel().getTransitionProbabilityMatrix()->getPointwiseProductRowSumVector(*this->getModel().getTransitionRewardMatrix());
				storm::utility::selectVectorValues(b, maybeStates, *pointwiseProductRowSumVector);
				delete pointwiseProductRowSumVector;

				if (this->getModel().hasStateRewards()) {
					// If a state-based reward model is also available, we need to add this vector
					// as well. As the state reward vector contains entries not just for the states
					// that we still consider (i.e. maybeStates), we need to extract these values
					// first.
					std::vector<Type>* subStateRewards = new std::vector<Type>(maybeStates.getNumberOfSetBits());
					storm::utility::setVectorValues(subStateRewards, maybeStates, *this->getModel().getStateRewards());
					gmm::add(*subStateRewards, *b);
					delete subStateRewards;
				}
			} else {
				// If only a state-based reward model is  available, we take this vector as the
				// right-hand side. As the state reward vector contains entries not just for the
				// states that we still consider (i.e. maybeStates), we need to extract these values
				// first.
				storm::utility::setVectorValues(b, maybeStates, *this->getModel().getStateRewards());
			}

			// Solve the corresponding system of linear equations.
			this->solveLinearEquationSystem(*gmmxxMatrix, x, *b);

			// Set values of resulting vector according to result.
			storm::utility::setVectorValues<Type>(result, maybeStates, x);

			// Delete temporary matrix and right-hand side.
			delete gmmxxMatrix;
			delete b;
		}

		// Set values of resulting vector that are known exactly.
		storm::utility::setVectorValues(result, *targetStates, storm::utility::constGetZero<Type>());
		storm::utility::setVectorValues(result, infinityStates, storm::utility::constGetInfinity<Type>());

		// Delete temporary storages and return result.
		delete targetStates;
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
		if ((lemethod != "bicgstab") && (lemethod != "qmr")) {
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
	void solveLinearEquationSystem(gmm::csr_matrix<Type> const& A, std::vector<Type>& x, std::vector<Type> const& b) const {
		// Get the settings object to customize linear solving.
		storm::settings::Settings* s = storm::settings::instance();

		// Prepare an iteration object that determines the accuracy, maximum number of iterations
		// and the like.
		gmm::iteration iter(s->get<double>("precision"), 0, s->get<unsigned>("lemaxiter"));

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
				gmm::bicgstab(A, x, b, gmm::ilu_precond<gmm::csr_matrix<Type>>(A), iter);
			} else if (precond == "diagonal") {
				gmm::bicgstab(A, x, b, gmm::diagonal_precond<gmm::csr_matrix<Type>>(A), iter);
			} else if (precond == "ildlt") {
				gmm::bicgstab(A, x, b, gmm::ildlt_precond<gmm::csr_matrix<Type>>(A), iter);
			} else if (precond == "none") {
				gmm::bicgstab(A, x, b, gmm::identity_matrix(), iter);
			}
		// FIXME: gmres has been disabled, because it triggers gmm++ compilation errors
		/* } else if (s->getString("lemethod").compare("gmres") == 0) {
			LOG4CPLUS_INFO(logger, "Using GMRES method.");
			if (precond.compare("ilu")) {
				gmm::gmres(A, x, b, gmm::ilu_precond<gmm::csr_matrix<Type>>(A), s->get<unsigned>("restart"), iter);
			} else if (precond == "diagonal") {
				gmm::gmres(A, x, b, gmm::diagonal_precond<gmm::csr_matrix<Type>>(A), s->get<unsigned>("restart"), iter);
			} else if (precond == "ildlt") {
				gmm::gmres(A, x, b, gmm::ildlt_precond<gmm::csr_matrix<Type>>(A), s->get<unsigned>("restart"), iter);
			} else if (precond == "none") {
				gmm::gmres(A, x, b, gmm::identity_matrix(), s->get<unsigned>("restart"), iter);
			} */
		} else if (s->getString("lemethod") == "qmr") {
			LOG4CPLUS_INFO(logger, "Using QMR method.");
			if (precond == "ilu") {
				gmm::qmr(A, x, b, gmm::ilu_precond<gmm::csr_matrix<Type>>(A), iter);
			} else if (precond == "diagonal") {
				gmm::qmr(A, x, b, gmm::diagonal_precond<gmm::csr_matrix<Type>>(A), iter);
			} else if (precond == "ildlt") {
				gmm::qmr(A, x, b, gmm::ildlt_precond<gmm::csr_matrix<Type>>(A), iter);
			} else if (precond == "none") {
				gmm::qmr(A, x, b, gmm::identity_matrix(), iter);
			}
		}

		// Check if the solver converged and issue a warning otherwise.
		if (iter.converged()) {
			LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iter.get_iteration() << " iterations.");
		} else {
			LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
		}
	}
};

} //namespace modelChecker

} //namespace storm

#endif /* STORM_MODELCHECKER_GMMXXDTMCPRCTLMODELCHECKER_H_ */
