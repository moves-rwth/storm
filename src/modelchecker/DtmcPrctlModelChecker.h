/*
 * DtmcPrctlModelChecker.h
 *
 *  Created on: 22.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_MODELCHECKER_DTMCPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_DTMCPRCTLMODELCHECKER_H_

#include <vector>

#include "src/formula/Formulas.h"
#include "src/utility/Vector.h"
#include "src/storage/SparseMatrix.h"

#include "src/models/Dtmc.h"
#include "src/storage/BitVector.h"
#include "src/exceptions/InvalidPropertyException.h"
#include "src/utility/Vector.h"
#include "src/utility/GraphAnalyzer.h"
#include "src/modelchecker/AbstractModelChecker.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {

namespace modelChecker {

/*!
 * @brief
 * Interface for model checker classes.
 *
 * This class provides basic functions that are the same for all subclasses, but mainly only declares
 * abstract methods that are to be implemented in concrete instances.
 *
 * @attention This class is abstract.
 */
template<class Type>
class DtmcPrctlModelChecker : public AbstractModelChecker<Type> {

public:
	/*!
	 * Constructor
	 *
	 * @param model The dtmc model which is checked.
	 */
	explicit DtmcPrctlModelChecker(storm::models::Dtmc<Type>& model) : AbstractModelChecker<Type>(model) {
		// Intentionally left empty.
	}

	/*!
	 * Copy constructor
	 *
	 * @param modelChecker The model checker that is copied.
	 */
	explicit DtmcPrctlModelChecker(const storm::modelChecker::DtmcPrctlModelChecker<Type>* modelChecker) : AbstractModelChecker<Type>(modelChecker) {
		// Intentionally left empty.
	}

	/*!
	 * Destructor
	 */
	virtual ~DtmcPrctlModelChecker() {
		// Intentionally left empty.
	}

	/*!
	 * @returns A reference to the dtmc of the model checker.
	 */
	storm::models::Dtmc<Type>& getModel() const {
		return AbstractModelChecker<Type>::template getModel<storm::models::Dtmc<Type>>();
	}

	/*!
	 * The check method for a state formula with a probabilistic operator node without bounds as root
	 * in its formula tree
	 *
	 * @param formula The state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	std::vector<Type>* checkNoBoundOperator(const storm::formula::NoBoundOperator<Type>& formula) const {
		// Check if the operator was an optimality operator and report a warning in that case.
		if (formula.isOptimalityOperator()) {
			LOG4CPLUS_WARN(logger, "Formula contains min/max operator which is not meaningful over deterministic models.");
		}
		return formula.getPathFormula().check(*this, false);
	}

	/*!
	 * The check method for a path formula with a Bounded Until operator node as root in its formula tree
	 *
	 * @param formula The Bounded Until path formula to check
	 * @returns for each state the probability that the path formula holds.
	 */
	virtual std::vector<Type>* checkBoundedUntil(const storm::formula::BoundedUntil<Type>& formula, bool qualitative) const {
		// First, we need to compute the states that satisfy the sub-formulas of the until-formula.
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
		this->performMatrixVectorMultiplication(tmpMatrix, &result, nullptr, formula.getBound());

		// Return result.
		return result;
	}

	/*!
	 * The check method for a path formula with a Next operator node as root in its formula tree
	 *
	 * @param formula The Next path formula to check
	 * @returns for each state the probability that the path formula holds.
	 */
	virtual std::vector<Type>* checkNext(const storm::formula::Next<Type>& formula, bool qualitative) const {
		// First, we need to compute the states that satisfy the child formula of the next-formula.
		storm::storage::BitVector* nextStates = formula.getChild().check(*this);

		// Create the vector with which to multiply and initialize it correctly.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());
		storm::utility::setVectorValues(result, *nextStates, storm::utility::constGetOne<Type>());

		// Delete obsolete intermediate.
		delete nextStates;

		// Perform one single matrix-vector multiplication.
		this->performMatrixVectorMultiplication(*this->getModel().getTransitionMatrix(), &result);

		// Return result.
		return result;
	}

	/*!
	 * The check method for a path formula with a Bounded Eventually operator node as root in its
	 * formula tree
	 *
	 * @param formula The Bounded Eventually path formula to check
	 * @returns for each state the probability that the path formula holds
	 */
	virtual std::vector<Type>* checkBoundedEventually(const storm::formula::BoundedEventually<Type>& formula, bool qualitative) const {
		// Create equivalent temporary bounded until formula and check it.
		storm::formula::BoundedUntil<Type> temporaryBoundedUntilFormula(new storm::formula::Ap<Type>("true"), formula.getChild().clone(), formula.getBound());
		return this->checkBoundedUntil(temporaryBoundedUntilFormula, qualitative);
	}

	/*!
	 * The check method for a path formula with an Eventually operator node as root in its formula tree
	 *
	 * @param formula The Eventually path formula to check
	 * @returns for each state the probability that the path formula holds
	 */
	virtual std::vector<Type>* checkEventually(const storm::formula::Eventually<Type>& formula, bool qualitative) const {
		// Create equivalent temporary until formula and check it.
		storm::formula::Until<Type> temporaryUntilFormula(new storm::formula::Ap<Type>("true"), formula.getChild().clone());
		return this->checkUntil(temporaryUntilFormula, qualitative);
	}

	/*!
	 * The check method for a path formula with a Globally operator node as root in its formula tree
	 *
	 * @param formula The Globally path formula to check
	 * @returns for each state the probability that the path formula holds
	 */
	virtual std::vector<Type>* checkGlobally(const storm::formula::Globally<Type>& formula, bool qualitative) const {
		// Create "equivalent" temporary eventually formula and check it.
		storm::formula::Eventually<Type> temporaryEventuallyFormula(new storm::formula::Not<Type>(formula.getChild().clone()));
		std::vector<Type>* result = this->checkEventually(temporaryEventuallyFormula, qualitative);

		// Now subtract the resulting vector from the constant one vector to obtain final result.
		storm::utility::subtractFromConstantOneVector(result);
		return result;
	}

	/*!
	 * The check method for a path formula with an Until operator node as root in its formula tree
	 *
	 * @param formula The Until path formula to check
	 * @returns for each state the probability that the path formula holds.
	 */
	virtual std::vector<Type>* checkUntil(const storm::formula::Until<Type>& formula, bool qualitative) const {
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
			storm::storage::SparseMatrix<Type>* submatrix = this->getModel().getTransitionMatrix()->getSubmatrix(maybeStates);
			// Converting the matrix from the fixpoint notation to the form needed for the equation
			// system. That is, we go from x = A*x + b to (I-A)x = b.
			submatrix->convertToEquationSystem();

			// Initialize the x vector with 0.5 for each element. This is the initial guess for
			// the iterative solvers. It should be safe as for all 'maybe' states we know that the
			// probability is strictly larger than 0.
			std::vector<Type>* x = new std::vector<Type>(maybeStatesSetBitCount, Type(0.5));

			// Prepare the right-hand side of the equation system. For entry i this corresponds to
			// the accumulated probability of going from state i to some 'yes' state.
			std::vector<Type> b(maybeStatesSetBitCount);
			this->getModel().getTransitionMatrix()->getConstrainedRowSumVector(maybeStates, statesWithProbability1, &b);

			this->solveEquationSystem(*submatrix, &x, b);

			// Delete the created submatrix.
			delete submatrix;

			// Set values of resulting vector according to result.
			storm::utility::setVectorValues<Type>(result, maybeStates, *x);
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
	 * The check method for a path formula with an Instantaneous Reward operator node as root in its
	 * formula tree
	 *
	 * @param formula The Instantaneous Reward formula to check
	 * @returns for each state the reward that the instantaneous reward yields
	 */
	virtual std::vector<Type>* checkInstantaneousReward(const storm::formula::InstantaneousReward<Type>& formula, bool qualitative) const {
		// Only compute the result if the model has a state-based reward model.
		if (!this->getModel().hasStateRewards()) {
			LOG4CPLUS_ERROR(logger, "Missing (state-based) reward model for formula.");
			throw storm::exceptions::InvalidPropertyException() << "Missing (state-based) reward model for formula.";
		}

		// Initialize result to state rewards of the model.
		std::vector<Type>* result = new std::vector<Type>(*this->getModel().getStateRewardVector());

		// Perform the actual matrix-vector multiplication as long as the bound of the formula is met.
		this->performMatrixVectorMultiplication(*this->getModel().getTransitionMatrix(), &result, nullptr, formula.getBound());

		// Return result.
		return result;
	}

	/*!
	 * The check method for a path formula with a Cumulative Reward operator node as root in its
	 * formula tree
	 *
	 * @param formula The Cumulative Reward formula to check
	 * @returns for each state the reward that the cumulative reward yields
	 */
	virtual std::vector<Type>* checkCumulativeReward(const storm::formula::CumulativeReward<Type>& formula, bool qualitative) const {
		// Only compute the result if the model has at least one reward model.
		if (!this->getModel().hasStateRewards() && !this->getModel().hasTransitionRewards()) {
			LOG4CPLUS_ERROR(logger, "Missing reward model for formula.");
			throw storm::exceptions::InvalidPropertyException() << "Missing reward model for formula.";
		}

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

		std::vector<Type>* result = new std::vector<Type>(*this->getModel().getStateRewardVector());

		this->performMatrixVectorMultiplication(*this->getModel().getTransitionMatrix(), &result, totalRewardVector, formula.getBound());

		// Delete temporary variables and return result.
		delete totalRewardVector;
		return result;
	}

	/*!
	 * The check method for a path formula with a Reachability Reward operator node as root in its
	 * formula tree
	 *
	 * @param formula The Reachbility Reward formula to check
	 * @returns for each state the reward that the reachability reward yields
	 */
	virtual std::vector<Type>* checkReachabilityReward(const storm::formula::ReachabilityReward<Type>& formula, bool qualitative) const {
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
			storm::storage::SparseMatrix<Type>* submatrix = this->getModel().getTransitionMatrix()->getSubmatrix(maybeStates);
			// Converting the matrix from the fixpoint notation to the form needed for the equation
			// system. That is, we go from x = A*x + b to (I-A)x = b.
			submatrix->convertToEquationSystem();

			// Initialize the x vector with 1 for each element. This is the initial guess for
			// the iterative solvers.
			std::vector<Type>* x = new std::vector<Type>(maybeStatesSetBitCount, storm::utility::constGetOne<Type>());

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
					storm::utility::selectVectorValues(subStateRewards, maybeStates, *this->getModel().getStateRewardVector());
					gmm::add(*subStateRewards, *b);
					delete subStateRewards;
				}
			} else {
				// If only a state-based reward model is  available, we take this vector as the
				// right-hand side. As the state reward vector contains entries not just for the
				// states that we still consider (i.e. maybeStates), we need to extract these values
				// first.
				storm::utility::selectVectorValues(b, maybeStates, *this->getModel().getStateRewardVector());
			}

			this->solveEquationSystem(*submatrix, &x, *b);

			// Set values of resulting vector according to result.
			storm::utility::setVectorValues<Type>(result, maybeStates, *x);

			// Delete temporary matrix and right-hand side.
			delete submatrix;
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
	virtual void performMatrixVectorMultiplication(storm::storage::SparseMatrix<Type> const& matrix, std::vector<Type>** vector, std::vector<Type>* summand = nullptr, uint_fast64_t repetitions = 1) const = 0;

	virtual void solveEquationSystem(storm::storage::SparseMatrix<Type> const& matrix, std::vector<Type>** vector, std::vector<Type>& b) const = 0;
};

} //namespace modelChecker

} //namespace storm

#endif /* STORM_MODELCHECKER_DTMCPRCTLMODELCHECKER_H_ */
