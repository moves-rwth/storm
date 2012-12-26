/*
 * ProbabilisticOperator.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_REWARDINTERVALOPERATOR_H_
#define STORM_FORMULA_REWARDINTERVALOPERATOR_H_

#include "PctlStateFormula.h"
#include "PctlPathFormula.h"
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
class RewardIntervalOperator : public PctlStateFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	RewardIntervalOperator() {
		upper = storm::utility::constGetZero<T>();
		lower = storm::utility::constGetZero<T>();
		pathFormula = nullptr;
	}

	/*!
	 * Constructor
	 *
	 * @param lowerBound The lower bound for the probability
	 * @param upperBound The upper bound for the probability
	 * @param pathFormula The child node
	 */
	RewardIntervalOperator(T lowerBound, T upperBound, PctlPathFormula<T>& pathFormula) {
		this->lower = lowerBound;
		this->upper = upperBound;
		this->pathFormula = &pathFormula;
	}

	/*!
	 * Destructor
	 *
	 * The subtree is deleted with the object
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~RewardIntervalOperator() {
	 if (pathFormula != nullptr) {
		 delete pathFormula;
	 }
	}

	/*!
	 * @returns the child node (representation of a PCTL path formula)
	 */
	const PctlPathFormula<T>& getPathFormula () const {
		return *pathFormula;
	}

	/*!
	 * @returns the lower bound for the probability
	 */
	const T& getLowerBound() const {
		return lower;
	}

	/*!
	 * @returns the upper bound for the probability
	 */
	const T& getUpperBound() const {
		return upper;
	}

	/*!
	 * Sets the child node
	 *
	 * @param pathFormula the path formula that becomes the new child node
	 */
	void setPathFormula(PctlPathFormula<T>* pathFormula) {
		this->pathFormula = pathFormula;
	}

	/*!
	 * Sets the interval in which the probability that the path formula holds may lie in.
	 *
	 * @param lowerBound The lower bound for the probability
	 * @param upperBound The upper bound for the probability
	 */
	void setInterval(T lowerBound, T upperBound) {
		this->lower = lowerBound;
		this->upper = upperBound;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "R [";
		result += std::to_string(lower);
		result += ", ";
		result += std::to_string(upper);
		result += "] [";
		result += pathFormula->toString();
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
		RewardIntervalOperator<T>* result = new RewardIntervalOperator<T>();
		result->setInterval(lower, upper);
		if (pathFormula != nullptr) {
			result->setPathFormula(pathFormula->clone());
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
	virtual storm::storage::BitVector *check(const storm::modelChecker::DtmcPrctlModelChecker<T>& modelChecker) const {
	  return modelChecker.checkRewardIntervalOperator(*this);
	}

private:
	T lower;
	T upper;
	PctlPathFormula<T>* pathFormula;
};

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_REWARDINTERVALOPERATOR_H_ */
