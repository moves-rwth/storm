/*
 * TimeBoundedEventually.h
 *
 *  Created on: 10.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_CSL_TIMEBOUNDEDEVENTUALLY_H_
#define STORM_FORMULA_CSL_TIMEBOUNDEDEVENTUALLY_H_

#include "src/formula/abstract/TimeBoundedEventually.h"
#include "AbstractPathFormula.h"
#include "AbstractStateFormula.h"

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
        virtual std::vector<T>* checkTimeBoundedEventually(const TimeBoundedEventually<T>& obj, bool qualitative) const = 0;
};


template<class T>
class TimeBoundedEventually: public storm::property::abstract::TimeBoundedEventually<T, AbstractStateFormula<T>>,
									  public AbstractPathFormula<T> {
public:
	/**
	 * Simple constructor: Only sets the bounds
	 *
	 * @param lowerBound
	 * @param upperBound
	 */
	TimeBoundedEventually(T lowerBound, T upperBound)
		: storm::property::abstract::TimeBoundedEventually<T, AbstractStateFormula<T>>(lowerBound, upperBound) {
		// Intentionally left empty
	}

	TimeBoundedEventually(T lowerBound, T upperBound, AbstractStateFormula<T>* child)
		: storm::property::abstract::TimeBoundedEventually<T, AbstractStateFormula<T>>(lowerBound, upperBound, child) {
		// Intentionally left empty
	}

	virtual ~TimeBoundedEventually() {
		// Intentionally left empty
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
	virtual std::vector<T>* check(const storm::modelchecker::AbstractModelChecker<T>& modelChecker, bool qualitative) const {
		return modelChecker.template as<ITimeBoundedEventuallyModelChecker>()->checkTimeBoundedEventually(*this, qualitative);
	}
};

} /* namespace csl */
} /* namespace property */
} /* namespace storm */

#endif /* STORM_FORMULA_CSL_TIMEBOUNDEDEVENTUALLY_H_ */
