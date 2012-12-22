/*
 * ProbabilisticOperator.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef MRMC_FORMULA_PROBABILISTICINTERVALOPERATOR_H_
#define MRMC_FORMULA_PROBABILISTICINTERVALOPERATOR_H_

#include "PctlStateFormula.h"
#include "PctlPathFormula.h"
#include "utility/ConstTemplates.h"

namespace mrmc {

namespace formula {

/*!
 * @brief
 * Class for a PCTL formula tree with a P (probablistic) operator node over a probability interval
 * as root.
 *
 * If the probability interval consist just of one single value (i.e. it is [x,x] for some
 * real number x), the class ProbabilisticOperator should be used instead.
 *
 *
 * Has one PCTL path formula as sub formula/tree.
 *
 * @par Semantics
 * 	  The formula holds iff the probability that the path formula holds is inside the bounds
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
class ProbabilisticIntervalOperator : public PctlStateFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	ProbabilisticIntervalOperator() {
		upper = mrmc::utility::constGetZero<T>();
		lower = mrmc::utility::constGetZero<T>();
		pathFormula = NULL;
	}

	/*!
	 * Constructor
	 *
	 * @param lowerBound The lower bound for the probability
	 * @param upperBound The upper bound for the probability
	 * @param pathFormula The child node
	 */
	ProbabilisticIntervalOperator(T lowerBound, T upperBound, PctlPathFormula<T>& pathFormula) {
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
	virtual ~ProbabilisticIntervalOperator() {
	 if (pathFormula != NULL) {
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
		std::string result = "P[";
		result += std::to_string(lower);
		result += ";";
		result += std::to_string(upper);
		result += "] (";
		result += pathFormula->toString();
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
		ProbabilisticIntervalOperator<T>* result = new ProbabilisticIntervalOperator<T>();
		result->setInterval(lower, upper);
		if (pathFormula != NULL) {
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
	virtual mrmc::storage::BitVector *check(const mrmc::modelChecker::DtmcPrctlModelChecker<T>& modelChecker) const {
	  return modelChecker.checkProbabilisticIntervalOperator(*this);
	}

private:
	T lower;
	T upper;
	PctlPathFormula<T>* pathFormula;
};

} //namespace formula

} //namespace mrmc

#endif /* MRMC_FORMULA_PROBABILISTICINTERVALOPERATOR_H_ */
