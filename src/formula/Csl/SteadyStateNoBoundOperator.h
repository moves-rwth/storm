/*
 * SteadyStateNoBoundOperator.h
 *
 *  Created on: 09.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_STEADYSTATENOBOUNDOPERATOR_H_
#define STORM_FORMULA_STEADYSTATENOBOUNDOPERATOR_H_

#include "StateNoBoundOperator.h"

namespace storm {
namespace formula {

template <class T> class SteadyStateNoBoundOperator;

/*!
 *  @brief Interface class for model checkers that support SteadyStateOperator.
 *
 *  All model checkers that support the formula class SteadyStateOperator must inherit
 *  this pure virtual class.
 */
template <class T>
class ISteadyStateNoBoundOperatorModelChecker {
    public:
		/*!
         *  @brief Evaluates SteadyStateOperator formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T>*  checkSteadyStateNoBoundOperator(const SteadyStateNoBoundOperator<T>& obj) const = 0;
};

template <class T>
class SteadyStateNoBoundOperator: public storm::formula::StateNoBoundOperator<T> {
public:
	/*!
	 * Empty constructor
	 */
	SteadyStateNoBoundOperator() : StateNoBoundOperator<T>() {
		// Intentionally left empty

	}

	/*!
	 * Constructor
	 *
	 * @param stateFormula The state formula that forms the subtree
	 */
	SteadyStateNoBoundOperator(AbstractStateFormula<T>* stateFormula) : StateNoBoundOperator<T>(stateFormula) {
		// Intentionally left empty
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		return "S" + StateNoBoundOperator<T>::toString();
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedUntil-object that is identical the called object.
	 */
	/* TODO: Add clone method to StateNoBoundOperator and use matching return type
	virtual AbstractStateFormula<T>* clone() const {
		SteadyStateNoBoundOperator<T>* result = new SteadyStateNoBoundOperator<T>();
		result->setStateFormula(this->getStateFormula().clone());
		return result;
	}*/

	/*!
	 * Calls the model checker to check this formula.
	 * Needed to infer the correct type of formula class.
	 *
	 * @note This function should only be called in a generic check function of a model checker class. For other uses,
	 *       the methods of the model checker should be used.
	 *
	 * @returns A vector indicating the probability that the formula holds for each state.
	 */
	virtual std::vector<T>*  check(const storm::modelchecker::AbstractModelChecker<T>& modelChecker) const {
		return modelChecker.template as<ISteadyStateNoBoundOperatorModelChecker>()->checkSteadyStateNoBoundOperator(*this);
	}

};

} /* namespace formula */
} /* namespace storm */
#endif /* STORM_FORMULA_STEADYSTATENOBOUNDOPERATOR_H_ */
