/*
 * TimeBoundedOperator.h
 *
 *  Created on: 10.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_ABSTRACT_TIMEBOUNDEDOPERATOR_H_
#define STORM_FORMULA_ABSTRACT_TIMEBOUNDEDOPERATOR_H_

#include <limits>

#include "src/formula/abstract/AbstractFormula.h"
#include "exceptions/InvalidArgumentException.h"

namespace storm {
namespace formula {
namespace abstract {

/*!
 * @brief
 * Class for a Abstract formula tree with a operator node as root that uses a time interval
 * (with upper and lower bound)
 *
 * This class does not provide support for sub formulas; this has to be done in concretizations of this abstract class.
 *
 *
 * @see AbstractFormula
 * @see AbstractFormula
 * @see AbstractFormula
 */
template<class T>
class TimeBoundedOperator: public virtual AbstractFormula<T> {
public:
	/**
	 * Constructor
	 *
	 * @param lowerBound The lower bound
	 * @param upperBound The upper bound
	 * @throw InvalidArgumentException if the lower bound is larger than the upper bound.
	 */
	TimeBoundedOperator(T lowerBound, T upperBound) {
		setInterval(lowerBound, upperBound);
	}

	/**
	 * Destructor
	 */
	virtual ~TimeBoundedOperator() {
		// Intentionally left empty
	}

	/**
	 * Getter for lowerBound attribute
	 *
	 * @return lower bound of the operator.
	 */
	T getLowerBound() const {
		return lowerBound;
	}

	/**
	 * Getter for upperBound attribute
	 * @return upper bound of the operator.
	 */
	T getUpperBound() const {
		return upperBound;
	}

	/**
	 * Set the time interval for the time bounded operator
	 *
	 * @param lowerBound
	 * @param upperBound
	 * @throw InvalidArgumentException if the lower bound is larger than the upper bound.
	 */
	void setInterval(T lowerBound, T upperBound) {
		if (lowerBound > upperBound) {
			throw new storm::exceptions::InvalidArgumentException("Lower bound is larger than upper bound");
		}
		this->lowerBound = lowerBound;
		this->upperBound = upperBound;
	}


	/*!
	 * @returns a string representation of the Interval of the formula
	 * 			May be used in subclasses to simplify string output.
	 */
	virtual std::string toString() const {
		std::string result = "";
		if (upperBound == std::numeric_limits<double>::infinity()) {
			result = ">=" + std::to_string(lowerBound);
		} else {
			result = "[";
			result += std::to_string(lowerBound);
			result += ",";
			result += std::to_string(upperBound);
			result += "]";
		}
		return result;
	}

private:
	T lowerBound, upperBound;
};

} //namespace abstract
} //namespace formula
} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_TIMEBOUNDEDOPERATOR_H_ */
