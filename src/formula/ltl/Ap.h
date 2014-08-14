/*
 * Ap.h
 *
 *  Created on: 22.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_LTL_AP_H_
#define STORM_FORMULA_LTL_AP_H_

#include "AbstractLtlFormula.h"

namespace storm {
namespace property {
namespace ltl {

template <class T> class Ap;

/*!
 *	@brief Interface class for model checkers that support And.
 *
 *	All model checkers that support the formula class And must inherit
 *	this pure virtual class.
 */
template <class T>
class IApModelChecker {
	public:
		/*!
		 *	@brief Evaluates And formula within a model checker.
		 *
		 *	@param obj Formula object with subformulas.
		 *	@return Result of the formula for every node.
		 */
		virtual std::vector<T> checkAp(const Ap<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for an abstract formula tree with atomic proposition as root.
 *
 * This class represents the leaves in the formula tree.
 *
 * @see AbstractLtlFormula
 */
template <class T>
class Ap: public storm::property::ltl::AbstractLtlFormula<T> {
public:
	/*!
	 * Empty constructor
	 */
	Ap() {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * Creates a new atomic proposition leaf, with the label Ap
	 *
	 * @param ap The string representing the atomic proposition
	 */
	Ap(std::string ap) : ap(ap) {
		// Intentionally left empty.
	}

	/*!
	 * Destructor
	 * At this time, empty...
	 */
	virtual ~Ap() {
		// Intentionally left empty
	}

	/*!
	 * Calls the model checker to check this formula.
	 * Needed to infer the correct type of formula class.
	 *
	 * @note This function should only be called in a generic check function of a model checker class. For other uses,
	 *       the methods of the model checker should be used.
	 *
	 * @note This function is not implemented in this class.
	 *
	 * @returns A vector indicating the probability that the formula holds for each state.
	 */
	virtual std::vector<T> check(const storm::modelchecker::ltl::AbstractModelChecker<T>& modelChecker) const override {
		return modelChecker.template as<IApModelChecker>()->checkAp(*this);
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual std::shared_ptr<AbstractLtlFormula<T>> clone() const override {
		std::shared_ptr<AbstractLtlFormula<T>> result(new Ap(this->getAp()));
		return result;
	}

	/*!
	 * @returns a string representation of the leaf.
	 *
	 */
	virtual std::string toString() const override {
		return getAp();
	}

	/*!
	 * Returns whether the formula is a propositional logic formula.
	 * That is, this formula and all its subformulas consist only of And, Or, Not and AP.
	 *
	 * @return True iff this is a propositional logic formula.
	 */
	virtual bool isPropositional() const override {
		return true;
	}

	/*!
	 * @returns the name of the atomic proposition
	 */
	std::string const & getAp() const {
		return ap;
	}

private:
	std::string ap;
};

} /* namespace ltl */
} /* namespace property */
} /* namespace storm */
#endif /* STORM_FORMULA_LTL_AP_H_ */
