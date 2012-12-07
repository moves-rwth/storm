/*
 * DtmcPrctlModelChecker.h
 *
 *  Created on: 22.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef DTMCPRCTLMODELCHECKER_H_
#define DTMCPRCTLMODELCHECKER_H_

namespace mrmc {

namespace modelChecker {

/* The formula classes need to reference a model checker for the check function,
 * which is used to infer the correct type of formula,
 * so the model checker class is declared here already.
 *
 */
template <class T>
class DtmcPrctlModelChecker;
}

}

#include "src/formula/PCTLPathFormula.h"
#include "src/formula/PCTLStateFormula.h"

#include "src/formula/Formulas.h"

#include "src/models/Dtmc.h"
#include "src/storage/BitVector.h"
#include <vector>

namespace mrmc {

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
template<class T>
class DtmcPrctlModelChecker {
public:
	/*!
	 * Constructor
	 *
	 * @param model The dtmc model which is checked.
	 */
	explicit DtmcPrctlModelChecker(mrmc::models::Dtmc<T>& model) {
	   this->model = &model;
	}

	/*!
	 * Copy constructor
	 *
	 * @param modelChecker The model checker that is copied.
	 */
	explicit DtmcPrctlModelChecker(const mrmc::modelChecker::DtmcPrctlModelChecker<T>* modelChecker) {
		this->model = new mrmc::models::Dtmc<T>(modelChecker->getModel());
	}

	/*!
	 * Destructor
	 */
	virtual ~DtmcPrctlModelChecker() {
		//should not be deleted here, according to ticket #24
	   //delete this->dtmc;
	}

	/*!
	 * @returns A reference to the dtmc of the model checker.
	 */
	mrmc::models::Dtmc<T>& getModel() const {
		return *(this->model);
	}

	/*!
	 * Sets the DTMC model which is checked
	 * @param model
	 */
	void setModel(mrmc::models::Dtmc<T>& model) {
		this->model = &model;
	}

	/*!
	 * The check method for a state formula; Will infer the actual type of formula and delegate it
	 * to the specialized method
	 *
	 * @param formula The state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	virtual mrmc::storage::BitVector* checkStateFormula(const mrmc::formula::PCTLStateFormula<T>& formula) const {
		return formula.check(*this);
	}

	/*!
	 * The check method for a state formula with an And node as root in its formula tree
	 *
	 * @param formula The And formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	virtual mrmc::storage::BitVector* checkAnd(const mrmc::formula::And<T>& formula) const {
		mrmc::storage::BitVector* result = checkStateFormula(formula.getLeft());
		mrmc::storage::BitVector* right = checkStateFormula(formula.getRight());
		(*result) &= (*right);
		delete right;
		return result;
	}

	/*!
	 * The check method for a formula with an AP node as root in its formula tree
	 *
	 * @param formula The AP state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	virtual mrmc::storage::BitVector* checkAP(const mrmc::formula::AP<T>& formula) const {
		if (formula.getAP().compare("true") == 0) {
			return new mrmc::storage::BitVector(model->getNumberOfStates(), 1);
		} else if (formula.getAP().compare("false") == 0) {
			return new mrmc::storage::BitVector(model->getNumberOfStates());
		}
		return new mrmc::storage::BitVector(*model->getLabeledStates(formula.getAP()));
	}

	/*!
	 * The check method for a formula with a Not node as root in its formula tree
	 *
	 * @param formula The Not state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	virtual mrmc::storage::BitVector* checkNot(const mrmc::formula::Not<T>& formula) const {
		mrmc::storage::BitVector* result = checkStateFormula(formula.getChild());
		result->complement();
		return result;
	}

	/*!
	 * The check method for a state formula with an Or node as root in its formula tree
	 *
	 * @param formula The Or state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	virtual mrmc::storage::BitVector* checkOr(const mrmc::formula::Or<T>& formula) const {
		mrmc::storage::BitVector* result = checkStateFormula(formula.getLeft());
		mrmc::storage::BitVector* right = checkStateFormula(formula.getRight());
		(*result) |= (*right);
		delete right;
		return result;
	}

	/*!
	 * The check method for a state formula with a probabilistic operator node as root in its
	 * formula tree
	 *
	 * @param formula The state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	virtual mrmc::storage::BitVector* checkProbabilisticOperator(
			const mrmc::formula::ProbabilisticOperator<T>& formula) const = 0;

	/*!
	 * The check method for a state formula with a probabilistic interval operator node as root in
	 * its formula tree
	 *
	 * @param formula The state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	virtual mrmc::storage::BitVector* checkProbabilisticIntervalOperator(
			const mrmc::formula::ProbabilisticIntervalOperator<T>& formula) const = 0;

	/*!
	 * The check method for a path formula; Will infer the actual type of formula and delegate it
	 * to the specialized method
	 *
	 * @param formula The path formula to check
	 * @returns for each state the probability that the path formula holds.
	 */
	virtual std::vector<T>* checkPathFormula(const mrmc::formula::PCTLPathFormula<T>& formula) const {
		return formula.check(*this);
	}

	/*!
	 * The check method for a path formula with a Bounded Until operator node as root in its formula tree
	 *
	 * @param formula The Bounded Until path formula to check
	 * @returns for each state the probability that the path formula holds.
	 */
	virtual std::vector<T>* checkBoundedUntil(const mrmc::formula::BoundedUntil<T>& formula) const = 0;

	/*!
	 * The check method for a path formula with a Next operator node as root in its formula tree
	 *
	 * @param formula The Next path formula to check
	 * @returns for each state the probability that the path formula holds.
	 */
	virtual std::vector<T>* checkNext(const mrmc::formula::Next<T>& formula) const = 0;

	/*!
	 * The check method for a path formula with an Until operator node as root in its formula tree
	 *
	 * @param formula The Until path formula to check
	 * @returns for each state the probability that the path formula holds.
	 */
	virtual std::vector<T>* checkUntil(const mrmc::formula::Until<T>& formula) const = 0;

private:
	mrmc::models::Dtmc<T>* model;
};

} //namespace modelChecker

} //namespace mrmc

#endif /* DTMCPRCTLMODELCHECKER_H_ */
