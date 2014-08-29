/*
 * And.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PRCTL_AND_H_
#define STORM_FORMULA_PRCTL_AND_H_

#include "src/properties/prctl/AbstractStateFormula.h"
#include "src/modelchecker/prctl/ForwardDeclarations.h"
#include <string>

namespace storm {
namespace properties {
namespace prctl {

// Forward declaration for the interface class.
template <class T> class And;

/*!
 * Interface class for model checkers that support And.
 *
 * All model checkers that support the formula class And must inherit
 * this pure virtual class.
 */
template <class T>
class IAndModelChecker {
	public:

		/*!
		 * Empty virtual destructor.
		 */
		virtual ~IAndModelChecker() {
			// Intentionally left empty
		}

		/*!
		 * Evaluates an And formula within a model checker.
		 *
		 * @param obj Formula object with subformulas.
		 * @return The modelchecking result of the formula for every state.
		 */
		virtual storm::storage::BitVector checkAnd(const And<T>& obj) const = 0;
};

/*!
 * Class for a Prctl formula tree with an And node as root.
 *
 * It has two state formulas as sub formulas/trees.
 *
 * As And is commutative, the order is \e theoretically not important, but will influence the order in which
 * the model checker works.
 *
 * The object has shared ownership of its subtrees. If this object is deleted and no other object has a shared
 * ownership of the subtrees they will be deleted as well.
 *
 * @see AbstractStateFormula
 * @see AbstractPrctlFormula
 */
template <class T>
class And : public AbstractStateFormula<T> {

public:

	/*!
	 * Creates an And node without subnodes.
	 * The resulting object will not represent a complete formula!
	 */
	And() : left(nullptr), right(nullptr) {
		// Intentionally left empty.
	}

	/*!
	 * Creates an And node with the parameters as subtrees.
	 *
	 * @param left The left sub formula.
	 * @param right The right sub formula.
	 */
	And(std::shared_ptr<AbstractStateFormula<T>> const & left, std::shared_ptr<AbstractStateFormula<T>> const & right) : left(left), right(right) {
		// Intentionally left empty.
	}

	/*!
	 * Empty virtual destructor.
	 */
	virtual ~And() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones.
	 *
	 * @returns A new And object that is a deep copy of the called object.
	 */
	virtual std::shared_ptr<AbstractStateFormula<T>> clone() const override {
		std::shared_ptr<And<T>> result(new And());
		if (this->isLeftSet()) {
		  result->setLeft(left->clone());
		}
		if (this->isRightSet()) {
		  result->setRight(right->clone());
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
	virtual storm::storage::BitVector check(const storm::modelchecker::prctl::AbstractModelChecker<T>& modelChecker) const override {
		return modelChecker.template as<IAndModelChecker>()->checkAnd(*this);
	}

	/*!
	 * Returns a textual representation of the formula tree with this node as root.
	 *
	 * @returns A string representing the formula tree.
	 */
	virtual std::string toString() const override {
		std::string result = "(";
		result += left->toString();
		result += " & ";
		result += right->toString();
		result += ")";
		return result;
	}

	/*!
	 * Returns whether the formula is a propositional logic formula.
	 * That is, this formula and all its subformulas consist only of And, Or, Not and AP.
	 *
	 * @return True iff this is a propositional logic formula.
	 */
	virtual bool isPropositional() const override {
		return left->isPropositional() && right->isPropositional();
	}

	/*!
	 * Gets the left child node.
	 *
	 * @returns The left child node.
	 */
	std::shared_ptr<AbstractStateFormula<T>> const & getLeft() const {
		return left;
	}

	/*!
	 * Gets the right child node.
	 *
	 * @returns The right child node.
	 */
	std::shared_ptr<AbstractStateFormula<T>> const & getRight() const {
		return right;
	}

	/*!
	 * Sets the left child node.
	 *
	 * @param newLeft The new left child.
	 */
	void setLeft(std::shared_ptr<AbstractStateFormula<T>> const & newLeft) {
		left = newLeft;
	}

	/*!
	 * Sets the right child node.
	 *
	 * @param newRight The new right child.
	 */
	void setRight(std::shared_ptr<AbstractStateFormula<T>> const & newRight) {
		right = newRight;
	}

	/*!
	 * Checks if the left child is set, i.e. it does not point to null.
	 *
	 * @return True iff the left child is set.
	 */
	bool isLeftSet() const {
		return left.get() != nullptr;
	}

	/*!
	 * Checks if the right child is set, i.e. it does not point to null.
	 *
	 * @return True iff the right child is set.
	 */
	bool isRightSet() const {
		return right.get() != nullptr;
	}

private:

	// The left child node.
	std::shared_ptr<AbstractStateFormula<T>> left;

	// The right child node.
	std::shared_ptr<AbstractStateFormula<T>> right;

};

} //namespace prctl

} //namespace properties

} //namespace storm

#endif /* STORM_FORMULA_PRCTL_AND_H_ */
