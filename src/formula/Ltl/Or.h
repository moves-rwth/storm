/*
 * Or.h
 *
 *  Created on: 22.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_LTL_OR_H_
#define STORM_FORMULA_LTL_OR_H_

#include "AbstractLtlFormula.h"
#include "src/formula/abstract/Or.h"

namespace storm {
namespace property {
namespace ltl {

template <class T> class Or;

/*!
 *	@brief Interface class for model checkers that support And.
 *
 *	All model checkers that support the formula class And must inherit
 *	this pure virtual class.
 */
template <class T>
class IOrModelChecker {
	public:
		/*!
		 *	@brief Evaluates And formula within a model checker.
		 *
		 *	@param obj Formula object with subformulas.
		 *	@return Result of the formula for every node.
		 */
		virtual std::vector<T>* checkOr(const Or<T>& obj) const = 0;
};

/*!
 *	@brief Interface class for visitors that support Or.
 *
 *	All visitors that support the formula class Or must inherit
 *	this pure virtual class.
 */
template <class T>
class IOrVisitor {
	public:
		/*!
		 *	@brief Visits Or formula.
		 *
		 *	@param obj Formula object with subformulas.
		 *	@return Result of the formula for every node.
		 */
		virtual void visitOr(const Or<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for an abstract formula tree with OR node as root.
 *
 * Has two LTL formulas as sub formulas/trees.
 *
 * As OR is commutative, the order is \e theoretically not important, but will influence the order in which
 * the model checker works.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractLtlFormula
 */
template <class T>
class Or: public storm::property::abstract::Or<T, AbstractLtlFormula<T>>,
			 public storm::property::ltl::AbstractLtlFormula<T> {
public:
	/*!
	 * Empty constructor
	 */
	Or() {
		// Intentionally left empty

	}

	/*!
	 * Constructor
	 * Creates an OR node with the parameters as subtrees.
	 *
	 * @param left The left subformula
	 * @param right The right subformula
	 */
	Or(AbstractLtlFormula<T>* left, AbstractLtlFormula<T>* right)
		: storm::property::abstract::Or<T,AbstractLtlFormula<T>>(left, right) {
		// Intentionally left empty
	}

	/*!
	 * Destructor
	 */
	virtual ~Or() {
		// Intentionally left empty
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual AbstractLtlFormula<T>* clone() const {
		Or<T>* result = new Or();
		if (this->leftIsSet()) {
		  result->setLeft(this->getLeft().clone());
		}
		if (this->rightIsSet()) {
		  result->setRight(this->getRight().clone());
		}
		return result;
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
	virtual std::vector<T>* check(const storm::modelchecker::ltl::AbstractModelChecker<T>& modelChecker) const {
		return modelChecker.template as<IOrModelChecker>()->checkOr(*this);
	}

	virtual void visit(const visitor::AbstractLtlFormulaVisitor<T>& visitor) const {
		visitor.template as<IOrVisitor>()->visitOr(*this);
	}

};

} /* namespace ltl */
} /* namespace property */
} /* namespace storm */
#endif /* OR_H_ */
