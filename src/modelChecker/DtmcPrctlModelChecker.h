/*
 * DtmcPrctlModelChecker.h
 *
 *  Created on: 22.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_MODELCHECKER_DTMCPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_DTMCPRCTLMODELCHECKER_H_

namespace storm {

namespace modelChecker {

/* The formula classes need to reference a model checker for the check function,
 * which is used to infer the correct type of formula,
 * so the model checker class is declared here already.
 *
 */
template <class Type>
class DtmcPrctlModelChecker;
}

}

#include "src/formula/PctlPathFormula.h"
#include "src/formula/PctlStateFormula.h"

#include "src/formula/Formulas.h"

#include "src/models/Dtmc.h"
#include "src/storage/BitVector.h"
#include <vector>

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
class DtmcPrctlModelChecker {
public:
	/*!
	 * Constructor
	 *
	 * @param model The dtmc model which is checked.
	 */
	explicit DtmcPrctlModelChecker(storm::models::Dtmc<Type>& model) : model(model) {

	}

	/*!
	 * Copy constructor
	 *
	 * @param modelChecker The model checker that is copied.
	 */
	explicit DtmcPrctlModelChecker(const storm::modelChecker::DtmcPrctlModelChecker<Type>* modelChecker) {
		this->model = new storm::models::Dtmc<Type>(modelChecker->getModel());
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
		return this->model;
	}

	/*!
	 * Sets the DTMC model which is checked
	 * @param model
	 */
	void setModel(storm::models::Dtmc<Type>& model) {
		this->model = &model;
	}

	/*!
	 * Checks the given state formula on the DTMC and prints the result (true/false) for all initial
	 * states.
	 * @param stateFormula The formula to be checked.
	 */
	void check(const storm::formula::PctlStateFormula<Type>& stateFormula) const {
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << stateFormula.toString());
		std::cout << "Model checking formula:\t" << stateFormula.toString() << std::endl;
		storm::storage::BitVector* result = stateFormula.check(*this);
		LOG4CPLUS_INFO(logger, "Result for initial states:");
		std::cout << "Result for initial states:" << std::endl;
		for (auto initialState : *this->getModel().getLabeledStates("init")) {
			LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << (result->get(initialState) ? "satisfied" : "not satisfied"));
			std::cout << "\t" << initialState << ": " << (*result)[initialState] << std::endl;
		}
		std::cout << std::endl;
		storm::utility::printSeparationLine(std::cout);
		delete result;
	}

	/*!
	 * Checks the given probabilistic operator (with no bound) on the DTMC and prints the result
	 * (probability) for all initial states.
	 * @param probabilisticNoBoundsFormula The formula to be checked.
	 */
	void check(const storm::formula::ProbabilisticNoBoundsOperator<Type>& probabilisticNoBoundsFormula) const {
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << probabilisticNoBoundsFormula.toString());
		std::cout << "Model checking formula:\t" << probabilisticNoBoundsFormula.toString() << std::endl;
		std::vector<Type>* result = checkProbabilisticNoBoundsOperator(probabilisticNoBoundsFormula);
		LOG4CPLUS_INFO(logger, "Result for initial states:");
		std::cout << "Result for initial states:" << std::endl;
		for (auto initialState : *this->getModel().getLabeledStates("init")) {
			LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << (*result)[initialState]);
			std::cout << "\t" << initialState << ": " << (*result)[initialState] << std::endl;
		}
		std::cout << std::endl;
		storm::utility::printSeparationLine(std::cout);
		delete result;
	}

	/*!
	 * Checks the given reward operator (with no bound) on the DTMC and prints the result
	 * (reward values) for all initial states.
	 * @param rewardNoBoundsFormula The formula to be checked.
	 */
	void check(const storm::formula::RewardNoBoundsOperator<Type>& rewardNoBoundsFormula) {
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << rewardNoBoundsFormula.toString());
		std::cout << "Model checking formula:\t" << rewardNoBoundsFormula.toString() << std::endl;
		std::vector<Type>* result = checkRewardNoBoundsOperator(rewardNoBoundsFormula);
		LOG4CPLUS_INFO(logger, "Result for initial states:");
		std::cout << "Result for initial states:" << std::endl;
		for (auto initialState : *this->getModel().getLabeledStates("init")) {
			LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << (*result)[initialState]);
			std::cout << "\t" << initialState << ": " << (*result)[initialState] << std::endl;
		}
		std::cout << std::endl;
		storm::utility::printSeparationLine(std::cout);
		delete result;
	}

	/*!
	 * The check method for a state formula; Will infer the actual type of formula and delegate it
	 * to the specialized method
	 *
	 * @param formula The state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	storm::storage::BitVector* checkStateFormula(const storm::formula::PctlStateFormula<Type>& formula) const {
		return formula.check(*this);
	}

	/*!
	 * The check method for a state formula with an And node as root in its formula tree
	 *
	 * @param formula The And formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	storm::storage::BitVector* checkAnd(const storm::formula::And<Type>& formula) const {
		storm::storage::BitVector* result = checkStateFormula(formula.getLeft());
		storm::storage::BitVector* right = checkStateFormula(formula.getRight());
		(*result) &= (*right);
		delete right;
		return result;
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
		return new storm::storage::BitVector(*this->getModel().getLabeledStates(formula.getAp()));
	}

	/*!
	 * The check method for a formula with a Not node as root in its formula tree
	 *
	 * @param formula The Not state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	storm::storage::BitVector* checkNot(const storm::formula::Not<Type>& formula) const {
		storm::storage::BitVector* result = checkStateFormula(formula.getChild());
		result->complement();
		return result;
	}

	/*!
	 * The check method for a state formula with an Or node as root in its formula tree
	 *
	 * @param formula The Or state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	storm::storage::BitVector* checkOr(const storm::formula::Or<Type>& formula) const {
		storm::storage::BitVector* result = checkStateFormula(formula.getLeft());
		storm::storage::BitVector* right = checkStateFormula(formula.getRight());
		(*result) |= (*right);
		delete right;
		return result;
	}

	/*!
	 * The check method for a state formula with a probabilistic interval operator node as root in
	 * its formula tree
	 *
	 * @param formula The state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	storm::storage::BitVector* checkProbabilisticIntervalOperator(
			const storm::formula::ProbabilisticIntervalOperator<Type>& formula) const {
		// First, we need to compute the probability for satisfying the path formula for each state.
		std::vector<Type>* probabilisticResult = this->checkPathFormula(formula.getPathFormula());

		// Create resulting bit vector, which will hold the yes/no-answer for every state.
		storm::storage::BitVector* result = new storm::storage::BitVector(this->getModel().getNumberOfStates());

		// Now, we can compute which states meet the bound specified in this operator, i.e.
		// lie in the interval that was given along with this operator, and set the corresponding bits
		// to true in the resulting vector.
		Type lower = formula.getLowerBound();
		Type upper = formula.getUpperBound();
		for (uint_fast64_t i = 0; i < this->getModel().getNumberOfStates(); ++i) {
			if ((*probabilisticResult)[i] >= lower && (*probabilisticResult)[i] <= upper) result->set(i, true);
		}

		// Delete the probabilities computed for the states and return result.
		delete probabilisticResult;
		return result;
	}

	/*!
	 * The check method for a state formula with a reward interval operator node as root in
	 * its formula tree
	 *
	 * @param formula The state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	storm::storage::BitVector* checkRewardIntervalOperator(
			const storm::formula::RewardIntervalOperator<Type>& formula) const {
		// First, we need to compute the probability for satisfying the path formula for each state.
		std::vector<Type>* rewardResult = this->checkPathFormula(formula.getPathFormula());

		// Create resulting bit vector, which will hold the yes/no-answer for every state.
		storm::storage::BitVector* result = new storm::storage::BitVector(this->getModel().getNumberOfStates());

		// Now, we can compute which states meet the bound specified in this operator, i.e.
		// lie in the interval that was given along with this operator, and set the corresponding bits
		// to true in the resulting vector.
		Type lower = formula.getLowerBound();
		Type upper = formula.getUpperBound();
		for (uint_fast64_t i = 0; i < this->getModel().getNumberOfStates(); ++i) {
			if ((*rewardResult)[i] >= lower && (*rewardResult)[i] <= upper) result->set(i, true);
		}

		// Delete the reward values computed for the states and return result.
		delete rewardResult;
		return result;
	}

	/*!
	 * The check method for a state formula with a probabilistic operator node without bounds as root
	 * in its formula tree
	 *
	 * @param formula The state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	std::vector<Type>* checkProbabilisticNoBoundsOperator(
			const storm::formula::ProbabilisticNoBoundsOperator<Type>& formula) const {
		return formula.getPathFormula().check(*this);
	}

	/*!
	 * The check method for a state formula with a reward operator node without bounds as root
	 * in its formula tree
	 *
	 * @param formula The state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	std::vector<Type>* checkRewardNoBoundsOperator(
			const storm::formula::RewardNoBoundsOperator<Type>& formula) const {
		return formula.getPathFormula().check(*this);
	}

	/*!
	 * The check method for a path formula; Will infer the actual type of formula and delegate it
	 * to the specialized method
	 *
	 * @param formula The path formula to check
	 * @returns for each state the probability that the path formula holds.
	 */
	std::vector<Type>* checkPathFormula(const storm::formula::PctlPathFormula<Type>& formula) const {
		return formula.check(*this);
	}

	/*!
	 * The check method for a path formula with a Bounded Until operator node as root in its formula tree
	 *
	 * @param formula The Bounded Until path formula to check
	 * @returns for each state the probability that the path formula holds.
	 */
	virtual std::vector<Type>* checkBoundedUntil(const storm::formula::BoundedUntil<Type>& formula) const = 0;

	/*!
	 * The check method for a path formula with a Next operator node as root in its formula tree
	 *
	 * @param formula The Next path formula to check
	 * @returns for each state the probability that the path formula holds.
	 */
	virtual std::vector<Type>* checkNext(const storm::formula::Next<Type>& formula) const = 0;

	/*!
	 * The check method for a path formula with an Eventually operator node as root in its formula tree
	 *
	 * @param formula The Eventually path formula to check
	 * @returns for each state the probability that the path formula holds
	 */
	virtual std::vector<Type>* checkEventually(const storm::formula::Eventually<Type>& formula) const {
		// Create equivalent temporary until formula and check it.
		storm::formula::Until<Type> temporaryUntilFormula(new storm::formula::Ap<Type>("true"), formula.getChild().clone());
		std::vector<Type>* result = this->checkUntil(temporaryUntilFormula);
		return result;
	}

	/*!
	 * The check method for a path formula with a Globally operator node as root in its formula tree
	 *
	 * @param formula The Globally path formula to check
	 * @returns for each state the probability that the path formula holds
	 */
	virtual std::vector<Type>* checkGlobally(const storm::formula::Globally<Type>& formula) const {
		// Create "equivalent" temporary eventually formula and check it.
		storm::formula::Eventually<Type> temporaryEventuallyFormula(new storm::formula::Not<Type>(formula.getChild().clone()));
		std::vector<Type>* result = this->checkEventually(temporaryEventuallyFormula);

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
	virtual std::vector<Type>* checkUntil(const storm::formula::Until<Type>& formula) const = 0;

	/*!
	 * The check method for a path formula with an Instantaneous Reward operator node as root in its
	 * formula tree
	 *
	 * @param formula The Instantaneous Reward formula to check
	 * @returns for each state the reward that the instantaneous reward yields
	 */
	virtual std::vector<Type>* checkInstantaneousReward(const storm::formula::InstantaneousReward<Type>& formula) const = 0;

	/*!
	 * The check method for a path formula with a Cumulative Reward operator node as root in its
	 * formula tree
	 *
	 * @param formula The Cumulative Reward formula to check
	 * @returns for each state the reward that the cumulative reward yields
	 */
	virtual std::vector<Type>* checkCumulativeReward(const storm::formula::CumulativeReward<Type>& formula) const = 0;

	/*!
	 * The check method for a path formula with a Reachability Reward operator node as root in its
	 * formula tree
	 *
	 * @param formula The Reachbility Reward formula to check
	 * @returns for each state the reward that the reachability reward yields
	 */
	virtual std::vector<Type>* checkReachabilityReward(const storm::formula::ReachabilityReward<Type>& formula) const = 0;

private:
	storm::models::Dtmc<Type>& model;
};

} //namespace modelChecker

} //namespace storm

#endif /* STORM_MODELCHECKER_DTMCPRCTLMODELCHECKER_H_ */
