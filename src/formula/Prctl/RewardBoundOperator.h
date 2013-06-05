/*
 * RewardBoundOperator.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PRCTL_REWARDBOUNDOPERATOR_H_
#define STORM_FORMULA_PRCTL_REWARDBOUNDOPERATOR_H_

#include "AbstractPathFormula.h"
#include "AbstractStateFormula.h"
#include "src/formula/abstract/RewardBoundOperator.h"
#include "utility/ConstTemplates.h"

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
        virtual storm::storage::BitVector* checkRewardBoundOperator(const RewardBoundOperator<T>& obj) const = 0;
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
class RewardBoundOperator : public storm::property::abstract::RewardBoundOperator<T, AbstractPathFormula<T>>,
									 public AbstractStateFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	RewardBoundOperator() {
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
			AbstractPathFormula<T>* pathFormula) :
				storm::property::abstract::RewardBoundOperator<T, AbstractPathFormula<T>>(comparisonRelation, bound, pathFormula) {
		// Intentionally left empty
	}


	/*!
	 * Constructor
	 *
	 * @param comparisonRelation
	 * @param bound
	 * @param pathFormula
	 * @param minimumOperator
	 */
	RewardBoundOperator(
			storm::property::ComparisonType comparisonRelation,
			T bound,
			AbstractPathFormula<T>* pathFormula,
			bool minimumOperator)
			: storm::property::abstract::RewardBoundOperator<T, AbstractPathFormula<T>>(comparisonRelation, bound, pathFormula, minimumOperator) {
		// Intentionally left empty
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual AbstractStateFormula<T>* clone() const override {
		RewardBoundOperator<T>* result = new RewardBoundOperator<T>();
		result->setComparisonOperator(this->getComparisonOperator());
		result->setBound(this->getBound());
		result->setPathFormula(this->getPathFormula().clone());
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
	virtual storm::storage::BitVector* check(const storm::modelchecker::prctl::AbstractModelChecker<T>& modelChecker) const override {
		return modelChecker.template as<IRewardBoundOperatorModelChecker>()->checkRewardBoundOperator(*this);
	}
};

} //namespace prctl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_REWARDBOUNDOPERATOR_H_ */
