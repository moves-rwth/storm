/*
 * ProbabilisticBoundOperator.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PROBABILISTICBOUNDOPERATOR_H_
#define STORM_FORMULA_PROBABILISTICBOUNDOPERATOR_H_

#include "AbstractStateFormula.h"
#include "AbstractPathFormula.h"
#include "BoundOperator.h"
#include "utility/ConstTemplates.h"

namespace storm {

namespace formula {

/*!
 * @brief
 * Class for a Abstract formula tree with a P (probablistic) operator node over a probability interval
 * as root.
 *
 * Has one Abstract path formula as sub formula/tree.
 *
 * @par Semantics
 * 	  The formula holds iff the probability that the path formula holds is inside the bounds
 * 	  specified in this operator
 *
 * The subtree is seen as part of the object and deleted with it
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 *
 * @see AbstractStateFormula
 * @see AbstractPathFormula
 * @see ProbabilisticOperator
 * @see ProbabilisticNoBoundsOperator
 * @see AbstractFormula
 */
template<class T>
class ProbabilisticBoundOperator : public BoundOperator<T> {

public:
	/*!
	 * Empty constructor
	 */
//! TODO: this constructor should give a comparisontype as first argument
	ProbabilisticBoundOperator() : BoundOperator<T>(storm::utility::constGetZero<T>(), storm::utility::constGetZero<T>(), nullptr) {
		// Intentionally left empty
	}


	/*!
	 * Constructor
	 *
	 * @param lowerBound The lower bound for the probability
	 * @param upperBound The upper bound for the probability
	 * @param pathFormula The child node
	 */
	ProbabilisticBoundOperator(T lowerBound, T upperBound, AbstractPathFormula<T>& pathFormula) : BoundOperator<T>(lowerBound, upperBound, pathFormula) {
		// Intentionally left empty
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "P [";
		result += std::to_string(this->getLowerBound());
		result += ",";
		result += std::to_string(this->getUpperBound());
		result += "] (";
		result += this->getPathFormula()->toString();
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
	virtual AbstractStateFormula<T>* clone() const {
		ProbabilisticBoundOperator<T>* result = new ProbabilisticBoundOperator<T>();
		result->setInterval(this->getLowerBound(), this->getUpperBound());
		result->setPathFormula(this->getPathFormula()->clone());
		return result;
	}
};

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_PROBABILISTICBOUNDOPERATOR_H_ */
