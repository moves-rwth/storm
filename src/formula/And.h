/*
 * And.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_AND_H_
#define STORM_FORMULA_AND_H_

#include "src/formula/AbstractStateFormula.h"
#include "src/modelChecker/AbstractModelChecker.h"
#include <string>

namespace storm {
namespace formula {

template <class T> class And;

template <class T>
class IAndModelChecker {
	public:
		virtual storm::storage::BitVector* checkAnd(const And<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for a Abstract formula tree with AND node as root.
 *
 * Has two Abstract state formulas as sub formulas/trees.
 *
 * As AND is commutative, the order is \e theoretically not important, but will influence the order in which
 * the model checker works.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractStateFormula
 * @see AbstractFormula
 */
template <class T>
class And : public AbstractStateFormula<T> {

public:
	/*!
	 * Empty constructor.
	 * Will create an AND-node without subnotes. Will not represent a complete formula!
	 */
	And() {
		left = NULL;
		right = NULL;
	}

	/*!
	 * Constructor.
	 * Creates an AND note with the parameters as subtrees.
	 *
	 * @param left The left sub formula
	 * @param right The right sub formula
	 */
	And(AbstractStateFormula<T>* left, AbstractStateFormula<T>* right) {
		this->left = left;
		this->right = right;
	}

	/*!
	 * Destructor.
	 *
	 * The subtrees are deleted with the object
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~And() {
		if (left != NULL) {
			delete left;
		}
		if (right != NULL) {
			delete right;
		}
	}

	/*!
	 * Sets the left child node.
	 *
	 * @param newLeft the new left child.
	 */
	void setLeft(AbstractStateFormula<T>* newLeft) {
		left = newLeft;
	}

	/*!
	 * Sets the right child node.
	 *
	 * @param newRight the new right child.
	 */
	void setRight(AbstractStateFormula<T>* newRight) {
		right = newRight;
	}

	/*!
	 * @returns a pointer to the left child node
	 */
	const AbstractStateFormula<T>& getLeft() const {
		return *left;
	}

	/*!
	 * @returns a pointer to the right child node
	 */
	const AbstractStateFormula<T>& getRight() const {
		return *right;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "(";
		result += left->toString();
		result += " && ";
		result += right->toString();
		result += ")";
		return result;
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual AbstractStateFormula<T>* clone() const {
		And<T>* result = new And();
		if (this->left != NULL) {
		  result->setLeft(left->clone());
		}
		if (this->right != NULL) {
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
	virtual storm::storage::BitVector *check(const storm::modelChecker::AbstractModelChecker<T>& modelChecker) {
		return this->template cast<IAndModelChecker>(modelChecker)->checkAnd(*this);
	}

private:
	AbstractStateFormula<T>* left;
	AbstractStateFormula<T>* right;
};

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_AND_H_ */
