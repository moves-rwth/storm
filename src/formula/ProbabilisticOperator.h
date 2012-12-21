/*
 * ProbabilisticOperator.h
 *
 *  Created on: 07.12.2012
 *      Author: Thomas Heinemann
 */

#ifndef MRMC_FORMULA_PROBABILISTICOPERATOR_H_
#define MRMC_FORMULA_PROBABILISTICOPERATOR_H_

#include "PctlStateFormula.h"

namespace mrmc {
namespace formula {

/*!
 * @brief
 * Class for a PCTL formula tree with a P (probablistic) operator node over a single real valued
 * probability as root.
 *
 * If the probability interval consist just of one single value (i.e. it is [x,x] for some
 * real number x), the class ProbabilisticOperator should be used instead.
 *
 *
 * Has one PCTL path formula as sub formula/tree.
 *
 * @par Semantics
 * 	  The formula holds iff the probability that the path formula holds is equal to the probablility
 * 	  specified in this operator
 *
 * The subtree is seen as part of the object and deleted with it
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 *
 * @see PctlStateFormula
 * @see PctlPathFormula
 * @see ProbabilisticIntervalOperator
 * @see ProbabilisticNoBoundsOperator
 * @see PctlFormula
 */
template<class T>
class ProbabilisticOperator : public mrmc::formula::PctlStateFormula<T> {
public:
	/*!
	 * Empty constructor
	 */
	ProbabilisticOperator() {
		this->pathFormula = NULL;
	}

	/*!
	 * Constructor
	 *
	 * @param bound The expected value for path formulas
	 * @param pathFormula The child node
	 */
	ProbabilisticOperator(T bound, PctlPathFormula<T>* pathFormula) {
		this->bound = bound;
		this->pathFormula = *pathFormula;
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
	const PctlPathFormula<T>& getPathFormula () const {
		return *pathFormula;
	}

	/*!
	 * @returns the bound for the probability
	 */
	const T& getBound() const {
		return bound;
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
	 * Sets the expected probability that the path formula holds.
	 *
	 * @param bound The bound for the probability
	 */
	void setBound(T bound) {
		this->bound = bound;
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new ProbabilisticOperator-object that is identical to the called object.
	 */
	virtual PctlStateFormula<T>* clone() const {
		ProbabilisticOperator<T>* result = new ProbabilisticOperator<T>();
		result->setBound(bound);
		if (pathFormula != NULL) {
			result->setPathFormula(pathFormula->clone());
		}
		return result;
	}

	/*!
	 * Calls the model checker to check this formula.
	 * Needed to infer the correct type of formula class.
	 *
	 * @note This function should only be called in a generic check function of a model checker
	 * 		class. For other uses, the methods of the model checker should be used.
	 *
	 * @returns A bit vector indicating all states that satisfy the formula represented by the
	 *          called object.
	 */
	virtual mrmc::storage::BitVector *check(
			const mrmc::modelChecker::DtmcPrctlModelChecker<T>& modelChecker) const {
	  return modelChecker.checkProbabilisticOperator(*this);
	}

	/*!
	 *	Returns a string representation of this PctlStateFormula
	 * 
	 * @returns a string representation of this PctlStateFormula
	 */
	virtual std::string toString() const {
		std::string result = " P=";
		result += std::to_string(bound);
		result += " (";
		result += pathFormula->toString();
		result += ")";
		return result;
	}
private:
	T bound;
	PctlPathFormula<T>* pathFormula;
};

} /* namespace formula */
} /* namespace mrmc */

#endif /* MRMC_FORMULA_PROBABILISTICOPERATOR_H_ */
