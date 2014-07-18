/*
 * SteadyStateReward.h
 *
 *  Created on: 08.04.2013
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PRCTL_STEADYSTATEREWARD_H_
#define STORM_FORMULA_PRCTL_STEADYSTATEREWARD_H_

#include "AbstractRewardPathFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include <string>

namespace storm {
namespace property {
namespace prctl {

template <class T> class SteadyStateReward;

/*!
 *  @brief Interface class for model checkers that support SteadyStateReward.
 *
 *  All model checkers that support the formula class SteadyStateReward must inherit
 *  this pure virtual class.
 */
template <class T>
class ISteadyStateRewardModelChecker {
    public:
		/*!
         *  @brief Evaluates CumulativeReward formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T> checkSteadyStateReward(const SteadyStateReward<T>& obj, bool qualitative) const = 0;
};

/*!
 * @brief
 * Class for an abstract (path) formula tree with a Steady State Reward node as root.
 *
 * @see AbstractPathFormula
 * @see AbstractPrctlFormula
 */
template <class T>
class SteadyStateReward: public AbstractRewardPathFormula<T> {
public:
	/*!
	 * Empty constructor
	 */
	SteadyStateReward() {
		// Intentionally left empty.
	}

	virtual ~SteadyStateReward() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new SteadyState-object that is identical the called object.
	 */
	virtual std::shared_ptr<AbstractRewardPathFormula<T>> clone() const override {
		std::shared_ptr<SteadyStateReward<T>> result(new SteadyStateReward<T>());
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
	virtual std::vector<T> check(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelChecker, bool qualitative) const override {
		return modelChecker.template as<ISteadyStateRewardModelChecker>()->checkSteadyStateReward(*this, qualitative);
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
		return "S";
	}

	/*!
     *  @brief Checks if all subtrees conform to some logic.
     *
     *  As SteadyStateReward objects have no subformulas, we return true here.
     *
     *  @param checker Formula checker object.
     *  @return true
     */
	virtual bool validate(AbstractFormulaChecker<T> const & checker) const override {
		return true;
	}
};

} //namespace prctl
} //namespace property
} //namespace storm
#endif /* STORM_FORMULA_PRCTL_STEADYSTATEREWARD_H_ */
