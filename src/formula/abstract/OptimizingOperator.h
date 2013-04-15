#ifndef STORM_FORMULA_ABSTRACT_OPTIMIZINGOPERATOR_H_
#define STORM_FORMULA_ABSTRACT_OPTIMIZINGOPERATOR_H_

namespace storm {

namespace formula {

namespace abstract {

class OptimizingOperator {
public:
	/*!
	 * Empty constructor
	 */
	OptimizingOperator() : optimalityOperator(false), minimumOperator(false) {
	}

	/*!
	 * Constructor
	 *
	 * @param minimumOperator A flag indicating whether this operator is a minimizing or a maximizing operator.
	 */
	OptimizingOperator(bool minimumOperator) : optimalityOperator(true), minimumOperator(minimumOperator) {
	}

	/*!
	 * Retrieves whether the operator is to be interpreted as an optimizing (i.e. min/max) operator.
	 * @returns True if the operator is an optimizing operator.
	 */
	bool isOptimalityOperator() const {
		return optimalityOperator;
	}

	/*!
	 * Retrieves whether the operator is a minimizing operator given that it is an optimality
	 * operator.
	 * @returns True if the operator is an optimizing operator and it is a minimizing operator and
	 * false otherwise, i.e. if it is either not an optimizing operator or not a minimizing operator.
	 */
	bool isMinimumOperator() const {
		return optimalityOperator && minimumOperator;
	}

private:
	// A flag that indicates whether this operator is meant as an optimizing (i.e. min/max) operator
	// over a nondeterministic model.
	bool optimalityOperator;

	// In the case this operator is an optimizing operator, this flag indicates whether it is
	// looking for the minimum or the maximum value.
	bool minimumOperator;
};

} //namespace abstract

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_OPTIMIZINGOPERATOR_H_ */
