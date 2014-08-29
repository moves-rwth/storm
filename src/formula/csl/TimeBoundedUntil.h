/*
 * TimeBoundedUntil.h
 *
 *  Created on: 10.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_CSL_TIMEBOUNDEDUNTIL_H_
#define STORM_FORMULA_CSL_TIMEBOUNDEDUNTIL_H_

#include "src/formula/csl/AbstractPathFormula.h"
#include "src/formula/csl/AbstractStateFormula.h"

namespace storm {
namespace property {
namespace csl {

// Forward declaration for the interface class.
template <class T> class TimeBoundedUntil;

/*!
 * Interface class for model checkers that support TimeBoundedUntil.
 *
 * All model checkers that support the formula class TimeBoundedUntil must inherit
 * this pure virtual class.
 */
template <class T>
class ITimeBoundedUntilModelChecker {
    public:

		/*!
		 * Empty virtual destructor.
		 */
		virtual ~ITimeBoundedUntilModelChecker() {
			// Intentionally left empty
		}

		/*!
		 * Evaluates a TimeBoundedUntil formula within a model checker.
		 *
		 * @param obj Formula object with subformulas.
		 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
		 *                    results are only compared against the bounds 0 and 1.
		 * @return The modelchecking result of the formula for every state.
		 */
        virtual std::vector<T> checkTimeBoundedUntil(const TimeBoundedUntil<T>& obj, bool qualitative) const = 0;
};


/*!
 * Class for a Csl (path) formula tree with a TimeBoundedUntil node as root.
 *
 * Has two state formulas as subformulas/trees.
 *
 * @par Semantics
 * The formula holds iff formula \e right holds within the given time interval [lowerBound, upperBound] and \e left holds
 * in each point in time before that.
 *
 * The object has shared ownership of its subtree. If this object is deleted and no other object has a shared
 * ownership of the subtree it will be deleted as well.
 *
 * @see AbstractPathFormula
 * @see AbstractCslFormula
 */
template <class T>
class TimeBoundedUntil: public AbstractPathFormula<T> {
public:

	/*!
	 * Creates a TimeBoundedUntil node without a subnode.
	 * The resulting object will not represent a complete formula!
	 */
	TimeBoundedUntil() : lowerBound(0), upperBound(0), left(nullptr), right(nullptr) {
		setInterval(lowerBound, upperBound);
	}

	/*!
	 * Creates a TimeBoundedUntil node using the given parameters.
	 *
	 * @param lowerBound The lower bound of the admissable time interval.
	 * @param upperBound The upper bound of the admissable time interval.
	 * @param child The child formula subtree.
	 */
	TimeBoundedUntil(T lowerBound, T upperBound, std::shared_ptr<AbstractStateFormula<T>> const & left, std::shared_ptr<AbstractStateFormula<T>> const & right) : left(left), right(right) {
		setInterval(lowerBound, upperBound);
	}

	/*!
	 * Empty virtual destructor.
	 */
	virtual ~TimeBoundedUntil() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subnodes of the new object are clones of the original ones.
	 *
	 * @returns A new TimeBoundedUntil object that is a deep copy of the called object.
	 */
	virtual std::shared_ptr<AbstractPathFormula<T>> clone() const override {
		std::shared_ptr<TimeBoundedUntil<T>> result(new TimeBoundedUntil<T>(lowerBound, upperBound));
		if (this->isLeftSet()) {
			result->setLeft(left->clone());
		}
		if (this->isRightSet()) {
			result->setRight(right->clone());
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
		return modelChecker.template as<ITimeBoundedUntilModelChecker>()->checkTimeBoundedUntil(*this, qualitative);
	}

	/*!
	 * Returns a textual representation of the formula tree with this node as root.
	 *
	 * @returns A string representing the formula tree.
	 */
	virtual std::string toString() const override {
		std::string result = left->toString();
		result += " U";
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
		result += right->toString();
		return result;
	}

	/*!
	 * Gets the left child node.
	 *
	 * @returns The left child node.
	 */
	std::shared_ptr<AbstractStateFormula<T>> const & getLeft() const {
		return left;
	}

	/*!
	 * Gets the right child node.
	 *
	 * @returns The right child node.
	 */
	std::shared_ptr<AbstractStateFormula<T>> const & getRight() const {
		return right;
	}

	/*!
	 * Sets the left child node.
	 *
	 * @param newLeft The new left child.
	 */
	void setLeft(std::shared_ptr<AbstractStateFormula<T>> const & newLeft) {
		left = newLeft;
	}

	/*!
	 * Sets the right child node.
	 *
	 * @param newRight The new right child.
	 */
	void setRight(std::shared_ptr<AbstractStateFormula<T>> const & newRight) {
		right = newRight;
	}

	/*!
	 * Checks if the left child is set, i.e. it does not point to null.
	 *
	 * @return True iff the left child is set.
	 */
	bool isLeftSet() const {
		return left.get() != nullptr;
	}

	/*!
	 * Checks if the right child is set, i.e. it does not point to null.
	 *
	 * @return True iff the right child is set.
	 */
	bool isRightSet() const {
		return right.get() != nullptr;
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

	/**
	 * Set the time interval for the time bounded operator
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

	// The left child node.
	std::shared_ptr<AbstractStateFormula<T>> left;

	// The right child node.
	std::shared_ptr<AbstractStateFormula<T>> right;

	// The lower time bound.
	T lowerBound;

	// The upper time bound.
	T upperBound;
};

} /* namespace csl */
} /* namespace property */
} /* namespace storm */

#endif /* STORM_FORMULA_CSL_TIMEBOUNDEDUNTIL_H_ */
