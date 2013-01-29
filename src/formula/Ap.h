/*
 * Ap.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_AP_H_
#define STORM_FORMULA_AP_H_

#include "src/formula/AbstractStateFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include "src/modelChecker/AbstractModelChecker.h"

namespace storm {

namespace formula {

template <class T> class Ap;

template <class T>
class IApModelChecker {
    public:
        virtual storm::storage::BitVector* checkAp(const Ap<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for a Abstract formula tree with atomic proposition as root.
 *
 * This class represents the leaves in the formula tree.
 *
 * @see AbstractStateFormula
 * @see AbstractFormula
 */
template <class T>
class Ap : public AbstractStateFormula<T> {

public:
	/*!
	 * Constructor
	 *
	 * Creates a new atomic proposition leaf, with the label Ap
	 *
	 * @param ap The string representing the atomic proposition
	 */
	Ap(std::string ap) {
		this->ap = ap;
	}

	/*!
	 * Destructor.
	 * At this time, empty...
	 */
	virtual ~Ap() { }

	/*!
	 * @returns the name of the atomic proposition
	 */
	const std::string& getAp() const {
		return ap;
	}

	/*!
	 * @returns a string representation of the leaf.
	 *
	 */
	virtual std::string toString() const {
		return getAp();
	}

	/*!
	 * Clones the called object.
	 *
	 * @returns a new Ap-object that is identical the called object.
	 */
	virtual AbstractStateFormula<T>* clone() const {
	  return new Ap(ap);
	}

	/*!
	 * Calls the model checker to check this formula.
	 * Needed to infer the correct type of formula class.
	 *
	 * @note This function should only be called in a generic check function of a model checker class. For other uses,
	 *       the methods of the model checker should be used.
	 *
	 * @returns A bit vector indicating all states that satisfy the formula represented by the called object.
	 */
	virtual storm::storage::BitVector *check(const storm::modelChecker::AbstractModelChecker<T>& modelChecker) const {
		return modelChecker.template as<IApModelChecker>()->checkAp(*this);
	}
	
	virtual bool conforms(const AbstractFormulaChecker<T>& checker) const {
		return true;
	}

private:
	std::string ap;
};

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_AP_H_ */
