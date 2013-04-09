/*
 * SteadyState.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_STEADYSTATEOPERATOR_H_
#define STORM_FORMULA_STEADYSTATEOPERATOR_H_

#include "src/formula/AbstractPathFormula.h"
#include "src/formula/AbstractStateFormula.h"
#include "src/formula/StateBoundOperator.h"
#include "src/formula/AbstractFormulaChecker.h"

namespace storm {

namespace formula {

template <class T> class SteadyStateBoundOperator;

/*!
 *  @brief Interface class for model checkers that support SteadyStateOperator.
 *   
 *  All model checkers that support the formula class SteadyStateOperator must inherit
 *  this pure virtual class.
 */
template <class T>
class ISteadyStateBoundOperatorModelChecker {
    public:
		/*!
         *  @brief Evaluates SteadyStateOperator formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual storm::storage::BitVector* checkSteadyStateBoundOperator(const SteadyStateBoundOperator<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for an Abstract (path) formula tree with a SteadyStateOperator node as root.
 *
 * Has two Abstract state formulas as sub formulas/trees.
 *
 * @par Semantics
 * The formula holds iff \e child holds  SteadyStateOperator step, \e child holds
 *
 * The subtree is seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractPathFormula
 * @see AbstractFormula
 */
template <class T>
class SteadyStateBoundOperator : public StateBoundOperator<T> {

public:
	/*!
	 * Empty constructor
	 */
	SteadyStateBoundOperator() : StateBoundOperator<T>
		(StateBoundOperator<T>::LESS_EQUAL, storm::utility::constGetZero<T>(), nullptr) {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param stateFormula The child node
	 */
	SteadyStateBoundOperator(
		typename StateBoundOperator<T>::ComparisonType comparisonRelation, T bound, AbstractStateFormula<T>* stateFormula) :
			StateBoundOperator<T>(comparisonRelation, bound, stateFormula) {
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "(";
		result += " S ";
		result += StateBoundOperator<T>::toString();
		result += ")";
		return result;
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedUntil-object that is identical the called object.
	 */
	virtual AbstractStateFormula<T>* clone() const {
		SteadyStateBoundOperator<T>* result = new SteadyStateBoundOperator<T>();
		result->setStateFormula(this->getStateFormula().clone());
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
	virtual storm::storage::BitVector* check(const storm::modelchecker::AbstractModelChecker<T>& modelChecker) const {
		return modelChecker.template as<ISteadyStateBoundOperatorModelChecker>()->checkSteadyStateBoundOperator(*this);
	}
	
};

} //namespace formula
} //namespace storm

#endif /* STORM_FORMULA_STEADYSTATEOPERATOR_H_ */
