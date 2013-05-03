/*
 * SparseDtmcPrctlModelChecker.h
 *
 *  Created on: 22.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_

#include "src/modelchecker/AbstractModelChecker.h"
#include "src/models/Dtmc.h"
#include "src/utility/Vector.h"
#include "src/utility/GraphAnalyzer.h"

#include <vector>

namespace storm {
namespace modelchecker {

/*!
 * @brief
 * Interface for all model checkers that can verify PRCTL formulae over DTMCs represented as a sparse matrix.
 */
template<class Type>
class SparseDtmcPrctlModelChecker : public AbstractModelChecker<Type> {

public:
	/*!
	 * Constructs a SparseDtmcPrctlModelChecker with the given model.
	 *
	 * @param model The DTMC to be checked.
	 */
	explicit SparseDtmcPrctlModelChecker(storm::models::Dtmc<Type> const& model) : AbstractModelChecker<Type>(model) {
		// Intentionally left empty.
	}

	/*!
	 * Copy constructs a SparseDtmcPrctlModelChecker from the given model checker. In particular, this means that the newly
	 * constructed model checker will have the model of the given model checker as its associated model.
	 */
	explicit SparseDtmcPrctlModelChecker(storm::modelchecker::SparseDtmcPrctlModelChecker<Type> const& modelChecker) : AbstractModelChecker<Type>(modelChecker) {
		// Intentionally left empty.
	}

	/*!
	 * Virtual destructor. Needs to be virtual, because this class has virtual methods.
	 */
	virtual ~SparseDtmcPrctlModelChecker() {
		// Intentionally left empty.
	}

	/*!
	 * Returns a constant reference to the DTMC associated with this model checker.
	 * @returns A constant reference to the DTMC associated with this model checker.
	 */
	storm::models::Dtmc<Type> const& getModel() const {
		return AbstractModelChecker<Type>::template getModel<storm::models::Dtmc<Type>>();
	}

	/*!
	 * Checks the given formula that is a P/R operator without a bound.
	 *
	 * @param formula The formula to check.
	 * @returns The set of states satisfying the formula represented by a bit vector.
	 */
	std::vector<Type>* checkNoBoundOperator(storm::property::prctl::AbstractNoBoundOperator<Type> const& formula) const {
		// Check if the operator was an optimality operator and report a warning in that case.
		if (formula.isOptimalityOperator()) {
			LOG4CPLUS_WARN(logger, "Formula contains min/max operator, which is not meaningful over deterministic models.");
		}
		return formula.check(*this, false);
	}

	/*!
	 * Checks the given formula that is a bounded-until formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bounds 0 and 1. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bounds 0 and 1.
	 * @returns The probabilities for the given formula to hold on every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact probabilities might not be computed.
	 */
	virtual std::vector<Type>* checkBoundedUntil(storm::property::prctl::BoundedUntil<Type> const& formula, bool qualitative) const {
		// First, we need to compute the states that satisfy the sub-formulas of the bounded until-formula.
		storm::storage::BitVector* leftStates = formula.getLeft().check(*this);
		storm::storage::BitVector* rightStates = formula.getRight().check(*this);

		// Copy the matrix before we make any changes.
		storm::storage::SparseMatrix<Type> tmpMatrix(*this->getModel().getTransitionMatrix());

		// Make all rows absorbing that violate both sub-formulas or satisfy the second sub-formula.
		tmpMatrix.makeRowsAbsorbing(~(*leftStates | *rightStates) | *rightStates);

		// Delete obsolete intermediates.
		delete leftStates;
		delete rightStates;

		// Create the vector with which to multiply.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());
		storm::utility::setVectorValues(result, *rightStates, storm::utility::constGetOne<Type>());

		// Perform the matrix vector multiplication as often as required by the formula bound.
		this->performMatrixVectorMultiplication(tmpMatrix, *result, nullptr, formula.getBound());

		// Return result.
		return result;
	}

	/*!
	 * Checks the given formula that is a next formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bounds 0 and 1. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bounds 0 and 1.
	 * @returns The probabilities for the given formula to hold on every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact probabilities might not be computed.
	 */
	virtual std::vector<Type>* checkNext(storm::property::prctl::Next<Type> const& formula, bool qualitative) const {
		// First, we need to compute the states that satisfy the child formula of the next-formula.
		storm::storage::BitVector* nextStates = formula.getChild().check(*this);

		// Create the vector with which to multiply and initialize it correctly.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());
		storm::utility::setVectorValues(result, *nextStates, storm::utility::constGetOne<Type>());

		// Delete obsolete intermediate.
		delete nextStates;

		// Perform one single matrix-vector multiplication.
		this->performMatrixVectorMultiplication(*this->getModel().getTransitionMatrix(), *result);

		// Return result.
		return result;
	}

	/*!
	 * Checks the given formula that is a bounded-eventually formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bounds 0 and 1. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bounds 0 and 1.
	 * @returns The probabilities for the given formula to hold on every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact probabilities might not be computed.
	 */
	virtual std::vector<Type>* checkBoundedEventually(storm::property::prctl::BoundedEventually<Type> const& formula, bool qualitative) const {
		// Create equivalent temporary bounded until formula and check it.
		storm::property::prctl::BoundedUntil<Type> temporaryBoundedUntilFormula(new storm::property::prctl::Ap<Type>("true"), formula.getChild().clone(), formula.getBound());
		return this->checkBoundedUntil(temporaryBoundedUntilFormula, qualitative);
	}

	/*!
	 * Checks the given formula that is an eventually formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bounds 0 and 1. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bounds 0 and 1.
	 * @returns The probabilities for the given formula to hold on every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact probabilities might not be computed.
	 */
	virtual std::vector<Type>* checkEventually(storm::property::prctl::Eventually<Type> const& formula, bool qualitative) const {
		// Create equivalent temporary until formula and check it.
		storm::property::prctl::Until<Type> temporaryUntilFormula(new storm::property::prctl::Ap<Type>("true"), formula.getChild().clone());
		return this->checkUntil(temporaryUntilFormula, qualitative);
	}

	/*!
	 * Checks the given formula that is a globally formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bounds 0 and 1. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bounds 0 and 1.
	 * @returns The probabilities for the given formula to hold on every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact probabilities might not be computed.
	 */
	virtual std::vector<Type>* checkGlobally(storm::property::prctl::Globally<Type> const& formula, bool qualitative) const {
		// Create "equivalent" (equivalent up to negation) temporary eventually formula and check it.
		storm::property::prctl::Eventually<Type> temporaryEventuallyFormula(new storm::property::prctl::Not<Type>(formula.getChild().clone()));
		std::vector<Type>* result = this->checkEventually(temporaryEventuallyFormula, qualitative);

		// Now subtract the resulting vector from the constant one vector to obtain final result.
		storm::utility::subtractFromConstantOneVector(result);
		return result;
	}

	/*!
	 * Check the given formula that is an until formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bounds 0 and 1. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bounds 0 and 1.
	 * @returns The probabilities for the given formula to hold on every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact probabilities might not be computed.
	 */
	virtual std::vector<Type>* checkUntil(storm::property::prctl::Until<Type> const& formula, bool qualitative) const {
		// First, we need to compute the states that satisfy the sub-formulas of the until-formula.
		storm::storage::BitVector* leftStates = formula.getLeft().check(*this);
		storm::storage::BitVector* rightStates = formula.getRight().check(*this);

		// Then, we need to identify the states which have to be taken out of the matrix, i.e.
		// all states that have probability 0 and 1 of satisfying the until-formula.
		storm::storage::BitVector statesWithProbability0(this->getModel().getNumberOfStates());
		storm::storage::BitVector statesWithProbability1(this->getModel().getNumberOfStates());
		storm::utility::GraphAnalyzer::performProb01(this->getModel(), *leftStates, *rightStates, &statesWithProbability0, &statesWithProbability1);

		// Delete intermediate results that are obsolete now.
		delete leftStates;
		delete rightStates;

		// Perform some logging.
		LOG4CPLUS_INFO(logger, "Found " << statesWithProbability0.getNumberOfSetBits() << " 'no' states.");
		LOG4CPLUS_INFO(logger, "Found " << statesWithProbability1.getNumberOfSetBits() << " 'yes' states.");
		storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);
		LOG4CPLUS_INFO(logger, "Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");

		// Create resulting vector.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());

		// Only try to solve system if there are states for which the probability is unknown.
		uint_fast64_t maybeStatesSetBitCount = maybeStates.getNumberOfSetBits();
		if (maybeStatesSetBitCount > 0 && !qualitative) {
			// Now we can eliminate the rows and columns from the original transition probability matrix.
			storm::storage::SparseMatrix<Type> submatrix = this->getModel().getTransitionMatrix()->getSubmatrix(maybeStates);
			// Converting the matrix from the fixpoint notation to the form needed for the equation
			// system. That is, we go from x = A*x + b to (I-A)x = b.
			submatrix.convertToEquationSystem();

			// Initialize the x vector with 0.5 for each element. This is the initial guess for
			// the iterative solvers. It should be safe as for all 'maybe' states we know that the
			// probability is strictly larger than 0.
			std::vector<Type> x(maybeStatesSetBitCount, Type(0.5));

			// Prepare the right-hand side of the equation system. For entry i this corresponds to
			// the accumulated probability of going from state i to some 'yes' state.
			std::vector<Type> b = this->getModel().getTransitionMatrix()->getConstrainedRowSumVector(maybeStates, statesWithProbability1);

			// Now solve the created system of linear equations.
			this->solveEquationSystem(submatrix, x, b);

			// Set values of resulting vector according to result.
			storm::utility::setVectorValues<Type>(result, maybeStates, x);
		} else if (qualitative) {
			// If we only need a qualitative result, we can safely assume that the results will only be compared to
			// bounds which are either 0 or 1. Setting the value to 0.5 is thus safe.
			storm::utility::setVectorValues<Type>(result, maybeStates, Type(0.5));
		}

		// Set values of resulting vector that are known exactly.
		storm::utility::setVectorValues<Type>(result, statesWithProbability0, storm::utility::constGetZero<Type>());
		storm::utility::setVectorValues<Type>(result, statesWithProbability1, storm::utility::constGetOne<Type>());

		return result;
	}

	/*!
	 * Checks the given formula that is an instantaneous reward formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bound 0. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bound 0.
	 * @returns The reward values for the given formula for every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact values might not be computed.
	 */
	virtual std::vector<Type>* checkInstantaneousReward(storm::property::prctl::InstantaneousReward<Type> const& formula, bool qualitative) const {
		// Only compute the result if the model has a state-based reward model.
		if (!this->getModel().hasStateRewards()) {
			LOG4CPLUS_ERROR(logger, "Missing (state-based) reward model for formula.");
			throw storm::exceptions::InvalidPropertyException() << "Missing (state-based) reward model for formula.";
		}

		// Initialize result to state rewards of the model.
		std::vector<Type>* result = new std::vector<Type>(*this->getModel().getStateRewardVector());

		// Perform the actual matrix-vector multiplication as long as the bound of the formula is met.
		this->performMatrixVectorMultiplication(*this->getModel().getTransitionMatrix(), *result, nullptr, formula.getBound());

		// Return result.
		return result;
	}

	/*!
	 * Check the given formula that is a cumulative reward formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bound 0. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bound 0.
	 * @returns The reward values for the given formula for every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact values might not be computed.
	 */
	virtual std::vector<Type>* checkCumulativeReward(storm::property::prctl::CumulativeReward<Type> const& formula, bool qualitative) const {
		// Only compute the result if the model has at least one reward model.
		if (!this->getModel().hasStateRewards() && !this->getModel().hasTransitionRewards()) {
			LOG4CPLUS_ERROR(logger, "Missing reward model for formula.");
			throw storm::exceptions::InvalidPropertyException() << "Missing reward model for formula.";
		}

		// Compute the reward vector to add in each step based on the available reward models.
		std::vector<Type> totalRewardVector;
		if (this->getModel().hasTransitionRewards()) {
			totalRewardVector = this->getModel().getTransitionMatrix()->getPointwiseProductRowSumVector(*this->getModel().getTransitionRewardMatrix());
			if (this->getModel().hasStateRewards()) {
				gmm::add(*this->getModel().getStateRewardVector(), totalRewardVector);
			}
		} else {
			totalRewardVector = std::vector<Type>(*this->getModel().getStateRewardVector());
		}

		// Initialize result to either the state rewards of the model or the null vector.
		std::vector<Type>* result = nullptr;
		if (this->getModel().hasStateRewards()) {
			result = new std::vector<Type>(*this->getModel().getStateRewardVector());
		} else {
			result = new std::vector<Type>(this->getModel().getNumberOfStates());
		}

		// Perform the actual matrix-vector multiplication as long as the bound of the formula is met.
		this->performMatrixVectorMultiplication(*this->getModel().getTransitionMatrix(), *result, &totalRewardVector, formula.getBound());

		// Return result.
		return result;
	}

	/*!
	 * Checks the given formula that is a reachability reward formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bound 0. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bound 0.
	 * @returns The reward values for the given formula for every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact values might not be computed.
	 */
	virtual std::vector<Type>* checkReachabilityReward(storm::property::prctl::ReachabilityReward<Type> const& formula, bool qualitative) const {
		// Only compute the result if the model has at least one reward model.
		if (!this->getModel().hasStateRewards() && !this->getModel().hasTransitionRewards()) {
			LOG4CPLUS_ERROR(logger, "Missing reward model for formula. Skipping formula");
			throw storm::exceptions::InvalidPropertyException() << "Missing reward model for formula.";
		}

		// Determine the states for which the target predicate holds.
		storm::storage::BitVector* targetStates = formula.getChild().check(*this);

		// Determine which states have a reward of infinity by definition.
		storm::storage::BitVector infinityStates(this->getModel().getNumberOfStates());
		storm::storage::BitVector trueStates(this->getModel().getNumberOfStates(), true);
		storm::utility::GraphAnalyzer::performProb1(this->getModel(), trueStates, *targetStates, &infinityStates);
		infinityStates.complement();

		// Create resulting vector.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());

		// Check whether there are states for which we have to compute the result.
		storm::storage::BitVector maybeStates = ~(*targetStates) & ~infinityStates;
		const int maybeStatesSetBitCount = maybeStates.getNumberOfSetBits();
		if (maybeStatesSetBitCount > 0) {
			// Now we can eliminate the rows and columns from the original transition probability matrix.
			storm::storage::SparseMatrix<Type> submatrix = this->getModel().getTransitionMatrix()->getSubmatrix(maybeStates);
			// Converting the matrix from the fixpoint notation to the form needed for the equation
			// system. That is, we go from x = A*x + b to (I-A)x = b.
			submatrix.convertToEquationSystem();

			// Initialize the x vector with 1 for each element. This is the initial guess for
			// the iterative solvers.
			std::vector<Type> x(maybeStatesSetBitCount, storm::utility::constGetOne<Type>());

			// Prepare the right-hand side of the equation system.
			std::vector<Type> b(maybeStatesSetBitCount);
			if (this->getModel().hasTransitionRewards()) {
				// If a transition-based reward model is available, we initialize the right-hand
				// side to the vector resulting from summing the rows of the pointwise product
				// of the transition probability matrix and the transition reward matrix.
				std::vector<Type> pointwiseProductRowSumVector = this->getModel().getTransitionMatrix()->getPointwiseProductRowSumVector(*this->getModel().getTransitionRewardMatrix());
				storm::utility::selectVectorValues(&b, maybeStates, pointwiseProductRowSumVector);

				if (this->getModel().hasStateRewards()) {
					// If a state-based reward model is also available, we need to add this vector
					// as well. As the state reward vector contains entries not just for the states
					// that we still consider (i.e. maybeStates), we need to extract these values
					// first.
					std::vector<Type> subStateRewards(maybeStatesSetBitCount);
					storm::utility::selectVectorValues(&subStateRewards, maybeStates, *this->getModel().getStateRewardVector());
					gmm::add(subStateRewards, b);
				}
			} else {
				// If only a state-based reward model is  available, we take this vector as the
				// right-hand side. As the state reward vector contains entries not just for the
				// states that we still consider (i.e. maybeStates), we need to extract these values
				// first.
				storm::utility::selectVectorValues(&b, maybeStates, *this->getModel().getStateRewardVector());
			}

			// Now solve the resulting equation system.
			this->solveEquationSystem(submatrix, x, b);

			// Set values of resulting vector according to result.
			storm::utility::setVectorValues<Type>(result, maybeStates, x);
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
	 * Performs (repeated) matrix-vector multiplication with the given parameters, i.e. computes x[i+1] = A*x[i] + b
	 * until x[n], where x[0] = x.
	 *
	 * @param A The matrix that is to be multiplied against the vector.
	 * @param x The initial vector that is to be multiplied against the matrix. This is also the output parameter,
	 * i.e. after the method returns, this vector will contain the computed values.
	 * @param b If not null, this vector is being added to the result after each matrix-vector multiplication.
	 * @param n Specifies the number of iterations the matrix-vector multiplication is performed.
	 * @returns The result of the repeated matrix-vector multiplication as the content of the parameter vector.
	 */
	virtual void performMatrixVectorMultiplication(storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type>* b = nullptr, uint_fast64_t n = 1) const = 0;

	/*!
	 * Solves the equation system A*x = b given by the parameters.
	 *
	 * @param A The matrix specifying the coefficients of the linear equations.
	 * @param x The solution vector x. The initial values of x represent a guess of the real values to the solver, but
	 * may be ignored.
	 * @param b The right-hand side of the equation system.
	 * @returns The solution vector x of the system of linear equations as the content of the parameter x.
	 */
	virtual void solveEquationSystem(storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type> const& b) const = 0;
};

} // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_ */
