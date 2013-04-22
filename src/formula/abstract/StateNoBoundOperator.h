/*
 * StateNoBoundOperator.h
 *
 *  Created on: 09.04.2013
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACT_STATENOBOUNDOPERATOR_H_
#define STORM_FORMULA_ABSTRACT_STATENOBOUNDOPERATOR_H_

#include "AbstractFormula.h"
#include "src/formula/AbstractFormulaChecker.h"

#include "src/modelchecker/ForwardDeclarations.h"

namespace storm {
namespace formula {
namespace abstract {

/*!
 * @brief
 * Class for a Abstract formula tree with an operator without declaration of bounds.
 * as root.
 *
 * Checking a formula with this operator as root returns the probabilities that the path formula holds
 * (for each state)
 *
 * Has one formula as sub formula/tree.
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
 * @tparam FormulaType The type of the subformula.
 * 		  The instantiation of FormulaType should be a subclass of AbstractFormula, as the functions
 * 		  "toString" and "conforms" of the subformulas are needed.
 *
 * @see AbstractFormula
 * @see StateBoundOperator
 */
template <class T, class FormulaType>
class StateNoBoundOperator: public virtual AbstractFormula<T> {
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
	StateNoBoundOperator(FormulaType* stateFormula) {
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

	const FormulaType& getStateFormula() const {
		return *(this->stateFormula);
	}

	void setStateFormula(FormulaType* stateFormula) {
		this->stateFormula = stateFormula;
	}

	/*!
	 *
	 * @return True if the state formula is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool stateFormulaIsSet() const {
		return stateFormula != nullptr;
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
	FormulaType* stateFormula;
};

} //namespace abstract
} //namespace formula
} //namespace storm
#endif /* STORM_FORMULA_ABSTRACT_STATENOBOUNDOPERATOR_H_ */
