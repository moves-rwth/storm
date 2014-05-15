/*
 * InstantaneousReward.h
 *
 *  Created on: 26.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_PRCTL_INSTANTANEOUSREWARD_H_
#define STORM_FORMULA_PRCTL_INSTANTANEOUSREWARD_H_

#include "AbstractRewardPathFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include <cstdint>
#include <string>

namespace storm {
namespace property {
namespace prctl {

template <class T> class InstantaneousReward;

/*!
 *  @brief Interface class for model checkers that support InstantaneousReward.
 *
 *  All model checkers that support the formula class InstantaneousReward must inherit
 *  this pure virtual class.
 */
template <class T>
class IInstantaneousRewardModelChecker {
    public:
		/*!
         *  @brief Evaluates InstantaneousReward formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T> checkInstantaneousReward(const InstantaneousReward<T>& obj, bool qualitative) const = 0;
};

/*!
 * @brief
 * Class for an abstract (path) formula tree with a Instantaneous Reward node as root.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractPathFormula
 * @see AbstractPrctlFormula
 */
template <class T>
class InstantaneousReward : public AbstractRewardPathFormula<T> {

public:

	/*!
	 * Empty constructor
	 */
	InstantaneousReward() : bound(0) {
		// Intentionally left empty.
	}

	/*!
	 * Constructor
	 *
	 * @param bound The time instance of the reward formula
	 */
	InstantaneousReward(uint_fast64_t bound) : bound(bound) {
		// Intentionally left empty.
	}

	/*!
	 * Empty destructor.
	 */
	virtual ~InstantaneousReward() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new InstantaneousReward-object that is identical the called object.
	 */
	virtual AbstractRewardPathFormula<T>* clone() const override {
		return new InstantaneousReward(this->getBound());
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
	virtual std::vector<T> check(const storm::modelchecker::prctl::AbstractModelChecker<T>& modelChecker, bool qualitative) const override {
		return modelChecker.template as<IInstantaneousRewardModelChecker>()->checkInstantaneousReward(*this, qualitative);
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
		std::string result = "I=";
		result += std::to_string(bound);
		return result;
	}

	/*!
	 *  @brief Checks if all subtrees conform to some logic.
	 *
	 *  As InstantaneousReward formulas have no subformulas, we return true here.
	 *
	 *  @param checker Formula checker object.
	 *  @return true
	 */
	virtual bool validate(const AbstractFormulaChecker<T>& checker) const override {
		return true;
	}

	/*!
	 * @returns the time instance for the instantaneous reward operator
	 */
	uint_fast64_t getBound() const {
		return bound;
	}

	/*!
	 * Sets the the time instance for the instantaneous reward operator
	 *
	 * @param bound the new bound.
	 */
	void setBound(uint_fast64_t bound) {
		this->bound = bound;
	}

private:
	uint_fast64_t bound;
};

} //namespace prctl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_INSTANTANEOUSREWARD_H_ */
