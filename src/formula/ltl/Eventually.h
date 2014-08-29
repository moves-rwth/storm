/*
 * Eventually.h
 *
 *  Created on: 26.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_LTL_EVENTUALLY_H_
#define STORM_FORMULA_LTL_EVENTUALLY_H_

#include "src/formula/ltl/AbstractLtlFormula.h"
#include "src/modelchecker/ltl/ForwardDeclarations.h"

namespace storm {
namespace property {
namespace ltl {

// Forward declaration for the interface class.
template <class T> class Eventually;

/*!
 * Interface class for model checkers that support Eventually.
 *
 * All model checkers that support the formula class Eventually must inherit
 * this pure virtual class.
 */
template <class T>
class IEventuallyModelChecker {
    public:

		/*!
		 * Empty virtual destructor.
		 */
		virtual ~IEventuallyModelChecker() {
			// Intentionally left empty.
		}

		/*!
		 * Evaluates an Eventually formula within a model checker.
		 *
		 * @param obj Formula object with subformulas.
		 * @return The modelchecking result of the formula for every state.
		 */
        virtual std::vector<T> checkEventually(const Eventually<T>& obj) const = 0;
};

/*!
 * Class for an Ltl formula tree with an Eventually node as root.
 *
 * Has one Abstract Ltl formula as sub formula/tree.
 *
 * @par Semantics
 * The formula holds iff eventually \e child holds.
 *
 * The object has shared ownership of its subtree. If this object is deleted and no other object has a shared
 * ownership of the subtree it will be deleted as well.
 *
 * @see AbstractLtlFormula
 */
template <class T>
class Eventually : public AbstractLtlFormula<T> {

public:

	/*!
	 * Creates an Eventually node without a subnode.
	 * The resulting object will not represent a complete formula!
	 */
	Eventually() : child(nullptr) {
		//  Intentionally left empty.
	}

	/*!
	 * Creates an Eventually node using the given parameter.
	 *
	 * @param child The child formula subtree.
	 */
	Eventually(std::shared_ptr<AbstractLtlFormula<T>> const & child) : child(child) {
		// Intentionally left empty.
	}

	/*!
	 * Empty virtual destructor.
	 */
	virtual ~Eventually() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subnodes of the new object are clones of the original ones.
	 *
	 * @returns A new Eventually object that is a deep copy of the called object.
	 */
	virtual std::shared_ptr<AbstractLtlFormula<T>> clone() const override {
		std::shared_ptr<Eventually<T>> result(new Eventually<T>());
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
	virtual std::vector<T> check(const storm::modelchecker::ltl::AbstractModelChecker<T>& modelChecker) const {
		return modelChecker.template as<IEventuallyModelChecker>()->checkEventually(*this);
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
	std::shared_ptr<AbstractLtlFormula<T>> const & getChild() const {
		return child;
	}

	/*!
	 * Sets the subtree.
	 *
	 * @param child The new child.
	 */
	void setChild(std::shared_ptr<AbstractLtlFormula<T>> const & child) {
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
	std::shared_ptr<AbstractLtlFormula<T>> child;
};

} //namespace ltl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_LTL_EVENTUALLY_H_ */
