/*
 * Next.h
 *
 *  Created on: 26.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_REACHABILITYREWARD_H_
#define STORM_FORMULA_REACHABILITYREWARD_H_

#include "PctlPathFormula.h"
#include "PctlStateFormula.h"

namespace storm {

namespace formula {

/*!
 * @brief
 * Class for a PCTL (path) formula tree with an Reachability Reward node as root.
 *
 * Has one PCTL state formula as sub formula/tree.
 *
 * The subtree is seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to nullptr before deletion)
 *
 * @see PctlPathFormula
 * @see PctlFormula
 */
template <class T>
class ReachabilityReward : public PctlPathFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	ReachabilityReward() {
		this->child = nullptr;
	}

	/*!
	 * Constructor
	 *
	 * @param child The child node
	 */
	ReachabilityReward(PctlStateFormula<T>* child) {
		this->child = child;
	}

	/*!
	 * Constructor.
	 *
	 * Also deletes the subtree.
	 * (this behaviour can be prevented by setting the subtrees to nullptr before deletion)
	 */
	virtual ~ReachabilityReward() {
	  if (child != nullptr) {
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
		std::string result = "F ";
		result += child->toString();
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
		ReachabilityReward<T>* result = new ReachabilityReward<T>();
		if (child != nullptr) {
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
	virtual std::vector<T> *check(const storm::modelChecker::DtmcPrctlModelChecker<T>& modelChecker) const {
	  return modelChecker.checkReachabilityReward(*this);
	}

private:
	PctlStateFormula<T>* child;
};

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_REACHABILITYREWARD_H_ */
