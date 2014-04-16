/*
 * TimeBoundedEventually.h
 *
 *  Created on: 10.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_CSL_TIMEBOUNDEDEVENTUALLY_H_
#define STORM_FORMULA_CSL_TIMEBOUNDEDEVENTUALLY_H_

#include "src/formula/Csl/AbstractPathFormula.h"
#include "src/formula/Csl/AbstractStateFormula.h"

namespace storm {
namespace property {
namespace csl {

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
        virtual std::vector<T> checkTimeBoundedEventually(const TimeBoundedEventually<T>& obj, bool qualitative) const = 0;
};


template<class T>
class TimeBoundedEventually: public AbstractPathFormula<T> {
public:

	/**
	 * Simple constructor: Only sets the bounds
	 *
	 * @param lowerBound
	 * @param upperBound
	 */
	TimeBoundedEventually(T lowerBound, T upperBound) : child(nullptr) {
		setInterval(lowerBound, upperBound);
	}

	TimeBoundedEventually(T lowerBound, T upperBound, AbstractStateFormula<T>* child) : child(nullptr) {
		setInterval(lowerBound, upperBound);
	}

	virtual ~TimeBoundedEventually() {
		if (child != nullptr) {
			delete child;
		}
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedUntil-object that is identical the called object.
	 */
	virtual AbstractPathFormula<T>* clone() const override {
		TimeBoundedEventually<T>* result = new TimeBoundedEventually<T>(this->getLowerBound(), this->getUpperBound());
		if (this->childIsSet()) {
			result->setChild(this->getChild().clone());
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
	virtual std::vector<T> check(const storm::modelchecker::csl::AbstractModelChecker<T>& modelChecker, bool qualitative) const override {
		return modelChecker.template as<ITimeBoundedEventuallyModelChecker>()->checkTimeBoundedEventually(*this, qualitative);
	}

	/*!
	 *  @brief Checks if the subtree conforms to some logic.
	 *
	 *  @param checker Formula checker object.
	 *  @return true iff the subtree conforms to some logic.
	 */
	virtual bool validate(const AbstractFormulaChecker<T>& checker) const override {
		return checker.validate(this->child);
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
		std::string result = "F";
		if (upperBound == std::numeric_limits<double>::infinity()) {
			result = ">=" + std::to_string(lowerBound);
		} else {
			result = "[";
			result += std::to_string(lowerBound);
			result += ",";
			result += std::to_string(upperBound);
			result += "]";
		}
		result += " ";
		result += child->toString();
		return result;
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
	 *
	 * @return True if the child is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool childIsSet() const {
		return child != nullptr;
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

private:
	AbstractStateFormula<T>* child;
	T lowerBound, upperBound;
};

} /* namespace csl */
} /* namespace property */
} /* namespace storm */

#endif /* STORM_FORMULA_CSL_TIMEBOUNDEDEVENTUALLY_H_ */
