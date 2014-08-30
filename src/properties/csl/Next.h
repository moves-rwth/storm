/*
 * Next.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_CSL_NEXT_H_
#define STORM_FORMULA_CSL_NEXT_H_

#include "src/properties/csl/AbstractPathFormula.h"
#include "src/properties/csl/AbstractStateFormula.h"

namespace storm {
namespace properties {
namespace csl {

// Forward declaration for the interface class.
template <class T> class Next;

/*!
 * Interface class for model checkers that support Next.
 *   
 * All model checkers that support the formula class Next must inherit
 * this pure virtual class.
 */
template <class T>
class INextModelChecker {
    public:

		/*!
		 * Empty virtual destructor.
		 */
		virtual ~INextModelChecker() {
			// Intentionally left empty
		}

		/*!
		 * Evaluates Next formula within a model checker.
		 *
		 * @param obj Formula object with subformulas.
		 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
		 *                    results are only compared against the bounds 0 and 1.
		 * @return Result of the formula for every node.
		 */
        virtual std::vector<T> checkNext(const Next<T>& obj, bool qualitative) const = 0;
};

/*!
 * Class for a Csl (path) formula tree with a Next node as root.
 *
 * Has two Csl state formulas as sub formulas/trees.
 *
 * @par Semantics
 * The formula holds iff in the next step, \e child holds
 *
 * The object has shared ownership of its subtree. If this object is deleted and no other object has a shared
 * ownership of the subtree it will be deleted as well.
 *
 * @see AbstractPathFormula
 * @see AbstractCslFormula
 */
template <class T>
class Next : public AbstractPathFormula<T> {

public:

	/*!
	 * Creates a Next node without a subnode.
	 * The resulting object will not represent a complete formula!
	 */
	Next() : child(nullptr){
		// Intentionally left empty.
	}

	/*!
	 * Creates a Next node using the given parameter.
	 *
	 * @param child The child formula subtree.
	 */
	Next(std::shared_ptr<AbstractStateFormula<T>> const & child) : child(child) {
		// Intentionally left empty.
	}

	/*!
	 * Empty virtual destructor.
	 */
	virtual ~Next() {
		// Intetionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subnodes of the new object are clones of the original ones.
	 *
	 * @returns A new Next object that is a deep copy of the called object.
	 */
	virtual std::shared_ptr<AbstractPathFormula<T>> clone() const override {
		std::shared_ptr<Next<T>> result(new Next<T>());
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
	virtual std::vector<T> check(storm::modelchecker::csl::AbstractModelChecker<T> const & modelChecker, bool qualitative) const override {
		return modelChecker.template as<INextModelChecker>()->checkNext(*this, qualitative);
	}

	/*!
	 * Returns a textual representation of the formula tree with this node as root.
	 *
	 * @returns A string representing the formula tree.
	 */
	virtual std::string toString() const override {
		std::string result = "X ";
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

} //namespace csl
} //namespace properties
} //namespace storm

#endif /* STORM_FORMULA_CSL_NEXT_H_ */
