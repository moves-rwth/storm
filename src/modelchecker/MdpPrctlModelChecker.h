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
	public virtual AbstractModelChecker<Type> {
public:
	/*!
	 * Constructor
	 *
	 * @param model The dtmc model which is checked.
	 */
	explicit MdpPrctlModelChecker(storm::models::Mdp<Type>& model) : model(model), minimumOperatorStack() {

	}

	/*!
	 * Copy constructor
	 *
	 * @param modelChecker The model checker that is copied.
	 */
	explicit MdpPrctlModelChecker(const storm::modelChecker::MdpPrctlModelChecker<Type>* modelChecker) : model(new storm::models::Mdp<Type>(modelChecker->getModel())),  minimumOperatorStack() {

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
		return this->model;
	}

	/*!
	 * Sets the DTMC model which is checked
	 * @param model
	 */
	void setModel(storm::models::Mdp<Type>& model) {
		this->model = &model;
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
	virtual std::vector<Type>* checkBoundedUntil(const storm::formula::BoundedUntil<Type>& formula, bool qualitative) const = 0;

	/*!
	 * The check method for a path formula with a Next operator node as root in its formula tree
	 *
	 * @param formula The Next path formula to check
	 * @returns for each state the probability that the path formula holds.
	 */
	virtual std::vector<Type>* checkNext(const storm::formula::Next<Type>& formula, bool qualitative) const = 0;

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
	virtual std::vector<Type>* checkUntil(const storm::formula::Until<Type>& formula, bool qualitative) const = 0;

	/*!
	 * The check method for a path formula with an Instantaneous Reward operator node as root in its
	 * formula tree
	 *
	 * @param formula The Instantaneous Reward formula to check
	 * @returns for each state the reward that the instantaneous reward yields
	 */
	virtual std::vector<Type>* checkInstantaneousReward(const storm::formula::InstantaneousReward<Type>& formula, bool qualitative) const = 0;

	/*!
	 * The check method for a path formula with a Cumulative Reward operator node as root in its
	 * formula tree
	 *
	 * @param formula The Cumulative Reward formula to check
	 * @returns for each state the reward that the cumulative reward yields
	 */
	virtual std::vector<Type>* checkCumulativeReward(const storm::formula::CumulativeReward<Type>& formula, bool qualitative) const = 0;

	/*!
	 * The check method for a path formula with a Reachability Reward operator node as root in its
	 * formula tree
	 *
	 * @param formula The Reachbility Reward formula to check
	 * @returns for each state the reward that the reachability reward yields
	 */
	virtual std::vector<Type>* checkReachabilityReward(const storm::formula::ReachabilityReward<Type>& formula, bool qualitative) const = 0;

private:
	storm::models::Mdp<Type>& model;

protected:
	mutable std::stack<bool> minimumOperatorStack;
};

} //namespace modelChecker

} //namespace storm

#endif /* STORM_MODELCHECKER_DTMCPRCTLMODELCHECKER_H_ */
