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

template <class T> class ReachabilityReward;

/*!
 *  @brief Interface class for model checkers that support ReachabilityReward.
 *   
 *  All model checkers that support the formula class ReachabilityReward must inherit
 *  this pure virtual class.
 */
template <class T>
class IReachabilityRewardModelChecker {
    public:
		/*!
         *  @brief Evaluates ReachabilityReward formula within a model checker.  
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T> checkReachabilityReward(const ReachabilityReward<T>& obj, bool qualitative) const = 0;
};

/*!
 * @brief
 * Class for an abstract (path) formula tree with an Reachability Reward node as root.
 *
 * Has one Abstract state formula as sub formula/tree.
 *
 * The subtree is seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to nullptr before deletion)
 *
 * @see AbstractPathFormula
 * @see AbstractPrctlFormula
 */
template <class T>
class ReachabilityReward : public AbstractRewardPathFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	ReachabilityReward() : child(nullptr){
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param child The child node
	 */
	ReachabilityReward(std::shared_ptr<AbstractStateFormula<T>> child) : child(child){
		// Intentionally left empty
	}

	/*!
	 * Destructor.
	 *
	 * Deletes the subtree iff this object is the last remaining owner of the subtree.
	 */
	virtual ~ReachabilityReward() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new ReachabilityReward-object that is identical the called object.
	 */
	virtual std::shared_ptr<AbstractRewardPathFormula<T>> clone() const override {
		std::shared_ptr<ReachabilityReward<T>> result(new ReachabilityReward<T>());
		if (this->childIsSet()) {
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
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
		std::string result = "F ";
		result += child->toString();
		return result;
	}

	/*!
	 * @returns the child node
	 */
	std::shared_ptr<AbstractStateFormula<T>> const & getChild() const {
		return child;
	}

	/*!
	 * Sets the subtree
	 * @param child the new child node
	 */
	void setChild(std::shared_ptr<AbstractStateFormula<T>> const & child) {
		this->child = child;
	}

	/*!
	 *
	 * @return True if the child node is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool childIsSet() const {
		return child.get() != nullptr;
	}

private:
	std::shared_ptr<AbstractStateFormula<T>> child;
};

} //namespace prctl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_REACHABILITYREWARD_H_ */
