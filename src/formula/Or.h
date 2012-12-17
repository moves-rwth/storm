/*
 * Or.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef MRMC_FORMULA_OR_H_
#define MRMC_FORMULA_OR_H_

#include "PctlStateFormula.h"

namespace mrmc {

namespace formula {

/*!
 * @brief
 * Class for a PCTL formula tree with OR node as root.
 *
 * Has two PCTL state formulas as sub formulas/trees.
 *
 * As OR is commutative, the order is \e theoretically not important, but will influence the order in which
 * the model checker works.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see PctlStateFormula
 * @see PctlFormula
 */
template <class T>
class Or : public PctlStateFormula<T> {

public:
	/*!
	 * Empty constructor.
	 * Will create an AND-node without subnotes. Will not represent a complete formula!
	 */
	Or() {
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
	Or(PctlStateFormula<T>* left, PctlStateFormula<T>* right) {
		this->left = left;
		this->right = right;
	}

	/*!
	 * Destructor.
	 *
	 * The subtrees are deleted with the object
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~Or() {
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
	void setLeft(PctlStateFormula<T>* newLeft) {
		left = newLeft;
	}

	/*!
	 * Sets the right child node.
	 *
	 * @param newRight the new right child.
	 */
	void setRight(PctlStateFormula<T>* newRight) {
		right = newRight;
	}

	/*!
	 * @returns a pointer to the left child node
	 */
	const PctlStateFormula<T>& getLeft() const {
		return *left;
	}

	/*!
	 * @returns a pointer to the right child node
	 */
	const PctlStateFormula<T>& getRight() const {
		return *right;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "(";
		result += left->toString();
		result += " || ";
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
	virtual PctlStateFormula<T>* clone() const {
		Or<T>* result = new Or();
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
	virtual mrmc::storage::BitVector *check(const mrmc::modelChecker::DtmcPrctlModelChecker<T>& modelChecker) const {
	  return modelChecker.checkOr(*this);
	}

private:
	PctlStateFormula<T>* left;
	PctlStateFormula<T>* right;
};

} //namespace formula

} //namespace mrmc

#endif /* MRMC_FORMULA_OR_H_ */
