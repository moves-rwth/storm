/*
 * SteadyStateReward.h
 *
 *  Created on: 08.04.2013
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PRCTL_STEADYSTATEREWARD_H_
#define STORM_FORMULA_PRCTL_STEADYSTATEREWARD_H_

#include "AbstractRewardPathFormula.h"
#include <string>

namespace storm {
namespace property {
namespace prctl {

// Forward declaration for the interface class.
template <class T> class SteadyStateReward;

/*!
 * Interface class for model checkers that support SteadyStateReward.
 *
 * All model checkers that support the formula class SteadyStateReward must inherit
 * this pure virtual class.
 */
template <class T>
class ISteadyStateRewardModelChecker {
    public:

		/*!
		 * Empty virtual destructor.
		 */
		virtual ~ISteadyStateRewardModelChecker() {
			// Intentionally left empty
		}

		/*!
		 * Evaluates a SteadyStateReward formula within a model checker.
		 *
		 * @param obj Formula object with subformulas.
		 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
         *                    results are only compared against the bounds 0 and 1.
		 * @return The modelchecking result of the formula for every state.
		 */
        virtual std::vector<T> checkSteadyStateReward(const SteadyStateReward<T>& obj, bool qualitative) const = 0;
};

/*!
 * Class for a Steady State Reward formula.
 * This class represents a possible leaf in a reward formula tree.
 *
 * This formula expresses the expected long-run rewards for each state in the model.
 *
 * @see AbstractRewardPathFormula
 * @see AbstractPrctlFormula
 */
template <class T>
class SteadyStateReward: public AbstractRewardPathFormula<T> {
public:

	/*!
	 * Creates a new SteadyStateReward node.
	 */
	SteadyStateReward() {
		// Intentionally left empty.
	}

	/*!
	 * Empty virtual destructor.
	 */
	virtual ~SteadyStateReward() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subnodes of the new object are clones of the original ones.
	 *
	 * @returns A new SteadyStateReward object that is a deep copy of the called object.
	 */
	virtual std::shared_ptr<AbstractRewardPathFormula<T>> clone() const override {
		auto result = std::make_shared<SteadyStateReward<T>>();
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
	 * Returns a textual representation of the formula tree with this node as root.
	 *
	 * @returns A string representing the formula tree.
	 */
	virtual std::string toString() const override {
		return "S";
	}
};

} //namespace prctl
} //namespace property
} //namespace storm
#endif /* STORM_FORMULA_PRCTL_STEADYSTATEREWARD_H_ */
