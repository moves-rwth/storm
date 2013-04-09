/*
 * StateNoBoundOperator.h
 *
 *  Created on: 09.04.2013
 *      Author: Thomas Heinemann
 */

#ifndef STATENOBOUNDOPERATOR_H_
#define STATENOBOUNDOPERATOR_H_

#include "src/formula/AbstractFormula.h"
#include "src/formula/AbstractPathFormula.h"
#include "src/formula/AbstractFormulaChecker.h"

#include "src/modelchecker/ForwardDeclarations.h"

namespace storm {
namespace formula {

template <class T> class StateNoBoundOperator;

/*!
 *  @brief Interface class for model checkers that support PathNoBoundOperator.
 *
 *  All model checkers that support the formula class NoBoundOperator must inherit
 *  this pure virtual class.
 */
template <class T>
class IStateNoBoundOperatorModelChecker {
    public:
		/*!
         *  @brief Evaluates NoBoundOperator formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T>* checkStateNoBoundOperator(const StateNoBoundOperator<T>& obj) const = 0;
};


/*!
 * @brief
 * Class for a Abstract formula tree with an operator without declaration of probabilities
 * as root.
 *
 * Checking a formula with this operator as root returns the probabilities that the path formula holds
 * (for each state)
 *
 * Has one Abstract state formula as sub formula/tree.
 *
 * @note
 * 	This class is a hybrid of a state and path formula, and may only appear as the outermost operator.
 * 	Hence, it is seen as neither a state nor a path formula, but is directly derived from AbstractFormula.
 *
 * @note
 * 	This class does not contain a check() method like the other formula classes.
 * 	The check method should only be called by the model checker to infer the correct check function for sub
 * 	formulas. As this operator can only appear at the root, the method is not useful here.
 * 	Use the checkProbabilisticNoBoundOperator method from the DtmcPrctlModelChecker class instead.
 *
 * The subtree is seen as part of the object and deleted with it
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 *
 * @see AbstractStateFormula
 * @see AbstractPathFormula
 * @see SteadyStateNoBoundOperator
 * @see AbstractFormula
 */
template <class T>
class StateNoBoundOperator: public storm::formula::AbstractFormula<T> {
public:
	/*!
	 * Empty constructor
	 */
	StateNoBoundOperator() {
		stateFormula = nullptr;
	}

	/*!
	 * Constructor
	 */
	StateNoBoundOperator(AbstractStateFormula<T>* stateFormula) {
		this->stateFormula = stateFormula;
	}

	/*!
	 * Destructor
	 *
	 * Deletes the subtree
	 */
	virtual ~StateNoBoundOperator() {
		if (stateFormula != nullptr) {
			delete stateFormula;
		}
	}

	const AbstractStateFormula<T>& getStateFormula() const {
		return *(this->stateFormula);
	}

	void setStateFormula(AbstractStateFormula<T>* stateFormula) {
		this->stateFormula = stateFormula;
	}

	/*!
	 * Calls the model checker to check this formula.
	 * Needed to infer the correct type of formula class.
	 *
	 * @note This function should only be called in a generic check function of a model checker class. For other uses,
	 *       the methods of the model checker should be used.
	 *
	 * @note This function is not implemented in this class.
	 *
	 * @returns A vector indicating all states that satisfy the formula represented by the called object.
	 */
	virtual std::vector<T>* check(const storm::modelchecker::AbstractModelChecker<T>& modelChecker) const {
		return modelChecker.template as<IStateNoBoundOperatorModelChecker>()->checkStateNoBoundOperator(*this);
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result;
		result += " = ? [";
		result += this->getStateFormula().toString();
		result += "]";
		return result;
	}

	/*!
     *  @brief Checks if the subtree conforms to some logic.
     *
     *  @param checker Formula checker object.
     *  @return true iff the subtree conforms to some logic.
     */
	virtual bool conforms(const AbstractFormulaChecker<T>& checker) const {
		return checker.conforms(this->stateFormula);
	}

private:
	AbstractStateFormula<T>* stateFormula;
};

} /* namespace formula */
} /* namespace storm */
#endif /* STATENOBOUNDOPERATOR_H_ */
