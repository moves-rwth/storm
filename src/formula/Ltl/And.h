/*
 * And.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_LTL_AND_H_
#define STORM_FORMULA_LTL_AND_H_

#include "AbstractLtlFormula.h"
#include "src/formula/abstract/And.h"
#include "src/formula/AbstractFormulaChecker.h"
#include "src/modelchecker/ltl/ForwardDeclarations.h"
#include <string>

namespace storm {
namespace property {
namespace ltl {

template <class T> class And;

/*!
 *	@brief Interface class for model checkers that support And.
 *
 *	All model checkers that support the formula class And must inherit
 *	this pure virtual class.
 */
template <class T>
class IAndModelChecker {
	public:
		/*!
		 *	@brief Evaluates And formula within a model checker.
		 *
		 *	@param obj Formula object with subformulas.
		 *	@return Result of the formula for every node.
		 */
		virtual std::vector<T>* checkAnd(const And<T>& obj) const = 0;
};

/*!
 *	@brief Interface class for visitors that support And.
 *
 *	All visitors that support the formula class And must inherit
 *	this pure virtual class.
 */
template <class T>
class IAndVisitor {
	public:
		/*!
		 *	@brief Evaluates And formula within a model checker.
		 *
		 *	@param obj Formula object with subformulas.
		 *	@return Result of the formula for every node.
		 */
		virtual void visitAnd(const And<T>& obj) = 0;
};

/*!
 * @brief
 * Class for an abstract formula tree with AND node as root.
 *
 * Has two Abstract LTL formulas as sub formulas/trees.
 *
 * As AND is commutative, the order is \e theoretically not important, but will influence the order in which
 * the model checker works.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractLtlFormula
 */
template <class T>
class And : public storm::property::abstract::And<T, AbstractLtlFormula<T>>, public AbstractLtlFormula<T> {

public:
	/*!
	 * Empty constructor.
	 * Will create an AND-node without subnotes. Will not represent a complete formula!
	 */
	And() {
		//intentionally left empty
	}

	/*!
	 * Constructor.
	 * Creates an AND node with the parameters as subtrees.
	 *
	 * @param left The left sub formula
	 * @param right The right sub formula
	 */
	And(AbstractLtlFormula<T>* left, AbstractLtlFormula<T>* right)
		: storm::property::abstract::And<T, AbstractLtlFormula<T>>(left, right) {
		//intentionally left empty
	}

	/*!
	 * Destructor.
	 *
	 * The subtrees are deleted with the object
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~And() {
		//intentionally left empty
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual AbstractLtlFormula<T>* clone() const override {
		And<T>* result = new And();
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
	virtual std::vector<T>* check(const storm::modelchecker::ltl::AbstractModelChecker<T>& modelChecker) const override {
		return modelChecker.template as<IAndModelChecker>()->checkAnd(*this);
	}

	virtual void visit(visitor::AbstractLtlFormulaVisitor<T>& visitor) const {
		visitor.template as<IAndVisitor>()->visitAnd(*this);
	}

};

} //namespace ltl

} //namespace property

} //namespace storm

#endif /* STORM_FORMULA_LTL_AND_H_ */
