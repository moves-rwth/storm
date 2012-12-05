/*
 * ProbabilisticOperator.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef PROBABILISTICOPERATOR_H_
#define PROBABILISTICOPERATOR_H_

#include "PCTLStateFormula.h"
#include "PCTLPathFormula.h"
#include "misc/const_templates.h"

namespace mrmc {

namespace formula {

/*!
 * @brief
 * Class for a PCTL formula tree with a P (probablistic) operator node as root.
 *
 * Has one PCTL path formula as sub formula/tree.
 *
 * @par Semantics
 * 	  The formula holds iff the probability that the path formula holds is inside the bounds specified in this operator
 *
 * The subtree is seen as part of the object and deleted with it
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see PCTLStateFormula
 * @see PCTLPathFormula
 * @see PCTLFormula
 */
template<class T>
class ProbabilisticOperator : public PCTLStateFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	ProbabilisticOperator() {
		upper = mrmc::misc::constGetZero(&upper);
		lower = mrmc::misc::constGetZero(&lower);
		pathFormula = NULL;
	}

	/*!
	 * Constructor
	 *
	 * @param lowerBound The lower bound for the probability
	 * @param upperBound The upper bound for the probability
	 * @param pathFormula The child node (can be omitted, is then set to NULL)
	 */
	ProbabilisticOperator(T lowerBound, T upperBound, PCTLPathFormula<T>* pathFormula=NULL) {
		this->lower = lowerBound;
		this->upper = upperBound;
		this->pathFormula = pathFormula;
	}

	/*!
	 * Destructor
	 *
	 * The subtree is deleted with the object
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~ProbabilisticOperator() {
	 if (pathFormula != NULL) {
		 delete pathFormula;
	 }
	}

	/*!
	 * @returns the child node (representation of a PCTL path formula)
	 */
	PCTLPathFormula<T>& getPathFormula () {
		return *pathFormula;
	}

	/*!
	 * @returns the lower bound for the probability
	 */
	const T& getLowerBound() {
		return lower;
	}

	/*!
	 * @returns the upper bound for the probability
	 */
	const T& getUpperBound() {
		return upper;
	}

	/*!
	 * Sets the child node
	 *
	 * @param pathFormula the path formula that becomes the new child node
	 */
	void setPathFormula(PCTLPathFormula<T>* pathFormula) {
		this->pathFormula = pathFormula;
	}

	/*!
	 * Sets the interval in which the probability that the path formula holds may lie.
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
	virtual std::string toString() {
		std::string result = "(";
		result += " P[";
		result += lower;
		result += ";";
		result += upper;
		result += "] ";
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
	virtual PCTLStateFormula<T>* clone() {
		ProbabilisticOperator<T>* result = new ProbabilisticOperator<T>();
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
	virtual mrmc::storage::BitVector *check(mrmc::modelChecker::DtmcPrctlModelChecker<T>& modelChecker) {
	  return modelChecker.checkProbabilisticOperator(this);
	}

private:
	T lower;
	T upper;
	PCTLPathFormula<T>* pathFormula;
};

} //namespace formula

} //namespace mrmc

#endif /* PROBABILISTICOPERATOR_H_ */
