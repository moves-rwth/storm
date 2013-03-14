/*
 * MdpPrctlModelChecker.h
 *
 *  Created on: 15.02.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_MODELCHECKER_MDPPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_MDPPRCTLMODELCHECKER_H_

#include "src/formula/Formulas.h"
#include "src/utility/Vector.h"
#include "src/storage/SparseMatrix.h"

#include "src/models/Mdp.h"
#include "src/storage/BitVector.h"
#include "src/exceptions/InvalidPropertyException.h"
#include "src/utility/Vector.h"
#include "src/modelchecker/AbstractModelChecker.h"
#include <vector>
#include <stack>

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
class MdpPrctlModelChecker :
	public AbstractModelChecker<Type> {

public:
	/*!
	 * Constructor
	 *
	 * @param model The dtmc model which is checked.
	 */
	explicit MdpPrctlModelChecker(storm::models::Mdp<Type>& model)
		: AbstractModelChecker<Type>(model), minimumOperatorStack() {
	}

	/*!
	 * Copy constructor
	 *
	 * @param modelChecker The model checker that is copied.
	 */
	explicit MdpPrctlModelChecker(const storm::modelChecker::MdpPrctlModelChecker<Type>* modelChecker)
		: AbstractModelChecker<Type>(modelChecker),  minimumOperatorStack() {

	}

	/*!
	 * Destructor
	 */
	virtual ~MdpPrctlModelChecker() {
		// Intentionally left empty.
	}

	/*!
	 * @returns A reference to the dtmc of the model checker.
	 */
	storm::models::Mdp<Type>& getModel() const {
		return AbstractModelChecker<Type>::template getModel<storm::models::Mdp<Type>>();
	}

	/*!
	 * Sets the DTMC model which is checked
	 * @param model
	 */
	void setModel(storm::models::Mdp<Type>& model) {
		AbstractModelChecker<Type>::setModel(model);
	}

	/*!
	 * Checks the given state formula on the DTMC and prints the result (true/false) for all initial
	 * states.
	 * @param stateFormula The formula to be checked.
	 */
	void check(const storm::formula::AbstractStateFormula<Type>& stateFormula) const {
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << stateFormula.toString());
		std::cout << "Model checking formula:\t" << stateFormula.toString() << std::endl;
		storm::storage::BitVector* result = nullptr;
		try {
			result = stateFormula.check(*this);
			LOG4CPLUS_INFO(logger, "Result for initial states:");
			std::cout << "Result for initial states:" << std::endl;
			for (auto initialState : *this->getModel().getLabeledStates("init")) {
				LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << (result->get(initialState) ? "satisfied" : "not satisfied"));
				std::cout << "\t" << initialState << ": " << (*result)[initialState] << std::endl;
			}
			delete result;
		} catch (std::exception& e) {
			std::cout << "Error during computation: " << e.what() << "Skipping property." << std::endl;
			if (result != nullptr) {
				delete result;
			}
		}
		std::cout << std::endl;
		storm::utility::printSeparationLine(std::cout);
	}

	/*!
	 * Checks the given operator (with no bound) on the DTMC and prints the result
	 * (probability/rewards) for all initial states.
	 * @param noBoundFormula The formula to be checked.
	 */
	void check(const storm::formula::NoBoundOperator<Type>& noBoundFormula) const {
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << noBoundFormula.toString());
		std::cout << "Model checking formula:\t" << noBoundFormula.toString() << std::endl;
		std::vector<Type>* result = nullptr;
		try {
			result = noBoundFormula.check(*this);
			LOG4CPLUS_INFO(logger, "Result for initial states:");
			std::cout << "Result for initial states:" << std::endl;
			for (auto initialState : *this->getModel().getLabeledStates("init")) {
				LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << (*result)[initialState]);
				std::cout << "\t" << initialState << ": " << (*result)[initialState] << std::endl;
			}
			delete result;
		} catch (std::exception& e) {
			std::cout << "Error during computation: " << e.what() << " Skipping property." << std::endl;
			if (result != nullptr) {
				delete result;
			}
		}
		std::cout << std::endl;
		storm::utility::printSeparationLine(std::cout);
	}

	/*!
	 * The check method for a formula with an AP node as root in its formula tree
	 *
	 * @param formula The Ap state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	storm::storage::BitVector* checkAp(const storm::formula::Ap<Type>& formula) const {
		if (formula.getAp().compare("true") == 0) {
			return new storm::storage::BitVector(this->getModel().getNumberOfStates(), true);
		} else if (formula.getAp().compare("false") == 0) {
			return new storm::storage::BitVector(this->getModel().getNumberOfStates());
		}

		if (!this->getModel().hasAtomicProposition(formula.getAp())) {
			LOG4CPLUS_ERROR(logger, "Atomic proposition '" << formula.getAp() << "' is invalid.");
			throw storm::exceptions::InvalidPropertyException() << "Atomic proposition '" << formula.getAp() << "' is invalid.";
			return nullptr;
		}

		return new storm::storage::BitVector(*this->getModel().getLabeledStates(formula.getAp()));
	}

	/*!
	 * The check method for a state formula with a probabilistic operator node without bounds as root
	 * in its formula tree
	 *
	 * @param formula The state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	std::vector<Type>* checkNoBoundOperator(const storm::formula::NoBoundOperator<Type>& formula) const {
		// Check if the operator was an non-optimality operator and report an error in that case.
		if (!formula.isOptimalityOperator()) {
			LOG4CPLUS_ERROR(logger, "Formula does not specify neither min nor max optimality, which is not meaningful over nondeterministic models.");
			throw storm::exceptions::InvalidArgumentException() << "Formula does not specify neither min nor max optimality, which is not meaningful over nondeterministic models.";
		}
		minimumOperatorStack.push(formula.isMinimumOperator());
		std::vector<Type>* result = formula.getPathFormula().check(*this, false);
		minimumOperatorStack.pop();
		return result;
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
		tmpMatrix.makeRowsAbsorbing(~(*leftStates | *rightStates) | *rightStates, *this->getModel().getNondeterministicChoiceIndices());

		// Create the vector with which to multiply.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());
		storm::utility::setVectorValues(result, *rightStates, storm::utility::constGetOne<Type>());

		this->performMatrixVectorMultiplication(*this->getModel().getTransitionMatrix(), *result, nullptr, formula.getBound());

		// Delete intermediate results and return result.
		delete leftStates;
		delete rightStates;
		return result;
	}

	/*!
	 * The check method for a path formula with a Next operator node as root in its formula tree
	 *
	 * @param formula The Next path formula to check
	 * @returns for each state the probability that the path formula holds.
	 */
	virtual std::vector<Type>* checkNext(const storm::formula::Next<Type>& formula, bool qualitative) const {
		// First, we need to compute the states that satisfy the sub-formula of the next-formula.
		storm::storage::BitVector* nextStates = formula.getChild().check(*this);

		// Create the vector with which to multiply and initialize it correctly.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());
		storm::utility::setVectorValues(result, *nextStates, storm::utility::constGetOne<Type>());

		// Delete obsolete sub-result.
		delete nextStates;

		this->performMatrixVectorMultiplication(*this->getModel().getTransitionMatrix(), *result);

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

			// Create vector for results for maybe states.
			std::vector<Type> x(maybeStatesSetBitCount);

			// Prepare the right-hand side of the equation system. For entry i this corresponds to
			// the accumulated probability of going from state i to some 'yes' state.
			std::vector<Type> b(submatrix->getRowCount());
			this->getModel().getTransitionMatrix()->getConstrainedRowSumVector(maybeStates, *this->getModel().getNondeterministicChoiceIndices(), statesWithProbability1, &b);

			// Solve the corresponding system of equations.
			this->solveEquationSystem(*submatrix, x, b, *subNondeterministicChoiceIndices);

			// Set values of resulting vector according to result.
			storm::utility::setVectorValues<Type>(result, maybeStates, x);

			// Delete temporary matrix.
			delete submatrix;
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

		this->performMatrixVectorMultiplication(*this->getModel().getTransitionMatrix(), *result, nullptr, formula.getBound());

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

		this->performMatrixVectorMultiplication(*this->getModel().getTransitionMatrix(), *result, totalRewardVector, formula.getBound());

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
		if (this->minimumOperatorStack.top()) {
			storm::utility::GraphAnalyzer::performProb1A(this->getModel(), trueStates, *targetStates, &infinityStates);
		} else {
			storm::utility::GraphAnalyzer::performProb1E(this->getModel(), trueStates, *targetStates, &infinityStates);
		}
		infinityStates.complement();

		LOG4CPLUS_INFO(logger, "Found " << infinityStates.getNumberOfSetBits() << " 'infinity' states.");
		LOG4CPLUS_INFO(logger, "Found " << targetStates->getNumberOfSetBits() << " 'target' states.");
		storm::storage::BitVector maybeStates = ~(*targetStates) & ~infinityStates;
		LOG4CPLUS_INFO(logger, "Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");

		// Create resulting vector.
		std::vector<Type>* result = new std::vector<Type>(this->getModel().getNumberOfStates());

		// Check whether there are states for which we have to compute the result.
		const int maybeStatesSetBitCount = maybeStates.getNumberOfSetBits();
		if (maybeStatesSetBitCount > 0) {
			// First, we can eliminate the rows and columns from the original transition probability matrix for states
			// whose probabilities are already known.
			storm::storage::SparseMatrix<Type>* submatrix = this->getModel().getTransitionMatrix()->getSubmatrix(maybeStates, *this->getModel().getNondeterministicChoiceIndices());

			// Get the "new" nondeterministic choice indices for the submatrix.
			std::shared_ptr<std::vector<uint_fast64_t>> subNondeterministicChoiceIndices = this->computeNondeterministicChoiceIndicesForConstraint(maybeStates);

			// Create vector for results for maybe states.
			std::vector<Type> x(maybeStatesSetBitCount);

			// Prepare the right-hand side of the equation system. For entry i this corresponds to
			// the accumulated probability of going from state i to some 'yes' state.
			std::vector<Type> b(submatrix->getRowCount());

			if (this->getModel().hasTransitionRewards()) {
				// If a transition-based reward model is available, we initialize the right-hand
				// side to the vector resulting from summing the rows of the pointwise product
				// of the transition probability matrix and the transition reward matrix.
				std::vector<Type>* pointwiseProductRowSumVector = this->getModel().getTransitionMatrix()->getPointwiseProductRowSumVector(*this->getModel().getTransitionRewardMatrix());
				storm::utility::selectVectorValues(&b, maybeStates, *this->getModel().getNondeterministicChoiceIndices(), *pointwiseProductRowSumVector);
				delete pointwiseProductRowSumVector;

				if (this->getModel().hasStateRewards()) {
					// If a state-based reward model is also available, we need to add this vector
					// as well. As the state reward vector contains entries not just for the states
					// that we still consider (i.e. maybeStates), we need to extract these values
					// first.
					std::vector<Type>* subStateRewards = new std::vector<Type>(b.size());
					storm::utility::selectVectorValuesRepeatedly(subStateRewards, maybeStates, *this->getModel().getNondeterministicChoiceIndices(), *this->getModel().getStateRewardVector());
					gmm::add(*subStateRewards, b);
					delete subStateRewards;
				}
			} else {
				// If only a state-based reward model is  available, we take this vector as the
				// right-hand side. As the state reward vector contains entries not just for the
				// states that we still consider (i.e. maybeStates), we need to extract these values
				// first.
				storm::utility::selectVectorValuesRepeatedly(&b, maybeStates, *this->getModel().getNondeterministicChoiceIndices(), *this->getModel().getStateRewardVector());
			}

			// Solve the corresponding system of equations.
			this->solveEquationSystem(*submatrix, x, b, *subNondeterministicChoiceIndices);

			// Set values of resulting vector according to result.
			storm::utility::setVectorValues<Type>(result, maybeStates, x);
			delete submatrix;
		}

		// Set values of resulting vector that are known exactly.
		storm::utility::setVectorValues(result, *targetStates, storm::utility::constGetZero<Type>());
		storm::utility::setVectorValues(result, infinityStates, storm::utility::constGetInfinity<Type>());

		// Delete temporary storages and return result.
		delete targetStates;
		return result;
	}

protected:
	mutable std::stack<bool> minimumOperatorStack;

private:
	virtual void performMatrixVectorMultiplication(storm::storage::SparseMatrix<Type> const& matrix, std::vector<Type>& vector, std::vector<Type>* summand = nullptr, uint_fast64_t repetitions = 1) const {
		// Get the starting row indices for the non-deterministic choices to reduce the resulting
		// vector properly.
		std::shared_ptr<std::vector<uint_fast64_t>> nondeterministicChoiceIndices = this->getModel().getNondeterministicChoiceIndices();

		// Create vector for result of multiplication, which is reduced to the result vector after
		// each multiplication.
		std::vector<Type> multiplyResult(matrix.getRowCount());

		// Now perform matrix-vector multiplication as long as we meet the bound of the formula.
		for (uint_fast64_t i = 0; i < repetitions; ++i) {
			matrix.multiplyWithVector(vector, multiplyResult);
			if (summand != nullptr) {
				gmm::add(*summand, multiplyResult);
			}
			if (this->minimumOperatorStack.top()) {
				storm::utility::reduceVectorMin(multiplyResult, &vector, *nondeterministicChoiceIndices);
			} else {
				storm::utility::reduceVectorMax(multiplyResult, &vector, *nondeterministicChoiceIndices);
			}
		}
	}

	virtual void solveEquationSystem(storm::storage::SparseMatrix<Type> const& matrix, std::vector<Type>& x, std::vector<Type> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices) const {
		// Get the settings object to customize solving.
		storm::settings::Settings* s = storm::settings::instance();

		// Get relevant user-defined settings for solving the equations.
		double precision = s->get<double>("precision");
		unsigned maxIterations = s->get<unsigned>("maxiter");
		bool relative = s->get<bool>("relative");

		// Set up the environment for the power method.
		std::vector<Type> multiplyResult(matrix.getRowCount());
		std::vector<Type>* currentX = &x;
		std::vector<Type>* newX = new std::vector<Type>(x.size());
		std::vector<Type>* swap = nullptr;
		uint_fast64_t iterations = 0;
		bool converged = false;

		// Proceed with the iterations as long as the method did not converge or reach the
		// user-specified maximum number of iterations.
		while (!converged && iterations < maxIterations) {
			// Compute x' = A*x + b.
			matrix.multiplyWithVector(*currentX, multiplyResult);

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

		if (iterations % 2 == 1) {
			std::swap(x, *currentX);
			delete currentX;
		} else {
			delete newX;
		}

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

#endif /* STORM_MODELCHECKER_DTMCPRCTLMODELCHECKER_H_ */
