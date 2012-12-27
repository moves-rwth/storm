/*
 * RewardBoundOperator.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_REWARDBOUNDOPERATOR_H_
#define STORM_FORMULA_REWARDBOUNDOPERATOR_H_

#include "PctlStateFormula.h"
#include "PctlPathFormula.h"
#include "BoundOperator.h"
#include "utility/ConstTemplates.h"

namespace storm {

namespace formula {

/*!
 * @brief
 * Class for a PCTL formula tree with a R (reward) operator node over a reward interval as root.
 *
 * Has a reward path formula as sub formula/tree.
 *
 * @par Semantics
 * 	  The formula holds iff the reward of the reward path formula is inside the bounds
 * 	  specified in this operator
 *
 * The subtree is seen as part of the object and deleted with it
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 *
 * @see PctlStateFormula
 * @see PctlPathFormula
 * @see ProbabilisticOperator
 * @see ProbabilisticNoBoundsOperator
 * @see PctlFormula
 */
template<class T>
class RewardBoundOperator : public BoundOperator<T> {

public:
	/*!
	 * Empty constructor
	 */
	RewardBoundOperator() : BoundOperator<T>(storm::utility::constGetZero<T>(), storm::utility::constGetZero<T>(), nullptr) {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param lowerBound The lower bound for the probability
	 * @param upperBound The upper bound for the probability
	 * @param pathFormula The child node
	 */
	RewardBoundOperator(T lowerBound, T upperBound, PctlPathFormula<T>& pathFormula) : BoundOperator<T>(lowerBound, upperBound, pathFormula) {
		// Intentionally left empty
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "R [";
		result += std::to_string(this->getLowerBound());
		result += ", ";
		result += std::to_string(this->getUpperBound());
		result += "] [";
		result += this->getPathFormula()->toString();
		result += "]";
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
		RewardBoundOperator<T>* result = new RewardBoundOperator<T>();
		result->setBound(this->getLowerBound(), this->getUpperBound());
		result->setPathFormula(this->getPathFormula()->clone());
		return result;
	}
};

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_REWARDBOUNDOPERATOR_H_ */
