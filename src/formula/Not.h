/*
 * Not.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef NOT_H_
#define NOT_H_

#include "PCTLStateFormula.h"

namespace mrmc {

namespace formula {

/*!
 * @brief
 * Class for a PCTL formula tree with NOT node as root.
 *
 * Has one PCTL state formula as sub formula/tree.
 *
 * The subtree is seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see PCTLStateFormula
 * @see PCTLFormula
 */
template <class T>
class Not : public PCTLStateFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	Not() {
		this->child = NULL;
	}

	/*!
	 * Constructor
	 * @param child The child node
	 */
	Not(PCTLStateFormula<T>* child) {
		this->child = child;
	}

	/*!
	 * Destructor
	 *
	 * Also deletes the subtree
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~Not() {
	  if (child != NULL) {
		  delete child;
	  }
	}

	/*!
	 * @returns The child node
	 */
	PCTLStateFormula<T>* getChild() {
		return child;
	}

	/*!
	 * Sets the subtree
	 * @param child the new child node
	 */
	void setChild(PCTLStateFormula<T>* child) {
		this->child = child;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() {
		std::string result = "!";
		result += child->toString();
		return result;
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual PCTLStateFormula<T>* clone() {
		Not<T>* result = new Not<T>();
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
	 * @returns A bit vector indicating all states that satisfy the formula represented by the called object.
	 */
	virtual mrmc::storage::BitVector *check(mrmc::modelChecker::DtmcPrctlModelChecker<T>* modelChecker) {
	  return modelChecker->checkNot(this);
	}

private:
	PCTLStateFormula<T>* child;
};

} //namespace formula

} //namespace MRMC

#endif /* NOT_H_ */
