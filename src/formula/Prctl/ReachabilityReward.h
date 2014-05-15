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
#include "src/formula/AbstractFormulaChecker.h"

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
	ReachabilityReward(AbstractStateFormula<T>* child) : child(child){
		// Intentionally left empty
	}

	/*!
	 * Constructor.
	 *
	 * Also deletes the subtree.
	 * (this behaviour can be prevented by setting the subtrees to nullptr before deletion)
	 */
	virtual ~ReachabilityReward() {
		if (child != nullptr) {
			delete child;
		}
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new ReachabilityReward-object that is identical the called object.
	 */
	virtual AbstractRewardPathFormula<T>* clone() const override {
		ReachabilityReward<T>* result = new ReachabilityReward<T>();
		if (this->childIsSet()) {
			result->setChild(this->getChild().clone());
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
	virtual std::vector<T> check(const storm::modelchecker::prctl::AbstractModelChecker<T>& modelChecker, bool qualitative) const override {
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
	 *  @brief Checks if the subtree conforms to some logic.
	 *
	 *  @param checker Formula checker object.
	 *  @return true iff the subtree conforms to some logic.
	 */
	virtual bool validate(const AbstractFormulaChecker<T>& checker) const override {
		return checker.validate(this->child);
	}

	/*!
	 * @returns the child node
	 */
	const AbstractStateFormula<T>& getChild() const {
		return *child;
	}

	/*!
	 * Sets the subtree
	 * @param child the new child node
	 */
	void setChild(AbstractStateFormula<T>* child) {
		this->child = child;
	}

	/*!
	 *
	 * @return True if the child node is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool childIsSet() const {
		return child != nullptr;
	}

private:
	AbstractStateFormula<T>* child;
};

} //namespace prctl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_REACHABILITYREWARD_H_ */
