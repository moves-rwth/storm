/*
 * BoundOperator.h
 *
 *  Created on: 27.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_STATEBOUNDOPERATOR_H_
#define STORM_FORMULA_STATEBOUNDOPERATOR_H_

#include "src/formula/AbstractStateFormula.h"
#include "src/formula/AbstractPathFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include "src/modelchecker/AbstractModelChecker.h"
#include "src/utility/ConstTemplates.h"

namespace storm {
namespace formula {

template <class T> class StateBoundOperator;

/*!
 *  @brief Interface class for model checkers that support StateBoundOperator.
 *   
 *  All model checkers that support the formula class StateBoundOperator must inherit
 *  this pure virtual class.
 */
template <class T>
class IStateBoundOperatorModelChecker {
    public:
        virtual storm::storage::BitVector* checkStateBoundOperator(const StateBoundOperator<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for a Abstract formula tree with a P (probablistic) operator node over a probability interval
 * as root.
 *
 * Has one Abstract state formula as sub formula/tree.
 *
 * @par Semantics
 * 	  The formula holds iff the probability that the state formula holds is inside the bounds
 * 	  specified in this operator
 *
 * The subtree is seen as part of the object and deleted with it
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 *
 * @see AbstractStateFormula
 * @see AbstractPathFormula
 * @see ProbabilisticOperator
 * @see ProbabilisticNoBoundsOperator
 * @see AbstractFormula
 */
template<class T>
class StateBoundOperator : public AbstractStateFormula<T> {

public:
	enum ComparisonType { LESS, LESS_EQUAL, GREATER, GREATER_EQUAL };

	/*!
	 * Constructor
	 *
	 * @param comparisonOperator The relation for the bound.
	 * @param bound The bound for the probability
	 * @param stateFormula The child node
	 */
	StateBoundOperator(ComparisonType comparisonOperator, T bound, AbstractStateFormula<T>* stateFormula)
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
	 * @returns the child node (representation of a Abstract state formula)
	 */
	const AbstractStateFormula<T>& getStateFormula () const {
		return *stateFormula;
	}

	/*!
	 * Sets the child node
	 *
	 * @param stateFormula the state formula that becomes the new child node
	 */
	void setStateFormula(AbstractStateFormula<T>* stateFormula) {
		this->stateFormula = stateFormula;
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
	virtual std::string toString() const {
		std::string result = "";
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
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual AbstractStateFormula<T>* clone() const = 0;

	/*!
	 * Calls the model checker to check this formula.
	 * Needed to infer the correct type of formula class.
	 *
	 * @note This function should only be called in a generic check function of a model checker class. For other uses,
	 *       the methods of the model checker should be used.
	 *
	 * @returns A bit vector indicating all states that satisfy the formula represented by the called object.
	 */
	virtual storm::storage::BitVector *check(const storm::modelchecker::AbstractModelChecker<T>& modelChecker) const {
		return modelChecker.template as<IStateBoundOperatorModelChecker>()->checkStateBoundOperator(*this);
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
	ComparisonType comparisonOperator;
	T bound;
	AbstractStateFormula<T>* stateFormula;
};

} //namespace formula
} //namespace storm

#endif /* STORM_FORMULA_STATEBOUNDOPERATOR_H_ */
