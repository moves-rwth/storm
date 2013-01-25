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
	RewardBoundOperator() : BoundOperator<T>(BoundOperator<T>::LESS_EQUAL, storm::utility::constGetZero<T>(), nullptr) {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param comparisonRelation The relation to compare the actual value and the bound
	 * @param bound The bound for the probability
	 * @param pathFormula The child node
	 */
	RewardBoundOperator(
			typename BoundOperator<T>::ComparisonType comparisonRelation, T bound, PctlPathFormula<T>* pathFormula) :
				BoundOperator<T>(BoundOperator<T>::LESS, bound, pathFormula) {
		// Intentionally left empty
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "R ";
		result += BoundOperator<T>::toString();
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
		result->setComparisonOperator(this->getComparisonOperator());
		result->setBound(this->getBound());
		result->setPathFormula(this->getPathFormula().clone());
		return result;
	}
};

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_REWARDBOUNDOPERATOR_H_ */
