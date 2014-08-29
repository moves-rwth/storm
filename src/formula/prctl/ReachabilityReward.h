/*
 * Next.h
 *
 *  Created on: 26.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_PRCTL_REACHABILITYREWARD_H_
#define STORM_FORMULA_PRCTL_REACHABILITYREWARD_H_

#include "AbstractRewardPathFormula.h"
#include "AbstractStateFormula.h"

namespace storm {
namespace property {
namespace prctl {

// Forward declaration for the interface class.
template <class T> class ReachabilityReward;

/*!
 * Interface class for model checkers that support ReachabilityReward.
 *   
 * All model checkers that support the formula class ReachabilityReward must inherit
 * this pure virtual class.
 */
template <class T>
class IReachabilityRewardModelChecker {
    public:

		/*!
		 * Empty virtual destructor.
		 */
		virtual ~IReachabilityRewardModelChecker() {
			// Intentionally left empty
		}

		/*!
		 * Evaluates a ReachabilityReward formula within a model checker.
		 *
		 * @param obj Formula object with subformulas.
		 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
         *                    results are only compared against the bounds 0 and 1.
		 * @return The modelchecking result of the formula for every state.
		 */
        virtual std::vector<T> checkReachabilityReward(const ReachabilityReward<T>& obj, bool qualitative) const = 0;
};

/*!
 * Class for an Prctl (reward path) formula tree with an Reachability Reward node as root.
 *
 * Has one state formula as sub formula/tree.
 *
 * This formula expresses the rewards received or costs needed to reach a state satisfying the formula \e child.
 * In case the state under consiteration itself satisfies the formula \e child the rewards are zero.
 * In case that there is a non zero probability of not reaching any of the target states the rewards are infinite.
 * Also note that for this formula both state and transition rewards are considered and use if available in the model.
 *
 * The object has shared ownership of its subtree. If this object is deleted and no other object has a shared
 * ownership of the subtree it will be deleted as well.
 *
 * @see AbstractRewardPathFormula
 * @see AbstractPrctlFormula
 */
template <class T>
class ReachabilityReward : public AbstractRewardPathFormula<T> {

public:

	/*!
	 * Creates a ReachabilityReward node without a subnode.
	 * The resulting object will not represent a complete formula!
	 */
	ReachabilityReward() : child(nullptr){
		// Intentionally left empty
	}

	/*!
	 * Creates an Eventually node using the given parameter.
	 *
	 * @param child The child formula subtree.
	 */
	ReachabilityReward(std::shared_ptr<AbstractStateFormula<T>> child) : child(child){
		// Intentionally left empty
	}

	/*!
	 * Empty virtual destructor.
	 */
	virtual ~ReachabilityReward() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subnodes of the new object are clones of the original ones.
	 *
	 * @returns A new ReachabilityReward object that is a deep copy of the called object.
	 */
	virtual std::shared_ptr<AbstractRewardPathFormula<T>> clone() const override {
		std::shared_ptr<ReachabilityReward<T>> result(new ReachabilityReward<T>());
		if (this->isChildSet()) {
			result->setChild(child->clone());
		}
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
		return modelChecker.template as<IReachabilityRewardModelChecker>()->checkReachabilityReward(*this, qualitative);
	}

	/*!
	 * Returns a textual representation of the formula tree with this node as root.
	 *
	 * @returns A string representing the formula tree.
	 */
	virtual std::string toString() const override {
		std::string result = "F ";
		result += child->toString();
		return result;
	}

	/*!
	 * Gets the child node.
	 *
	 * @returns The child node.
	 */
	std::shared_ptr<AbstractStateFormula<T>> const & getChild() const {
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

private:

	// The child node.
	std::shared_ptr<AbstractStateFormula<T>> child;
};

} //namespace prctl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_REACHABILITYREWARD_H_ */
