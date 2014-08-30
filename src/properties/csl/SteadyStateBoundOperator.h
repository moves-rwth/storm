/*
 * SteadyState.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_CSL_STEADYSTATEOPERATOR_H_
#define STORM_FORMULA_CSL_STEADYSTATEOPERATOR_H_

#include "AbstractStateFormula.h"
#include "src/properties/ComparisonType.h"

namespace storm {
namespace properties {
namespace csl {

// Forward declaration for the interface class.
template <class T> class SteadyStateBoundOperator;

/*!
 * Interface class for model checkers that support SteadyStateOperator.
 *   
 * All model checkers that support the formula class SteadyStateOperator must inherit
 * this pure virtual class.
 */
template <class T>
class ISteadyStateBoundOperatorModelChecker {
    public:

		/*!
		 * Empty virtual destructor.
		 */
		virtual ~ISteadyStateBoundOperatorModelChecker() {
			// Intentionally left empty
		}

		/*!
         * Evaluates a SteadyStateOperator formula within a model checker.
         *
         * @param obj Formula object with subformulas.
         * @return The modelchecking result of the formula for every state.
         */
        virtual storm::storage::BitVector checkSteadyStateBoundOperator(const SteadyStateBoundOperator<T>& obj) const = 0;
};

/*!
 * Class for a Csl formula tree with a SteadyStateOperator node as root.
 *
 * Has two state formulas as sub formulas/trees.
 *
 * @par Semantics
 * The formula holds iff the long-run probability of being in a state satisfying \e child meets the \e bound specified in this operator.
 *
 * The object has shared ownership of its subtree. If this object is deleted and no other object has a shared
 * ownership of the subtree it will be deleted as well.
 *
 * @see AbstractPathFormula
 * @see AbstractCslFormula
 */
template <class T>
class SteadyStateBoundOperator : public AbstractStateFormula<T> {

public:

	/*!
	 * Creates a SteadyStateBoundOperator node without a subnode.
	 * The resulting object will not represent a complete formula!
	 */
	SteadyStateBoundOperator() : comparisonOperator(LESS), bound(storm::utility::constantZero<T>()), child(nullptr) {
		// Intentionally left empty
	}

	/*!
	 * Creates a SteadyStateBoundOperator node using the given parameters.
	 *
	 * @param comparisonOperator The relation for the bound.
	 * @param bound The bound for the probability.
	 * @param child The child formula subtree.
	 */
	SteadyStateBoundOperator(storm::properties::ComparisonType comparisonOperator, T bound, std::shared_ptr<AbstractStateFormula<T>> const & child)
		: comparisonOperator(comparisonOperator), bound(bound), child(child) {
		// Intentionally left empty
	}

	/*!
	 * Empty virtual destructor.
	 */
	virtual ~SteadyStateBoundOperator() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subnodes of the new object are clones of the original ones.
	 *
	 * @returns A new SteadyStateBoundOperator object that is a deep copy of the called object.
	 */
	virtual std::shared_ptr<AbstractStateFormula<T>> clone() const override {
		std::shared_ptr<SteadyStateBoundOperator<T>> result(new SteadyStateBoundOperator<T>());
		result->setChild(child->clone());
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
	 * Returns a textual representation of the formula tree with this node as root.
	 *
	 * @returns A string representing the formula tree.
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
		result += child->toString();
		result += ")";
		return result;
	}

	/*!
	 * Gets the child node.
	 *
	 * @returns The child node.
	 */
	std::shared_ptr<AbstractStateFormula<T>> const & getChild () const {
		return child;
	}

	/*!
	 * Sets the subtree.
	 *
	 * @param child The new child.
	 */
	void setChild(std::shared_ptr<AbstractStateFormula<T>> const & child) {
		this->child = child;
	}

	/*!
	 * Checks if the child is set, i.e. it does not point to null.
	 *
	 * @return True iff the child is set.
	 */
	bool isChildSet() const {
		return child.get() != nullptr;
	}

	/*!
	 * Gets the comparison operator.
	 *
	 * @returns An enum value representing the comparison relation.
	 */
	ComparisonType const getComparisonOperator() const {
		return comparisonOperator;
	}

	/*!
	 * Sets the comparison operator.
	 *
	 * @param comparisonOperator An enum value representing the new comparison relation.
	 */
	void setComparisonOperator(ComparisonType comparisonOperator) {
		this->comparisonOperator = comparisonOperator;
	}

	/*!
	 * Gets the bound which the steady state probability has to obey.
	 *
	 * @returns The probability bound.
	 */
	T const & getBound() const {
		return bound;
	}

	/*!
	 * Sets the bound which the steady state probability has to obey.
	 *
	 * @param bound The new probability bound.
	 */
	void setBound(T const & bound) {
		this->bound = bound;
	}

	/*!
	 * Checks if the bound is met by the given value.
	 *
	 * @param value The value to test against the bound.
	 * @returns True iff value <comparisonOperator> bound holds.
	 */
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

	// The operator used to indicate the kind of bound that is to be met.
	ComparisonType comparisonOperator;

	// The probability bound.
	T bound;

	// The state formula for whose state the long-run probability has to meet the bound.
	std::shared_ptr<AbstractStateFormula<T>> child;
};

} //namespace csl
} //namespace properties
} //namespace storm

#endif /* STORM_FORMULA_CSL_STEADYSTATEOPERATOR_H_ */
