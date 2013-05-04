/*
 * SteadyStateNoBoundOperator.h
 *
 *  Created on: 09.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_ABSTRACT_STEADYSTATENOBOUNDOPERATOR_H_
#define STORM_FORMULA_ABSTRACT_STEADYSTATENOBOUNDOPERATOR_H_

#include "StateNoBoundOperator.h"

namespace storm {
namespace property {
namespace abstract {

/*!
 * @brief
 * Class for an abstract formula tree with a steady state operator as root, without explicit declaration of bounds.
 *
 * Checking a formula with this operator as root returns the exact bound parameter for the corresponding subformula.
 * (for each state)
 *
 * Has one formula as sub formula/tree.
 *
 * @tparam FormulaType The type of the subformula.
 * 		  The instantiation of FormulaType should be a subclass of AbstractFormula, as the functions
 * 		  "toString" and "conforms" of the subformulas are needed.
 */
template <class T, class FormulaType>
class SteadyStateNoBoundOperator: public StateNoBoundOperator<T, FormulaType> {
public:
	/*!
	 * Empty constructor
	 */
	SteadyStateNoBoundOperator() : StateNoBoundOperator<T, FormulaType>() {
		// Intentionally left empty

	}

	/*!
	 * Constructor
	 *
	 * @param stateFormula The state formula that forms the subtree
	 */
	SteadyStateNoBoundOperator(FormulaType* stateFormula)
		: StateNoBoundOperator<T, FormulaType>(stateFormula) {
		// Intentionally left empty
	}

	/*!
	 * Destructor
	 */
	virtual ~SteadyStateNoBoundOperator() {
		// Intentionally left empty
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		return "S" + StateNoBoundOperator<T, FormulaType>::toString();
	}

};

} /* namespace abstract */
} /* namespace property */
} /* namespace storm */

#endif /* STORM_FORMULA_ABSTRACT_STEADYSTATENOBOUNDOPERATOR_H_ */
