/*
 * BoundOperator.h
 *
 *  Created on: 27.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_ABSTRACT_STATEBOUNDOPERATOR_H_
#define STORM_FORMULA_ABSTRACT_STATEBOUNDOPERATOR_H_

#include "src/formula/abstract/AbstractFormula.h"
#include "src/formula/abstract/AbstractFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include "src/formula/ComparisonType.h"
#include "src/utility/ConstTemplates.h"

namespace storm {
namespace property {
namespace abstract {

/*!
 * @brief
 * Class for an abstract formula tree with a P (probablistic) operator node over a probability interval
 * as root.
 *
 * Has one formula as sub formula/tree.
 *
 * @par Semantics
 * 	  The formula holds iff the probability that the state formula holds is inside the bounds
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
 * @see StateNoBoundOperator
 */
template<class T, class FormulaType>
class StateBoundOperator : public virtual AbstractFormula<T> {

	// Throw a compiler error if FormulaType is not a subclass of AbstractFormula.
	static_assert(std::is_base_of<AbstractFormula<T>, FormulaType>::value,
				  "Instantiaton of FormulaType for storm::property::abstract::StateBoundOperator<T,FormulaType> has to be a subtype of storm::property::abstract::AbstractFormula<T>");

public:

	/*!
	 * Constructor
	 *
	 * @param comparisonOperator The relation for the bound.
	 * @param bound The bound for the probability
	 * @param stateFormula The child node
	 */
	StateBoundOperator(storm::property::ComparisonType comparisonOperator, T bound, FormulaType* stateFormula)
		: comparisonOperator(comparisonOperator), bound(bound), stateFormula(stateFormula) {
		// Intentionally left empty
	}

	/*!
	 * Destructor
	 *
	 * The subtree is deleted with the object
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~StateBoundOperator() {
	 if (stateFormula != nullptr) {
		 delete stateFormula;
	 }
	}

	/*!
	 * @returns the child node (representation of a formula)
	 */
	const FormulaType& getStateFormula () const {
		return *stateFormula;
	}

	/*!
	 * Sets the child node
	 *
	 * @param stateFormula the state formula that becomes the new child node
	 */
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
	 * @returns the comparison relation
	 */
	const ComparisonType getComparisonOperator() const {
		return comparisonOperator;
	}

	void setComparisonOperator(ComparisonType comparisonOperator) {
		this->comparisonOperator = comparisonOperator;
	}

	/*!
	 * @returns the bound for the measure
	 */
	const T& getBound() const {
		return bound;
	}

	/*!
	 * Sets the interval in which the probability that the path formula holds may lie in.
	 *
	 * @param bound The bound for the measure
	 */
	void setBound(T bound) {
		this->bound = bound;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
		std::string result = " ";
		switch (comparisonOperator) {
		case LESS: result += "< "; break;
		case LESS_EQUAL: result += "<= "; break;
		case GREATER: result += "> "; break;
		case GREATER_EQUAL: result += ">= "; break;
		}
		result += std::to_string(bound);
		result += " [";
		result += stateFormula->toString();
		result += "]";
		return result;
	}

	bool meetsBound(T value) const {
		switch (comparisonOperator) {
		case LESS: return value < bound; break;
		case LESS_EQUAL: return value <= bound; break;
		case GREATER: return value > bound; break;
		case GREATER_EQUAL: return value >= bound; break;
		default: return false;
		}
	}

	/*!
     *  @brief Checks if the subtree conforms to some logic.
     * 
     *  @param checker Formula checker object.
     *  @return true iff the subtree conforms to some logic.
     */
	virtual bool validate(const AbstractFormulaChecker<T>& checker) const override {
        return checker.validate(this->stateFormula);
    }

private:
	ComparisonType comparisonOperator;
	T bound;
	FormulaType* stateFormula;
};

} //namespace abstract
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_STATEBOUNDOPERATOR_H_ */
