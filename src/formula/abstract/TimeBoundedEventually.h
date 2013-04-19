/*
 * TimeBoundedEventually.h
 *
 *  Created on: 10.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_ABSTRACT_TIMEBOUNDEDEVENTUALLY_H_
#define STORM_FORMULA_ABSTRACT_TIMEBOUNDEDEVENTUALLY_H_

#include "TimeBoundedOperator.h"

namespace storm {
namespace formula {
namespace abstract {


template<class T, class FormulaType>
class TimeBoundedEventually: public storm::formula::abstract::TimeBoundedOperator<T> {
public:
	/**
	 * Simple constructor: Only sets the bounds
	 *
	 * @param lowerBound
	 * @param upperBound
	 */
	TimeBoundedEventually(T lowerBound, T upperBound) : TimeBoundedOperator<T>(lowerBound, upperBound) {
		child = nullptr;
	}

	TimeBoundedEventually(T lowerBound, T upperBound, FormulaType* child) :
		TimeBoundedOperator<T>(lowerBound, upperBound) {
		this->child = child;
	}

	virtual ~TimeBoundedEventually() {
		if (child != nullptr) {
			delete child;
		}
	}

	/*!
	 * @returns the child node
	 */
	const FormulaType& getChild() const {
		return *child;
	}

	/*!
	 * Sets the subtree
	 * @param child the new child node
	 */
	void setChild(FormulaType* child) {
		this->child = child;
	}

	/*!
	 *
	 * @return True if the child is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool childIsSet() const {
		return child != nullptr;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "F";
		result += TimeBoundedOperator<T>::toString();
		result += " ";
		result += child->toString();
		return result;
	}

	/*!
     *  @brief Checks if the subtree conforms to some logic.
     *
     *  @param checker Formula checker object.
     *  @return true iff the subtree conforms to some logic.
     */
	virtual bool conforms(const AbstractFormulaChecker<T>& checker) const {
		return checker.conforms(this->child);
	}

private:
	FormulaType* child;
};

} /* namespace abstract */
} /* namespace formula */
} /* namespace storm */

#endif /* STORM_FORMULA_ABSTRACT_TIMEBOUNDEDEVENTUALLY_H_ */
