/*
 * TimeBoundedEventually.h
 *
 *  Created on: 10.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_TIMEBOUNDEDEVENTUALLY_H_
#define STORM_FORMULA_TIMEBOUNDEDEVENTUALLY_H_

#include "TimeBoundedOperator.h"
#include "AbstractStateFormula.h"

namespace storm {
namespace formula {

template<class T> class TimeBoundedEventually;

/*!
 *  @brief Interface class for model checkers that support TimeBoundedEventually.
 *
 *  All model checkers that support the formula class BoundedEventually must inherit
 *  this pure virtual class.
 */
template <class T>
class ITimeBoundedEventuallyModelChecker {
    public:
		/*!
         *  @brief Evaluates TimeBoundedUntil formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T>* checkTimeBoundedEventually(const TimeBoundedEventually<T>& obj, bool qualitative) const = 0;
};


template<class T>
class TimeBoundedEventually: public storm::formula::TimeBoundedOperator<T> {
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

	TimeBoundedEventually(T lowerBound, T upperBound, AbstractStateFormula<T>* child) :
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
	const AbstractStateFormula<T>& getChild() const {
		return *child;
	}

	/*!
	 * Sets the subtree
	 * @param child the new child node
	 */
	void setChild(AbstractStateFormula<T>* child) {
		this->child = child;
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
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedUntil-object that is identical the called object.
	 */
	virtual AbstractPathFormula<T>* clone() const {
		TimeBoundedEventually<T>* result = new TimeBoundedEventually<T>(this->getLowerBound(), this->getUpperBound());
		if (child != nullptr) {
			result->setChild(child->clone());
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
	 * @returns A vector indicating the probability that the formula holds for each state.
	 */
	virtual std::vector<T>* check(const storm::modelchecker::AbstractModelChecker<T>& modelChecker, bool qualitative) const {
		return modelChecker.template as<ITimeBoundedEventuallyModelChecker>()->checkTimeBoundedEventually(*this, qualitative);
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
	AbstractStateFormula<T>* child;
};

} /* namespace formula */
} /* namespace storm */
#endif /* STORM_FORMULA_TIMEBOUNDEDEVENTUALLY_H_ */
