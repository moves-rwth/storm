/*
 * TimeBoundedUntil.h
 *
 *  Created on: 10.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_CSL_TIMEBOUNDEDUNTIL_H_
#define STORM_FORMULA_CSL_TIMEBOUNDEDUNTIL_H_

#include "AbstractPathFormula.h"
#include "AbstractStateFormula.h"
#include "src/formula/abstract/TimeBoundedUntil.h"

namespace storm {
namespace property {
namespace csl {

template <class T> class TimeBoundedUntil;

/*!
 *  @brief Interface class for model checkers that support TimeBoundedUntil.
 *
 *  All model checkers that support the formula class BoundedEventually must inherit
 *  this pure virtual class.
 */
template <class T>
class ITimeBoundedUntilModelChecker {
    public:
		/*!
         *  @brief Evaluates TimeBoundedUntil formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T> checkTimeBoundedUntil(const TimeBoundedUntil<T>& obj, bool qualitative) const = 0;
};

template <class T>
class TimeBoundedUntil: public storm::property::abstract::TimeBoundedUntil<T, AbstractStateFormula<T>>,
								public AbstractPathFormula<T> {
public:
	/**
	 * Constructor providing bounds only;
	 * Sub formulas are set to null.
	 *
	 * @param lowerBound
	 * @param upperBound
	 */
	TimeBoundedUntil(T lowerBound, T upperBound) :
		storm::property::abstract::TimeBoundedUntil<T, AbstractStateFormula<T>>(lowerBound, upperBound) {
		// Intentionally left empty
	}

	/**
	 * Full constructor
	 * @param lowerBound
	 * @param upperBound
	 * @param left
	 * @param right
	 */
	TimeBoundedUntil(T lowerBound, T upperBound, AbstractStateFormula<T>* left, AbstractStateFormula<T>* right) :
		storm::property::abstract::TimeBoundedUntil<T, AbstractStateFormula<T>>(lowerBound, upperBound, left, right) {

	}

	/*!
	 * Destructor
	 */
	virtual ~TimeBoundedUntil() {
	   // Intentionally left empty
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedUntil-object that is identical the called object.
	 */
	virtual AbstractPathFormula<T>* clone() const override {
		TimeBoundedUntil<T>* result = new TimeBoundedUntil<T>(this->getLowerBound(), this->getUpperBound());
		if (this->leftIsSet()) {
			result->setLeft(this->getLeft().clone());
		}
		if (this->rightIsSet()) {
			result->setRight(this->getRight().clone());
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
		return modelChecker.template as<ITimeBoundedUntilModelChecker>()->checkTimeBoundedUntil(*this, qualitative);
	}
};

} /* namespace csl */
} /* namespace property */
} /* namespace storm */

#endif /* STORM_FORMULA_CSL_TIMEBOUNDEDUNTIL_H_ */
