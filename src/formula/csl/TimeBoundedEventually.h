/*
 * TimeBoundedEventually.h
 *
 *  Created on: 10.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_CSL_TIMEBOUNDEDEVENTUALLY_H_
#define STORM_FORMULA_CSL_TIMEBOUNDEDEVENTUALLY_H_

#include "src/formula/csl/AbstractPathFormula.h"
#include "src/formula/csl/AbstractStateFormula.h"

namespace storm {
namespace property {
namespace csl {

// Forward declaration for the interface class.
template<class T> class TimeBoundedEventually;

/*!
 * Interface class for model checkers that support TimeBoundedEventually.
 *
 * All model checkers that support the formula class TimeBoundedEventually must inherit
 * this pure virtual class.
 */
template <class T>
class ITimeBoundedEventuallyModelChecker {
    public:

		/*!
		 * Empty virtual destructor.
		 */
		virtual ~ITimeBoundedEventuallyModelChecker() {
			// Intentionally left empty
		}

		/*!
		 * Evaluates a TimeBoundedEventually formula within a model checker.
		 *
		 * @param obj Formula object with subformulas.
		 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
		 *                    results are only compared against the bounds 0 and 1.
		 * @return The modelchecking result of the formula for every state.
		 */
        virtual std::vector<T> checkTimeBoundedEventually(const TimeBoundedEventually<T>& obj, bool qualitative) const = 0;
};

/*!
 * Class for a Csl (path) formula tree with a TimeBoundedEventually node as root.
 *
 * Has one state formula as subformula/tree.
 *
 * @par Semantics
 * The formula holds iff formula \e child holds within the given time interval [lowerBound, upperBound].
 *
 * The object has shared ownership of its subtree. If this object is deleted and no other object has a shared
 * ownership of the subtree it will be deleted as well.
 *
 * @see AbstractPathFormula
 * @see AbstractCslFormula
 */
template<class T>
class TimeBoundedEventually: public AbstractPathFormula<T> {
public:

	/*!
	 * Creates a TimeBoundedEventually node without a subnode.
	 * The resulting object will not represent a complete formula!
	 */
	TimeBoundedEventually() : lowerBound(0), upperBound(0), child(nullptr) {
		// Intentionally left empty.
	}

	/*!
	 * Creates a TimeBoundedEventually node using the given parameters.
	 *
	 * @param lowerBound The lower bound of the admissable time interval.
	 * @param upperBound The upper bound of the admissable time interval.
	 * @param child The child formula subtree.
	 */
	TimeBoundedEventually(T lowerBound, T upperBound, std::shared_ptr<AbstractStateFormula<T>> const & child) : child(child) {
		setInterval(lowerBound, upperBound);
	}

	/*!
	 * Empty virtual destructor.
	 */
	virtual ~TimeBoundedEventually() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subnodes of the new object are clones of the original ones.
	 *
	 * @returns A new TimeBoundedEventually object that is a deep copy of the called object.
	 */
	virtual std::shared_ptr<AbstractPathFormula<T>> clone() const override {
		std::shared_ptr<TimeBoundedEventually<T>> result(new TimeBoundedEventually<T>(lowerBound, upperBound));
		if (this->isChildSet()) {
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
	virtual std::vector<T> check(storm::modelchecker::csl::AbstractModelChecker<T> const & modelChecker, bool qualitative) const override {
		return modelChecker.template as<ITimeBoundedEventuallyModelChecker>()->checkTimeBoundedEventually(*this, qualitative);
	}

	/*!
	 * Returns a textual representation of the formula tree with this node as root.
	 *
	 * @returns A string representing the formula tree.
	 */
	virtual std::string toString() const override {
		std::string result = "F";
		if (upperBound == std::numeric_limits<double>::infinity()) {
			result += ">=" + std::to_string(lowerBound);
		} else {
			result += "[";
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
	 * Gets the child node.
	 *
	 * @returns The child node.
	 */
	std::shared_ptr<AbstractStateFormula<T>> const & getChild() const {
		return child;
	}

	/*!
	 * Sets the subtree.
	 *
	 * @param child The new child.
	 */
	void setChild(std::shared_ptr<AbstractStateFormula<T>> const & child) {
		this->child = child;
	}

	/*!
	 * Checks if the child is set, i.e. it does not point to null.
	 *
	 * @return True iff the child is set.
	 */
	bool isChildSet() const {
		return child.get() != nullptr;
	}

	/*!
	 * Get the lower time bound.
	 *
	 * @return The lower time bound.
	 */
	T const & getLowerBound() const {
		return lowerBound;
	}

	/*!
	 * Get the upper time bound.
	 *
	 * @return The upper time bound.
	 */
	T const & getUpperBound() const {
		return upperBound;
	}

	/*!
	 * Set the time interval for the time bounded operator.
	 *
	 * @param lowerBound The new lower time bound.
	 * @param upperBound The new upper time bound.
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

	// The child node.
	std::shared_ptr<AbstractStateFormula<T>> child;

	// The lower time bound.
	T lowerBound;

	// The upper time bound.
	T upperBound;
};

} /* namespace csl */
} /* namespace property */
} /* namespace storm */

#endif /* STORM_FORMULA_CSL_TIMEBOUNDEDEVENTUALLY_H_ */
