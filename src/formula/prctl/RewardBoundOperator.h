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
#include "src/formula/ComparisonType.h"

namespace storm {
namespace property {
namespace prctl {

template <class T> class RewardBoundOperator;

/*!
 *  @brief Interface class for model checkers that support RewardBoundOperator.
 *
 *  All model checkers that support the formula class PathBoundOperator must inherit
 *  this pure virtual class.
 */
template <class T>
class IRewardBoundOperatorModelChecker {
    public:
        virtual storm::storage::BitVector checkRewardBoundOperator(const RewardBoundOperator<T>& obj) const = 0;
};

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
 *
 * @see AbstractStateFormula
 * @see AbstractPathFormula
 * @see ProbabilisticOperator
 * @see ProbabilisticNoBoundsOperator
 * @see AbstractPrctlFormula
 */
template<class T>
class RewardBoundOperator : public AbstractStateFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	RewardBoundOperator() : comparisonOperator(LESS), bound(0), pathFormula(nullptr){
		// Intentionally left empty
	}

	/*!
	 * Constructor for non-optimizing operator.
	 *
	 * @param comparisonOperator The relation for the bound.
	 * @param bound The bound for the probability
	 * @param pathFormula The child node
	 */
	RewardBoundOperator(storm::property::ComparisonType comparisonOperator, T bound, std::shared_ptr<AbstractRewardPathFormula<T>> const & pathFormula)
		: comparisonOperator(comparisonOperator), bound(bound), pathFormula(pathFormula) {
		// Intentionally left empty
	}

	/*!
	 * Destructor
	 *
	 * Deletes the subtree iff this object is the last remaining owner of the subtree.
	 */
	virtual ~RewardBoundOperator() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual std::shared_ptr<AbstractStateFormula<T>> clone() const override {
		std::shared_ptr<RewardBoundOperator<T>> result(new RewardBoundOperator<T>());
		result->setComparisonOperator(comparisonOperator);
		result->setBound(bound);
		result->setPathFormula(pathFormula->clone());
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
	 *  @brief Checks if the subtree conforms to some logic.
	 *
	 *  @param checker Formula checker object.
	 *  @return true iff the subtree conforms to some logic.
	 */
	virtual bool validate(AbstractFormulaChecker<T> const & checker) const override {
		return checker.validate(this->pathFormula);
	}

	/*!
	 * @returns a string representation of the formula
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
		result += pathFormula->toString();
		result += ")";
		return result;
	}

	/*!
	 * @returns the child node (representation of a formula)
	 */
	std::shared_ptr<AbstractRewardPathFormula<T>> const & getPathFormula () const {
		return pathFormula;
	}

	/*!
	 * Sets the child node
	 *
	 * @param pathFormula the path formula that becomes the new child node
	 */
	void setPathFormula(std::shared_ptr<AbstractRewardPathFormula<T>> const & pathFormula) {
		this->pathFormula = pathFormula;
	}

	/*!
	 *
	 * @return True if the path formula is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool pathFormulaIsSet() const {
		return pathFormula.get() != nullptr;
	}

	/*!
	 * @returns the comparison relation
	 */
	storm::property::ComparisonType const getComparisonOperator() const {
		return comparisonOperator;
	}

	void setComparisonOperator(storm::property::ComparisonType comparisonOperator) {
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
	void setBound(T bound) {
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
	storm::property::ComparisonType comparisonOperator;
	T bound;
	std::shared_ptr<AbstractRewardPathFormula<T>> pathFormula;
};

} //namespace prctl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_REWARDBOUNDOPERATOR_H_ */
