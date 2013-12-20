/*
 * SteadyState.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACT_STEADYSTATEOPERATOR_H_
#define STORM_FORMULA_ABSTRACT_STEADYSTATEOPERATOR_H_

#include "StateBoundOperator.h"
#include "src/formula/AbstractFormulaChecker.h"

namespace storm {
namespace property {
namespace abstract {

/*!
 * @brief
 * Class for an Abstract (path) formula tree with a SteadyStateOperator node as root.
 *
 * Has two formulas as sub formulas/trees.
 *
 * @par Semantics
 * The formula holds iff \e child holds  SteadyStateOperator step, \e child holds
 *
 * The subtree is seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractFormula
 */
template <class T, class FormulaType>
class SteadyStateBoundOperator : public StateBoundOperator<T, FormulaType> {

	// Throw a compiler error if FormulaType is not a subclass of AbstractFormula.
	static_assert(std::is_base_of<AbstractFormula<T>, FormulaType>::value,
				  "Instantiaton of FormulaType for storm::property::abstract::SteadyStateBoundOperator<T,FormulaType> has to be a subtype of storm::property::abstract::AbstractFormula<T>");

public:
	/*!
	 * Empty constructor
	 */
	SteadyStateBoundOperator() : StateBoundOperator<T, FormulaType>
		(LESS_EQUAL, storm::utility::constantZero<T>(), nullptr) {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param stateFormula The child node
	 */
	SteadyStateBoundOperator(
		storm::property::ComparisonType comparisonRelation, T bound, FormulaType* stateFormula) :
			StateBoundOperator<T, FormulaType>(comparisonRelation, bound, stateFormula) {
	}

	/*!
	 * Destructor
	 */
	virtual ~SteadyStateBoundOperator() {
		// Intentionally left empty
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
		return "S" + StateBoundOperator<T, FormulaType>::toString();
	}
};

} //namespace abstract
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_STEADYSTATEOPERATOR_H_ */
