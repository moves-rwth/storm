/*
 * RewardBoundOperator.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACT_REWARDBOUNDOPERATOR_H_
#define STORM_FORMULA_ABSTRACT_REWARDBOUNDOPERATOR_H_

#include "PathBoundOperator.h"
#include "utility/constants.h"

namespace storm {
namespace property {
namespace abstract {

/*!
 * @brief
 * Class for an abstract formula tree with a R (reward) operator node over a reward interval as root.
 *
 * Has a reward path formula as sub formula/tree.
 *
 * @par Semantics
 * 	  The formula holds iff the reward of the reward path formula is inside the bounds
 * 	  specified in this operator
 *
 * The subtree is seen as part of the object and deleted with it
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @tparam FormulaType The type of the subformula.
 * 		  The instantiation of FormulaType should be a subclass of AbstractFormula, as the functions
 * 		  "toString" and "conforms" of the subformulas are needed.
 *
 * @see AbstractFormula
 * @see PathBoundOperator
 * @see RewardNoBoundOperator
 */
template<class T, class FormulaType>
class RewardBoundOperator : public PathBoundOperator<T, FormulaType> {

	// Throw a compiler error if FormulaType is not a subclass of AbstractFormula.
	static_assert(std::is_base_of<AbstractFormula<T>, FormulaType>::value,
				  "Instantiaton of FormulaType for storm::property::abstract::RewardBoundOperator<T,FormulaType> has to be a subtype of storm::property::abstract::AbstractFormula<T>");

public:
	/*!
	 * Empty constructor
	 */
	RewardBoundOperator() : PathBoundOperator<T, FormulaType>(LESS_EQUAL, storm::utility::constantZero<T>(), nullptr) {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param comparisonRelation The relation to compare the actual value and the bound
	 * @param bound The bound for the probability
	 * @param pathFormula The child node
	 */
	RewardBoundOperator(
			storm::property::ComparisonType comparisonRelation,
			T bound,
			FormulaType* pathFormula) :
				PathBoundOperator<T, FormulaType>(comparisonRelation, bound, pathFormula) {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 * @param comparisonRelation
	 * @param bound
	 * @param pathFormula
	 * @param minimumOperator
	 */
	RewardBoundOperator(
			storm::property::ComparisonType comparisonRelation,
			T bound,
			FormulaType* pathFormula,
			bool minimumOperator)
			: PathBoundOperator<T, FormulaType>(comparisonRelation, bound, pathFormula, minimumOperator) {
		// Intentionally left empty
	}

	/*!
	 * Destructor
	 */
	virtual ~RewardBoundOperator() {
		// Intentionally left empty
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
		std::string result = "R ";
		result += PathBoundOperator<T, FormulaType>::toString();
		return result;
	}
};

} //namespace abstract
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_REWARDBOUNDOPERATOR_H_ */
