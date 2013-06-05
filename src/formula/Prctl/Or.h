/*
 * Or.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PRCTL_OR_H_
#define STORM_FORMULA_PRCTL_OR_H_

#include "AbstractStateFormula.h"
#include "src/formula/abstract/Or.h"
#include "src/formula/AbstractFormulaChecker.h"

namespace storm {
namespace property {
namespace prctl {

template <class T> class Or;

/*!
 *  @brief Interface class for model checkers that support Or.
 *   
 *  All model checkers that support the formula class Or must inherit
 *  this pure virtual class.
 */
template <class T>
class IOrModelChecker {
	public:
		/*!
         *  @brief Evaluates Or formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
		virtual storm::storage::BitVector* checkOr(const Or<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for an abstract formula tree with OR node as root.
 *
 * Has two Abstract state formulas as sub formulas/trees.
 *
 * As OR is commutative, the order is \e theoretically not important, but will influence the order in which
 * the model checker works.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractStateFormula
 * @see AbstractPrctlFormula
 */
template <class T>
class Or : public storm::property::abstract::Or<T, AbstractStateFormula<T>>,
			  public AbstractStateFormula<T> {

public:
	/*!
	 * Empty constructor.
	 * Will create an OR-node without subnotes. The result does not represent a complete formula!
	 */
	Or() {
		//intentionally left empty
	}

	/*!
	 * Constructor.
	 * Creates an OR note with the parameters as subtrees.
	 *
	 * @param left The left sub formula
	 * @param right The right sub formula
	 */
	Or(AbstractStateFormula<T>* left, AbstractStateFormula<T>* right) :
		storm::property::abstract::Or<T, AbstractStateFormula<T>>(left, right) {
		//intentionally left empty
	}

	/*!
	 * Destructor.
	 *
	 * The subtrees are deleted with the object
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~Or() {
	  //intentionally left empty
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual AbstractStateFormula<T>* clone() const override {
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
	virtual storm::storage::BitVector *check(const storm::modelchecker::prctl::AbstractModelChecker<T>& modelChecker) const override {
		return modelChecker.template as<IOrModelChecker>()->checkOr(*this);
	}
};

} //namespace prctl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_OR_H_ */
