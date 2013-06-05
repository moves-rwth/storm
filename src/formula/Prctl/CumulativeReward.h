/*
 * InstantaneousReward.h
 *
 *  Created on: 26.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_PRCTL_CUMULATIVEREWARD_H_
#define STORM_FORMULA_PRCTL_CUMULATIVEREWARD_H_

#include "AbstractPathFormula.h"
#include "AbstractStateFormula.h"
#include "src/formula/abstract/CumulativeReward.h"
#include "src/formula/AbstractFormulaChecker.h"
#include <string>

namespace storm {
namespace property {
namespace prctl {

template <class T> class CumulativeReward;

/*!
 *  @brief Interface class for model checkers that support CumulativeReward.
 *
 *  All model checkers that support the formula class CumulativeReward must inherit
 *  this pure virtual class.
 */
template <class T>
class ICumulativeRewardModelChecker {
    public:
		/*!
         *  @brief Evaluates CumulativeReward formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T>* checkCumulativeReward(const CumulativeReward<T>& obj, bool qualitative) const = 0;
};

/*!
 * @brief
 * Class for an abstract (path) formula tree with a Cumulative Reward node as root.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractPathFormula
 * @see AbstractPrctlFormula
 */
template <class T>
class CumulativeReward : public storm::property::abstract::CumulativeReward<T>,
								 public AbstractPathFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	CumulativeReward() {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param bound The time bound of the reward formula
	 */
	CumulativeReward(T bound) :
		storm::property::abstract::CumulativeReward<T>(bound) {
		// Intentionally left empty
	}

	/*!
	 * Empty destructor.
	 */
	virtual ~CumulativeReward() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new CumulativeReward-object that is identical the called object.
	 */
	virtual AbstractPathFormula<T>* clone() const override {
		return new CumulativeReward(this->getBound());
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
	virtual std::vector<T>* check(const storm::modelchecker::prctl::AbstractModelChecker<T>& modelChecker, bool qualitative) const override {
		return modelChecker.template as<ICumulativeRewardModelChecker>()->checkCumulativeReward(*this, qualitative);
	}
};

} //namespace prctl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_INSTANTANEOUSREWARD_H_ */
