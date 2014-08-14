/*
 * SteadyState.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_CSL_STEADYSTATEOPERATOR_H_
#define STORM_FORMULA_CSL_STEADYSTATEOPERATOR_H_

#include "AbstractStateFormula.h"
#include "src/formula/ComparisonType.h"

namespace storm {
namespace property {
namespace csl {

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
        virtual storm::storage::BitVector checkSteadyStateBoundOperator(const SteadyStateBoundOperator<T>& obj) const = 0;
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
 * @see AbstractCslFormula
 */
template <class T>
class SteadyStateBoundOperator : public AbstractStateFormula<T> {

public:

	/*!
	 * Empty constructor
	 */
	SteadyStateBoundOperator() : comparisonOperator(LESS), bound(storm::utility::constantZero<T>()), stateFormula(nullptr) {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param comparisonOperator The relation for the bound.
	 * @param bound The bound for the probability
	 * @param stateFormula The child node
	 */
	SteadyStateBoundOperator(storm::property::ComparisonType comparisonOperator, T bound, std::shared_ptr<AbstractStateFormula<T>> const & stateFormula)
		: comparisonOperator(comparisonOperator), bound(bound), stateFormula(stateFormula) {
		// Intentionally left empty
	}

	/*!
	 * Destructor
	 *
	 * The subtree is deleted with the object
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~SteadyStateBoundOperator() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedUntil-object that is identical the called object.
	 */
	virtual std::shared_ptr<AbstractStateFormula<T>> clone() const override {
		std::shared_ptr<SteadyStateBoundOperator<T>> result(new SteadyStateBoundOperator<T>());
		result->setStateFormula(stateFormula->clone());
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
	virtual storm::storage::BitVector check(storm::modelchecker::csl::AbstractModelChecker<T> const & modelChecker) const override {
		return modelChecker.template as<ISteadyStateBoundOperatorModelChecker>()->checkSteadyStateBoundOperator(*this);
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
		std::string result = "S ";
		switch (comparisonOperator) {
		case LESS: result += "< "; break;
		case LESS_EQUAL: result += "<= "; break;
		case GREATER: result += "> "; break;
		case GREATER_EQUAL: result += ">= "; break;
		}
		result += std::to_string(bound);
		result += " (";
		result += stateFormula->toString();
		result += ")";
		return result;
	}

	/*!
	 * @returns the child node (representation of a formula)
	 */
	std::shared_ptr<AbstractStateFormula<T>> const & getStateFormula () const {
		return stateFormula;
	}

	/*!
	 * Sets the child node
	 *
	 * @param stateFormula the state formula that becomes the new child node
	 */
	void setStateFormula(std::shared_ptr<AbstractStateFormula<T>> const & stateFormula) {
		this->stateFormula = stateFormula;
	}

	/*!
	 *
	 * @return True if the state formula is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool stateFormulaIsSet() const {
		return stateFormula.get() != nullptr;
	}

	/*!
	 * @returns the comparison relation
	 */
	ComparisonType const getComparisonOperator() const {
		return comparisonOperator;
	}

	void setComparisonOperator(ComparisonType comparisonOperator) {
		this->comparisonOperator = comparisonOperator;
	}

	/*!
	 * @returns the bound for the measure
	 */
	T const & getBound() const {
		return bound;
	}

	/*!
	 * Sets the interval in which the probability that the path formula holds may lie in.
	 *
	 * @param bound The bound for the measure
	 */
	void setBound(T const & bound) {
		this->bound = bound;
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

private:
	ComparisonType comparisonOperator;
	T bound;
	std::shared_ptr<AbstractStateFormula<T>> stateFormula;
};

} //namespace csl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_CSL_STEADYSTATEOPERATOR_H_ */
