/*
 * Until.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PRCTL_UNTIL_H_
#define STORM_FORMULA_PRCTL_UNTIL_H_

#include "AbstractPathFormula.h"
#include "AbstractStateFormula.h"

namespace storm {
namespace properties {
namespace prctl {

// Forward declaration for the interface class.
template <class T> class Until;

/*!
 * Interface class for model checkers that support Until.
 *
 * All model checkers that support the formula class Until must inherit
 * this pure virtual class.
 */
template <class T>
class IUntilModelChecker {
    public:

		/*!
		 * Empty virtual destructor.
		 */
		virtual ~IUntilModelChecker() {
			// Intentionally left empty
		}

		/*!
		 * Evaluates an Until formula within a model checker.
		 *
		 * @param obj Formula object with subformulas.
		 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
         *                    results are only compared against the bounds 0 and 1.
		 * @return The modelchecking result of the formula for every state.
		 */
        virtual std::vector<T> checkUntil(const Until<T>& obj, bool qualitative) const = 0;
};

/*!
 * Class for a Prctl (path) formula tree with an Until node as root.
 *
 * Has two state formulas as sub formulas/trees.
 *
 * @par Semantics
 * The formula holds iff eventually, formula \e right (the right subtree) holds, and before,
 * \e left holds always.
 *
 * The object has shared ownership of its subtrees. If this object is deleted and no other object has a shared
 * ownership of the subtrees they will be deleted as well.
 *
 * @see AbstractPathFormula
 * @see AbstractPrctlFormula
 * @see BoundedUntil
 */
template <class T>
class Until : public AbstractPathFormula<T> {

public:

	/*!
	 * Creates an Until node without subnodes.
	 * The resulting object will not represent a complete formula!
	 */
	Until() : left(nullptr), right(nullptr) {
		// Intentionally left empty.
	}

	/*!
	 * Creates an Until node using the given parameters.
	 *
	 * @param left The left formula subtree.
	 * @param right The right formula subtree.
	 */
	Until(std::shared_ptr<AbstractStateFormula<T>> const & left, std::shared_ptr<AbstractStateFormula<T>> const & right) : left(left), right(right) {
		// Intentionally left empty.
	}

	/*!
	 * Empty virtual destructor.
	 */
	virtual ~Until() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subnodes of the new object are clones of the original ones.
	 *
	 * @returns A new Until object that is a deep copy of the called object.
	 */
	virtual std::shared_ptr<AbstractPathFormula<T>> clone() const override {
		std::shared_ptr<Until<T>> result(new Until());
		if (this->isLeftSet()) {
		  result->setLeft(left->clone());
		}
		if (this->isRightSet()) {
		  result->setRight(right->clone());
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
		return modelChecker.template as<IUntilModelChecker>()->checkUntil(*this, qualitative);
	}

	/*!
	 * Returns a textual representation of the formula tree with this node as root.
	 *
	 * @returns A string representing the formula tree.
	 */
	virtual std::string toString() const override {
		std::string result = left->toString();
		result += " U ";
		result += right->toString();
		return result;
	}

	/*!
	 * Gets the left child node.
	 *
	 * @returns The left child node.
	 */
	std::shared_ptr<AbstractStateFormula<T>> const & getLeft() const {
		return left;
	}

	/*!
	 * Gets the right child node.
	 *
	 * @returns The right child node.
	 */
	std::shared_ptr<AbstractStateFormula<T>> const & getRight() const {
		return right;
	}

	/*!
	 * Sets the left child node.
	 *
	 * @param newLeft The new left child.
	 */
	void setLeft(std::shared_ptr<AbstractStateFormula<T>> const & newLeft) {
		left = newLeft;
	}

	/*!
	 * Sets the right child node.
	 *
	 * @param newRight The new right child.
	 */
	void setRight(std::shared_ptr<AbstractStateFormula<T>> const & newRight) {
		right = newRight;
	}

	/*!
	 * Checks if the left child is set, i.e. it does not point to null.
	 *
	 * @return True iff the left child is set.
	 */
	bool isLeftSet() const {
		return left.get() != nullptr;
	}

	/*!
	 * Checks if the right child is set, i.e. it does not point to null.
	 *
	 * @return True iff the right child is set.
	 */
	bool isRightSet() const {
		return right.get() != nullptr;
	}

private:

	// The left child node.
	std::shared_ptr<AbstractStateFormula<T>> left;

	// The right child node.
	std::shared_ptr<AbstractStateFormula<T>> right;
};

} //namespace prctl
} //namespace properties
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_UNTIL_H_ */
