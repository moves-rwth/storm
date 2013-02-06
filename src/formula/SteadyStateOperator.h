/*
 * SteadyState.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_STEADYSTATEOPERATOR_H_
#define STORM_FORMULA_STEADYSTATEOPERATOR_H_

#include "AbstractPathFormula.h"
#include "AbstractStateFormula.h"
#include "BoundOperator.h"
#include "src/formula/AbstractFormulaChecker.h"

namespace storm {

namespace formula {

template <class T> class SteadyStateOperator;

/*!
 *  @brief Interface class for model checkers that support SteadyStateOperator.
 *   
 *  All model checkers that support the formula class SteadyStateOperator must inherit
 *  this pure virtual class.
 */
template <class T>
class ISteadyStateOperatorModelChecker {
    public:
		/*!
         *  @brief Evaluates SteadyStateOperator formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual storm::storage::BitVector* checkSteadyStateOperator(const SteadyStateOperator<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for a Abstract (path) formula tree with a SteadyStateOperator node as root.
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
class SteadyStateOperator : public BoundOperator<T> {

public:
	/*!
	 * Empty constructor
	 */
	SteadyStateOperator() : BoundOperator<T>
		(BoundOperator<T>::LESS_EQUAL, storm::utility::constGetZero<T>(), nullptr) {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param child The child node
	 */
	SteadyStateOperator(
		BoundOperator<T>::ComparisonType comparisonRelation, T bound, AbstractStateFormula<T>* child) {
		this->child = child;
	}

	/*!
	 * Constructor.
	 *
	 * Also deletes the subtree.
	 * (this behaviour can be prevented by setting the subtrees to NULL before deletion)
	 */
	virtual ~SteadyStateOperator() {
	  if (child != NULL) {
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
		std::string result = "(";
		result += " S ";
		result += child->toString();
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
	virtual AbstractPathFormula<T>* clone() const {
		SteadyStateOperator<T>* result = new SteadyStateOperator<T>();
		if (child != NULL) {
			result->setChild(child);
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
	virtual storm::storage::BitVector* check(const storm::modelChecker::AbstractModelChecker<T>& modelChecker) const {
		return modelChecker.template as<ISteadyStateOperatorModelChecker>()->checkSteadyStateOperator(*this);
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

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_STEADYSTATEOPERATOR_H_ */
