/*
 * Not.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_LTL_NOT_H_
#define STORM_FORMULA_LTL_NOT_H_

#include "AbstractLtlFormula.h"

namespace storm {
namespace property {
namespace ltl {

template <class T> class Not;

/*!
 *  @brief Interface class for model checkers that support Not.
 *   
 *  All model checkers that support the formula class Not must inherit
 *  this pure virtual class.
 */
template <class T>
class INotModelChecker {
    public:
		/*!
         *  @brief Evaluates Not formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T> checkNot(const Not<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for an abstract formula tree with NOT node as root.
 *
 * Has one Abstract LTL formula as sub formula/tree.
 *
 * The subtree is seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractLtlFormula
 */
template <class T>
class Not : public AbstractLtlFormula<T> {

public:

	/*!
	 * Empty constructor
	 */
	Not() : child(nullptr) {
		// Intentionally left empty.
	}

	/*!
	 * Constructor
	 * @param child The child node
	 */
	Not(std::shared_ptr<AbstractLtlFormula<T>> const & child) : child(child) {
		// Intentionally left empty.
	}

	/*!
	 * Destructor
	 *
	 * Also deletes the subtree
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~Not() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual std::shared_ptr<AbstractLtlFormula<T>> clone() const override {
		std::shared_ptr<Not<T>> result(new Not<T>());
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
	 * @returns A bit vector indicating all states that satisfy the formula represented by the called object.
	 */
	virtual std::vector<T> check(storm::modelchecker::ltl::AbstractModelChecker<T> const & modelChecker) const override {
		return modelChecker.template as<INotModelChecker>()->checkNot(*this);
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
		std::string result = "!";
		result += child->toString();
		return result;
	}

	/*!
	 * Returns whether the formula is a propositional logic formula.
	 * That is, this formula and all its subformulas consist only of And, Or, Not and AP.
	 *
	 * @return True iff this is a propositional logic formula.
	 */
	virtual bool isPropositional() const override {
		return child->isPropositional();
	}

	/*!
	 * @returns The child node
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

#endif /* STORM_FORMULA_LTL_NOT_H_ */
