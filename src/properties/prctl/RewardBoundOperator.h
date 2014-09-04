/*
 * RewardBoundOperator.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PRCTL_REWARDBOUNDOPERATOR_H_
#define STORM_FORMULA_PRCTL_REWARDBOUNDOPERATOR_H_

#include "AbstractRewardPathFormula.h"
#include "AbstractStateFormula.h"
#include "utility/constants.h"
#include "src/properties/ComparisonType.h"

namespace storm {
namespace properties {
namespace prctl {

// Forward declaration for the interface class.
template <class T> class RewardBoundOperator;

/*!
 * Interface class for model checkers that support RewardBoundOperator.
 *
 * All model checkers that support the formula class PathBoundOperator must inherit
 * this pure virtual class.
 */
template <class T>
class IRewardBoundOperatorModelChecker {
    public:

		/*!
		 * Empty virtual destructor.
		 */
		virtual ~IRewardBoundOperatorModelChecker() {
			// Intentionally left empty
		}

		/*!
		 * Evaluates a RewardBoundOperator within a model checker.
		 *
		 * @param obj Formula object with subformulas.
		 * @return The modelchecking result of the formula for every state.
		 */
        virtual storm::storage::BitVector checkRewardBoundOperator(const RewardBoundOperator<T>& obj) const = 0;
};

/*!
 * Class for a Prctl formula tree with an R (reward) operator node as root.
 *
 * Has a reward path formula as sub formula/tree.
 *
 * @par Semantics
 * 	  The formula holds iff the reward of the reward path formula meets the bound
 * 	  specified in this operator.
 *
 * The object has shared ownership of its subtree. If this object is deleted and no other object has a shared
 * ownership of the subtree it will be deleted as well.
 *
 *
 * @see AbstractStateFormula
 * @see AbstractRewardPathFormula
 * @see AbstractPrctlFormula
 * @see ProbabilisticBoundOperator
 */
template<class T>
class RewardBoundOperator : public AbstractStateFormula<T> {

public:

	/*!
	 * Creates a RewardBoundOperator node without a subnode.
	 * The resulting object will not represent a complete formula!
	 */
	RewardBoundOperator() : comparisonOperator(LESS), bound(0), child(nullptr){
		// Intentionally left empty
	}

	/*!
	 * Creates a ProbabilisticBoundOperator node using the given parameters.
	 *
	 * @param comparisonOperator The relation for the bound.
	 * @param bound The bound for the rewards.
	 * @param child The child formula subtree.
	 */
	RewardBoundOperator(storm::properties::ComparisonType comparisonOperator, T bound, std::shared_ptr<AbstractRewardPathFormula<T>> const & child)
		: comparisonOperator(comparisonOperator), bound(bound), child(child) {
		// Intentionally left empty
	}

	/*!
	 * Empty virtual destructor.
	 */
	virtual ~RewardBoundOperator() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subnodes of the new object are clones of the original ones.
	 *
	 * @returns A new RewardBoundOperator object that is a deep copy of the called object.
	 */
	virtual std::shared_ptr<AbstractStateFormula<T>> clone() const override {
		std::shared_ptr<RewardBoundOperator<T>> result(new RewardBoundOperator<T>());
		result->setComparisonOperator(comparisonOperator);
		result->setBound(bound);
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
	 * @returns A bit vector indicating all states that satisfy the formula represented by the called object.
	 */
	virtual storm::storage::BitVector check(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelChecker) const override {
		return modelChecker.template as<IRewardBoundOperatorModelChecker>()->checkRewardBoundOperator(*this);
	}

	/*!
	 * Returns a textual representation of the formula tree with this node as root.
	 *
	 * @returns A string representing the formula tree.
	 */
	virtual std::string toString() const override {
		std::string result = "R ";
		switch (comparisonOperator) {
			case LESS: result += "<"; break;
			case LESS_EQUAL: result += "<="; break;
			case GREATER: result += ">"; break;
			case GREATER_EQUAL: result += ">="; break;
		}
		result += " ";
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
	std::shared_ptr<AbstractRewardPathFormula<T>> const & getChild () const {
		return child;
	}

	/*!
	 * Sets the subtree.
	 *
	 * @param child The new child.
	 */
	void setChild(std::shared_ptr<AbstractRewardPathFormula<T>> const & child) {
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
	storm::properties::ComparisonType const getComparisonOperator() const {
		return comparisonOperator;
	}

	/*!
	 * Sets the comparison operator.
	 *
	 * @param comparisonOperator An enum value representing the new comparison relation.
	 */
	void setComparisonOperator(storm::properties::ComparisonType comparisonOperator) {
		this->comparisonOperator = comparisonOperator;
	}

	/*!
	 * Gets the bound which is to be obeyed by the rewards of the reward path formula.
	 *
	 * @returns The probability bound.
	 */
	T const & getBound() const {
		return bound;
	}

	/*!
	 * Sets the bound which is to be obeyed by the rewards of the reward path formula
	 *
	 * @param bound The new reward bound.
	 */
	void setBound(T bound) {
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
	storm::properties::ComparisonType comparisonOperator;

	// The reward bound.
	T bound;

	// The reward path formula whose rewards have to meet the bound.
	std::shared_ptr<AbstractRewardPathFormula<T>> child;
};

} //namespace prctl
} //namespace properties
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_REWARDBOUNDOPERATOR_H_ */
