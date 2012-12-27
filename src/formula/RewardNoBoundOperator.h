/*
 * RewardNoBoundOperator.h
 *
 *  Created on: 25.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_REWARDNOBOUNDOPERATOR_H_
#define STORM_FORMULA_REWARDNOBOUNDOPERATOR_H_

#include "PctlFormula.h"
#include "PctlPathFormula.h"
#include "NoBoundOperator.h"

namespace storm {

namespace formula {

/*!
 * @brief
 * Class for a PCTL formula tree with a R (reward) operator without declaration of reward values
 * as root.
 *
 * Checking a formula with this operator as root returns the reward for the reward path formula for
 * each state
 *
 * Has one PCTL path formula as sub formula/tree.
 *
 * @note
 * 	This class is a hybrid of a state and path formula, and may only appear as the outermost operator.
 * 	Hence, it is seen as neither a state nor a path formula, but is directly derived from PctlFormula.
 *
 * @note
 * 	This class does not contain a check() method like the other formula classes.
 * 	The check method should only be called by the model checker to infer the correct check function for sub
 * 	formulas. As this operator can only appear at the root, the method is not useful here.
 * 	Use the checkRewardNoBoundOperator method from the DtmcPrctlModelChecker class instead.
 *
 * The subtree is seen as part of the object and deleted with it
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 *
 * @see PctlStateFormula
 * @see PctlPathFormula
 * @see ProbabilisticOperator
 * @see ProbabilisticIntervalOperator
 * @see PctlFormula
 */
template <class T>
class RewardNoBoundOperator: public NoBoundOperator<T> {
public:
	/*!
	 * Empty constructor
	 */
	RewardNoBoundOperator() : NoBoundOperator<T>(nullptr) {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param pathFormula The child node.
	 */
	RewardNoBoundOperator(PctlPathFormula<T>* pathFormula) : NoBoundOperator<T>(pathFormula) {
		// Intentionally left empty
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = " R=? [";
		result += this->getPathFormula().toString();
		result += "]";
		return result;
	}
};

} /* namespace formula */

} /* namespace storm */

#endif /* STORM_FORMULA_REWARDNOBOUNDOPERATOR_H_ */
