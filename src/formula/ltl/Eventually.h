/*
 * Next.h
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

template <class T> class Eventually;

/*!
 *  @brief Interface class for model checkers that support Eventually.
 *
 *  All model checkers that support the formula class Eventually must inherit
 *  this pure virtual class.
 */
template <class T>
class IEventuallyModelChecker {
    public:
		/*!
         *  @brief Evaluates Eventually formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T> checkEventually(const Eventually<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for an abstract (path) formula tree with an Eventually node as root.
 *
 * Has one Abstract LTL formula as sub formula/tree.
 *
 * @par Semantics
 * The formula holds iff eventually \e child holds.
 *
 * The subtree is seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to nullptr before deletion)
 *
 * @see AbstractLtlFormula
 */
template <class T>
class Eventually : public AbstractLtlFormula<T> {

public:

	/*!
	 * Empty constructor
	 */
	Eventually() : child(nullptr) {
		//  Intentionally left empty.
	}

	/*!
	 * Constructor
	 *
	 * @param child The child node
	 */
	Eventually(std::shared_ptr<AbstractLtlFormula<T>> const & child) : child(child) {
		// Intentionally left empty.
	}

	/*!
	 * Constructor.
	 *
	 * Also deletes the subtree.
	 * (this behaviour can be prevented by setting the subtrees to nullptr before deletion)
	 */
	virtual ~Eventually() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new Eventually-object that is identical the called object.
	 */
	virtual std::shared_ptr<AbstractLtlFormula<T>> clone() const override {
		std::shared_ptr<Eventually<T>> result(new Eventually<T>());
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
	virtual std::vector<T> check(const storm::modelchecker::ltl::AbstractModelChecker<T>& modelChecker) const {
		return modelChecker.template as<IEventuallyModelChecker>()->checkEventually(*this);
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
	std::shared_ptr<AbstractLtlFormula<T>> const & getChild() const {
		return child;
	}

	/*!
	 * Sets the subtree
	 * @param child the new child node
	 */
	void setChild(std::shared_ptr<AbstractLtlFormula<T>> const & child) {
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
	std::shared_ptr<AbstractLtlFormula<T>> child;
};

} //namespace ltl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_LTL_EVENTUALLY_H_ */
