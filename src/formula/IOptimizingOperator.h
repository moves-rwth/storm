/*
 * IOptimizingOperator.h
 *
 *  Created on: 17.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_IOPTIMIZINGOPERATOR_H_
#define STORM_FORMULA_IOPTIMIZINGOPERATOR_H_

namespace storm {
namespace property {

/*!
 * @brief Interface for optimizing operators
 *
 * Needed to link abstract classes in concrete logics with the logic-abstract implementation.
 */
class IOptimizingOperator {
public:

	virtual ~IOptimizingOperator() {
		// Intentionally left empty
	}

	/*!
	 * Retrieves whether the operator is to be interpreted as an optimizing (i.e. min/max) operator.
	 * @returns True if the operator is an optimizing operator.
	 */
	virtual bool isOptimalityOperator() const = 0;

	/*!
	 * Retrieves whether the operator is a minimizing operator given that it is an optimality
	 * operator.
	 * @returns True if the operator is an optimizing operator and it is a minimizing operator and
	 * false otherwise, i.e. if it is either not an optimizing operator or not a minimizing operator.
	 */
	virtual bool isMinimumOperator() const = 0;
};

} /* namespace property */
} /* namespace storm */
#endif /* STORM_FORMULA_IOPTIMIZINGOPERATOR_H_ */
