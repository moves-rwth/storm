/*
 * InstantaneousReward.h
 *
 *  Created on: 26.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_PRCTL_CUMULATIVEREWARD_H_
#define STORM_FORMULA_PRCTL_CUMULATIVEREWARD_H_

#include "AbstractRewardPathFormula.h"
#include <string>

namespace storm {
namespace properties {
namespace prctl {

// Forward declaration for the interface class.
template <class T> class CumulativeReward;

/*!
 * Interface class for model checkers that support CumulativeReward.
 *
 * All model checkers that support the formula class CumulativeReward must inherit
 * this pure virtual class.
 */
template <class T>
class ICumulativeRewardModelChecker {
    public:

		/*!
		 * Empty virtual destructor.
		 */
		virtual ~ICumulativeRewardModelChecker() {
			// Intentionally left empty
		}

		/*!
         * Evaluates CumulativeReward formula within a model checker.
         *
         * @param obj Formula object with subformulas.
         * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
         *                    results are only compared against the bounds 0 and 1.
         * @return Result of the formula for every node.
         */
        virtual std::vector<T> checkCumulativeReward(const CumulativeReward<T>& obj, bool qualitative) const = 0;
};

/*!
 * Class for a Prctl (reward path) formula tree with a Cumulative Reward node as root.
 *
 * Given a path of finite length.
 * The sum of all rewards received upon entering each state of the path is the cumulative reward of the path.
 * The cumulative reward for a state s at time \e bound is the expected cumulative reward of a path of length \e bound starting in s.
 * In the continuous case all paths that need at most time \e bound are considered.
 *
 * @see AbstractPathFormula
 * @see AbstractPrctlFormula
 */
template <class T>
class CumulativeReward : public AbstractRewardPathFormula<T> {

public:

	/*!
	 * Creates a CumulativeReward node with the given bound.
	 *
	 * If no bound is given it defaults to 0, referencing the state reward received upon entering the state s itself.
	 *
	 * @param bound The time instance of the reward formula.
	 */
	CumulativeReward(T bound = 0) : bound(bound){
		// Intentionally left empty.
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
	 * Performs a "deep copy", i.e. the subnodes of the new object are clones of the original ones.
	 *
	 * @returns A new CumulativeReward object that is a deep copy of the called object.
	 */
	virtual std::shared_ptr<AbstractRewardPathFormula<T>> clone() const override {
		std::shared_ptr<CumulativeReward<T>> result(new CumulativeReward(this->getBound()));
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
		return modelChecker.template as<ICumulativeRewardModelChecker>()->checkCumulativeReward(*this, qualitative);
	}

	/*!
	 * Returns a textual representation of the formula tree with this node as root.
	 *
	 * @returns A string representing the formula tree.
	 */
	virtual std::string toString() const override {
		std::string result = "C <= ";
		result += std::to_string(bound);
		return result;
	}

	/*!
	 * Gets the time bound for the paths considered.
	 *
	 * @returns The time bound for the paths considered.
	 */
	T getBound() const {
		return bound;
	}

	/*!
	 * Sets the time bound for the paths considered.
	 *
	 * @param bound The new bound.
	 */
	void setBound(T bound) {
		this->bound = bound;
	}

private:

	// The time bound for the paths considered.
	T bound;
};

} //namespace prctl
} //namespace properties
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_INSTANTANEOUSREWARD_H_ */
