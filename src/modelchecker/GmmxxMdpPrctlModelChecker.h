/*
 * GmmxxDtmcPrctlModelChecker.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_MODELCHECKER_GMMXXMDPPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_GMMXXMDPPRCTLMODELCHECKER_H_

#include <cmath>

#include "src/models/Mdp.h"
#include "src/modelchecker/MdpPrctlModelChecker.h"
#include "src/utility/GraphAnalyzer.h"
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
class GmmxxMdpPrctlModelChecker : public MdpPrctlModelChecker<Type> {

public:
	explicit GmmxxMdpPrctlModelChecker(storm::models::Mdp<Type>& mdp) : MdpPrctlModelChecker<Type>(mdp) { }

	virtual ~GmmxxMdpPrctlModelChecker() { }

	virtual std::vector<Type>* checkBoundedUntil(const storm::formula::BoundedUntil<Type>& formula) const {
		// First, we need to compute the states that satisfy the sub-formulas of the until-formula.
		storm::storage::BitVector* leftStates = this->checkStateFormula(formula.getLeft());
		storm::storage::BitVector* rightStates = this->checkStateFormula(formula.getRight());

		// Copy the matrix before we make any changes.
		storm::storage::SparseMatrix<Type> tmpMatrix(*this->getModel().getTransitionMatrix());

		// Get the starting row indices for the non-deterministic choices to reduce the resulting
		// vector properly.
		std::shared_ptr<std::vector<uint_fast64_t>> nondeterministicChoiceIndices = this->getModel().getNondeterministicChoiceIndices();

		// Make all rows absorbing that violate both sub-formulas or satisfy the second sub-formula.
		tmpMatrix.makeRowsAbsorbing(~(*leftStates | *rightStates) | *rightStates);

		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(tmpMatrix);

		// Create the vector with which to multiply.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());
		storm::utility::setVectorValues(result, *rightStates, storm::utility::constGetOne<Type>());

		// Create vector for result of multiplication, which is reduced to the result vector after
		// each multiplication.
		std::vector<Type>* multiplyResult = new std::vector<Type>(this->getModel().getTransitionMatrix()->getRowCount());

		// Now perform matrix-vector multiplication as long as we meet the bound of the formula.
		for (uint_fast64_t i = 0; i < formula.getBound(); ++i) {
			gmm::mult(*gmmxxMatrix, *result, *multiplyResult);

			if (this->minimumOperatorStack.top()) {
				storm::utility::reduceVectorMin(*multiplyResult, result, *nondeterministicChoiceIndices);
			} else {
				storm::utility::reduceVectorMax(*multiplyResult, result, *nondeterministicChoiceIndices);
			}
		}
		delete multiplyResult;

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
		gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(*this->getModel().getTransitionMatrix());

		// Create the vector with which to multiply and initialize it correctly.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());
		storm::utility::setVectorValues(result, *nextStates, storm::utility::constGetOne<Type>());

		// Delete obsolete sub-result.
		delete nextStates;

		// Create resulting vector.
		std::vector<Type>* temporaryResult = new std::vector<Type>(this->getModel().getTransitionMatrix()->getRowCount());

		// Perform the actual computation, namely matrix-vector multiplication.
		gmm::mult(*gmmxxMatrix, *result, *temporaryResult);

		// Get the starting row indices for the non-deterministic choices to reduce the resulting
		// vector properly.
		std::shared_ptr<std::vector<uint_fast64_t>> nondeterministicChoiceIndices = this->getModel().getNondeterministicChoiceIndices();

		if (this->minimumOperatorStack.top()) {
			storm::utility::reduceVectorMin(*temporaryResult, result, *nondeterministicChoiceIndices);
		} else {
			storm::utility::reduceVectorMax(*temporaryResult, result, *nondeterministicChoiceIndices);
		}

		// Delete temporary matrix plus temporary result and return result.
		delete gmmxxMatrix;
		delete temporaryResult;
		return result;
	}

	virtual std::vector<Type>* checkUntil(const storm::formula::Until<Type>& formula) const {
		// First, we need to compute the states that satisfy the sub-formulas of the until-formula.
		storm::storage::BitVector* leftStates = this->checkStateFormula(formula.getLeft());
		storm::storage::BitVector* rightStates = this->checkStateFormula(formula.getRight());

		// Then, we need to identify the states which have to be taken out of the matrix, i.e.
		// all states that have probability 0 and 1 of satisfying the until-formula.
		storm::storage::BitVector statesWithProbability0(this->getModel().getNumberOfStates());
		storm::storage::BitVector statesWithProbability1(this->getModel().getNumberOfStates());
		if (this->minimumOperatorStack.top()) {
			storm::utility::GraphAnalyzer::performProb01Min(this->getModel(), *leftStates, *rightStates, &statesWithProbability0, &statesWithProbability1);
		} else {
			storm::utility::GraphAnalyzer::performProb01Max(this->getModel(), *leftStates, *rightStates, &statesWithProbability0, &statesWithProbability1);
		}

		// Delete sub-results that are obsolete now.
		delete leftStates;
		delete rightStates;

		LOG4CPLUS_INFO(logger, "Found " << statesWithProbability0.getNumberOfSetBits() << " 'no' states.");
		LOG4CPLUS_INFO(logger, "Found " << statesWithProbability1.getNumberOfSetBits() << " 'yes' states.");
		storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);
		LOG4CPLUS_INFO(logger, "Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");

		// Create resulting vector.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());

		// Only try to solve system if there are states for which the probability is unknown.
		uint_fast64_t maybeStatesSetBitCount = maybeStates.getNumberOfSetBits();
		if (maybeStatesSetBitCount > 0) {
			// First, we can eliminate the rows and columns from the original transition probability matrix for states
			// whose probabilities are already known.
			storm::storage::SparseMatrix<Type>* submatrix = this->getModel().getTransitionMatrix()->getSubmatrix(maybeStates, *this->getModel().getNondeterministicChoiceIndices());

			// Get the "new" nondeterministic choice indices for the submatrix.
			std::shared_ptr<std::vector<uint_fast64_t>> subNondeterministicChoiceIndices = this->computeNondeterministicChoiceIndicesForConstraint(maybeStates);

			// Transform the submatrix to the gmm++ format to use its capabilities.
			gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(*submatrix);

			// Create vector for results for maybe states.
			std::vector<Type>* x = new std::vector<Type>(maybeStatesSetBitCount);

			// Prepare the right-hand side of the equation system. For entry i this corresponds to
			// the accumulated probability of going from state i to some 'yes' state.
			std::vector<Type> b(submatrix->getRowCount());
			this->getModel().getTransitionMatrix()->getConstrainedRowSumVector(maybeStates, *this->getModel().getNondeterministicChoiceIndices(), statesWithProbability1, &b);
			delete submatrix;

			// Solve the corresponding system of equations.
			this->solveEquationSystem(*gmmxxMatrix, x, b, *subNondeterministicChoiceIndices);

			// Set values of resulting vector according to result.
			storm::utility::setVectorValues<Type>(result, maybeStates, *x);

			// Delete temporary matrix and vector.
			delete gmmxxMatrix;
			delete x;
		}

		// Set values of resulting vector that are known exactly.
		storm::utility::setVectorValues<Type>(result, statesWithProbability0, storm::utility::constGetZero<Type>());
		storm::utility::setVectorValues<Type>(result, statesWithProbability1, storm::utility::constGetOne<Type>());

		return result;
	}

	virtual std::vector<Type>* checkInstantaneousReward(const storm::formula::InstantaneousReward<Type>& formula) const {
		// Only compute the result if the model has a state-based reward model.
		if (!this->getModel().hasStateRewards()) {
			LOG4CPLUS_ERROR(logger, "Missing (state-based) reward model for formula.");
			throw storm::exceptions::InvalidPropertyException() << "Missing (state-based) reward model for formula.";
		}

		// Transform the transition probability matrix to the gmm++ format to use its arithmetic.
		gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(*this->getModel().getTransitionMatrix());

		// Initialize result to state rewards of the model.
		std::vector<Type>* result = new std::vector<Type>(*this->getModel().getStateRewardVector());

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
		gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(*this->getModel().getTransitionMatrix());

		// Compute the reward vector to add in each step based on the available reward models.
		std::vector<Type>* totalRewardVector = nullptr;
		if (this->getModel().hasTransitionRewards()) {
			totalRewardVector = this->getModel().getTransitionMatrix()->getPointwiseProductRowSumVector(*this->getModel().getTransitionRewardMatrix());
			if (this->getModel().hasStateRewards()) {
				gmm::add(*this->getModel().getStateRewardVector(), *totalRewardVector);
			}
		} else {
			totalRewardVector = new std::vector<Type>(*this->getModel().getStateRewardVector());
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
		// TODO: just commented out to make it compile
		//storm::utility::GraphAnalyzer::performProb1(this->getModel(), trueStates, *targetStates, &infinityStates);
		infinityStates.complement();

		// Create resulting vector.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());

		// Check whether there are states for which we have to compute the result.
		storm::storage::BitVector maybeStates = ~(*targetStates) & ~infinityStates;
		const int maybeStatesSetBitCount = maybeStates.getNumberOfSetBits();
		if (maybeStatesSetBitCount > 0) {
			// Now we can eliminate the rows and columns from the original transition probability matrix.
			storm::storage::SparseMatrix<Type>* submatrix = this->getModel().getTransitionMatrix()->getSubmatrix(maybeStates);
			// Converting the matrix from the fixpoint notation to the form needed for the equation
			// system. That is, we go from x = A*x + b to (I-A)x = b.
			submatrix->convertToEquationSystem();

			// Transform the submatrix to the gmm++ format to use its solvers.
			gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(*submatrix);
			delete submatrix;

			// Initialize the x vector with 1 for each element. This is the initial guess for
			// the iterative solvers.
			std::vector<Type> x(maybeStatesSetBitCount, storm::utility::constGetOne<Type>());

			// Prepare the right-hand side of the equation system.
			std::vector<Type>* b = new std::vector<Type>(maybeStatesSetBitCount);
			if (this->getModel().hasTransitionRewards()) {
				// If a transition-based reward model is available, we initialize the right-hand
				// side to the vector resulting from summing the rows of the pointwise product
				// of the transition probability matrix and the transition reward matrix.
				std::vector<Type>* pointwiseProductRowSumVector = this->getModel().getTransitionMatrix()->getPointwiseProductRowSumVector(*this->getModel().getTransitionRewardMatrix());
				storm::utility::selectVectorValues(b, maybeStates, *pointwiseProductRowSumVector);
				delete pointwiseProductRowSumVector;

				if (this->getModel().hasStateRewards()) {
					// If a state-based reward model is also available, we need to add this vector
					// as well. As the state reward vector contains entries not just for the states
					// that we still consider (i.e. maybeStates), we need to extract these values
					// first.
					std::vector<Type>* subStateRewards = new std::vector<Type>(maybeStatesSetBitCount);
					storm::utility::setVectorValues(subStateRewards, maybeStates, *this->getModel().getStateRewardVector());
					gmm::add(*subStateRewards, *b);
					delete subStateRewards;
				}
			} else {
				// If only a state-based reward model is  available, we take this vector as the
				// right-hand side. As the state reward vector contains entries not just for the
				// states that we still consider (i.e. maybeStates), we need to extract these values
				// first.
				storm::utility::setVectorValues(b, maybeStates, *this->getModel().getStateRewardVector());
			}

			// Solve the corresponding system of linear equations.
			// TODO: just commented out to make it compile
			// this->solveEquationSystem(*gmmxxMatrix, x, *b);

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

private:
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
	void solveEquationSystem(gmm::csr_matrix<Type> const& A, std::vector<Type>* x, std::vector<Type> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices) const {
		// Get the settings object to customize solving.
		storm::settings::Settings* s = storm::settings::instance();

		// Get relevant user-defined settings for solving the equations.
		double precision = s->get<double>("precision");
		unsigned maxIterations = s->get<unsigned>("maxiter");
		bool relative = s->get<bool>("relative");

		// Set up the environment for the power method.
		std::vector<Type>* temporaryResult = new std::vector<Type>(b.size());
		std::vector<Type>* newX = new std::vector<Type>(x->size());
		std::vector<Type>* swap = nullptr;
		bool converged = false;
		uint_fast64_t iterations = 0;

		// Proceed with the iterations as long as the method did not converge or reach the
		// user-specified maximum number of iterations.
		while (!converged && iterations < maxIterations) {
			// Compute x' = A*x + b.
			gmm::mult(A, *x, *temporaryResult);
			gmm::add(b, *temporaryResult);

			// Reduce the vector x' by applying min/max for all non-deterministic choices.
			if (this->minimumOperatorStack.top()) {
				storm::utility::reduceVectorMin(*temporaryResult, newX, nondeterministicChoiceIndices);
			} else {
				storm::utility::reduceVectorMax(*temporaryResult, newX, nondeterministicChoiceIndices);
			}

			// Determine whether the method converged.
			converged = storm::utility::equalModuloPrecision(*x, *newX, precision, relative);

			// Update environment variables.
			swap = x;
			x = newX;
			newX = swap;
			++iterations;
		}

		delete temporaryResult;

		// Check if the solver converged and issue a warning otherwise.
		if (converged) {
			LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iterations << " iterations.");
		} else {
			LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
		}
	}

	std::shared_ptr<std::vector<uint_fast64_t>> computeNondeterministicChoiceIndicesForConstraint(storm::storage::BitVector constraint) const {
		std::shared_ptr<std::vector<uint_fast64_t>> nondeterministicChoiceIndices = this->getModel().getNondeterministicChoiceIndices();
		std::shared_ptr<std::vector<uint_fast64_t>> subNondeterministicChoiceIndices(new std::vector<uint_fast64_t>(constraint.getNumberOfSetBits() + 1));
		uint_fast64_t currentRowCount = 0;
		uint_fast64_t currentIndexCount = 1;
		(*subNondeterministicChoiceIndices)[0] = 0;
		for (auto index : constraint) {
			(*subNondeterministicChoiceIndices)[currentIndexCount] = currentRowCount + (*nondeterministicChoiceIndices)[index + 1] - (*nondeterministicChoiceIndices)[index];
			currentRowCount += (*nondeterministicChoiceIndices)[index + 1] - (*nondeterministicChoiceIndices)[index];
			++currentIndexCount;
		}
		(*subNondeterministicChoiceIndices)[constraint.getNumberOfSetBits()] = currentRowCount;

		return subNondeterministicChoiceIndices;
	}
};

} //namespace modelChecker

} //namespace storm

#endif /* STORM_MODELCHECKER_GMMXXMDPPRCTLMODELCHECKER_H_ */
