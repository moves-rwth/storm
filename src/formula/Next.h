/*
 * Next.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef MRMC_FORMULA_NEXT_H_
#define MRMC_FORMULA_NEXT_H_

#include "PctlPathFormula.h"
#include "PctlStateFormula.h"

namespace mrmc {

namespace formula {

/*!
 * @brief
 * Class for a PCTL (path) formula tree with a Next node as root.
 *
 * Has two PCTL state formulas as sub formulas/trees.
 *
 * @par Semantics
 * The formula holds iff in the next step, \e child holds
 *
 * The subtree is seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see PctlPathFormula
 * @see PctlFormula
 */
template <class T>
class Next : public PctlPathFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	Next() {
		this->child = NULL;
	}

	/*!
	 * Constructor
	 *
	 * @param child The child node
	 */
	Next(PctlStateFormula<T>* child) {
		this->child = child;
	}

	/*!
	 * Constructor.
	 *
	 * Also deletes the subtree.
	 * (this behaviour can be prevented by setting the subtrees to NULL before deletion)
	 */
	virtual ~Next() {
	  if (child != NULL) {
		  delete child;
	  }
	}

	/*!
	 * @returns the child node
	 */
	const PctlStateFormula<T>& getChild() const {
		return *child;
	}

	/*!
	 * Sets the subtree
	 * @param child the new child node
	 */
	void setChild(PctlStateFormula<T>* child) {
		this->child = child;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "(";
		result += " X ";
		result += child->toString();
		result += ")";
		return result;
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedUntil-object that is identical the called object.
	 */
	virtual PctlPathFormula<T>* clone() const {
		Next<T>* result = new Next<T>();
		if (child != NULL) {
			result->setChild(child);
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
	 * @returns A vector indicating the probability that the formula holds for each state.
	 */
	virtual std::vector<T> *check(const mrmc::modelChecker::DtmcPrctlModelChecker<T>& modelChecker) const {
	  return modelChecker.checkNext(*this);
	}

private:
	PctlStateFormula<T>* child;
};

} //namespace formula

} //namespace mrmc

#endif /* MRMC_FORMULA_NEXT_H_ */
