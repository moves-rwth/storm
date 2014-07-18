/*
 * BoundedUntil.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PRCTL_BOUNDEDUNTIL_H_
#define STORM_FORMULA_PRCTL_BOUNDEDUNTIL_H_

#include "src/formula/prctl/AbstractPathFormula.h"
#include "src/formula/prctl/AbstractStateFormula.h"
#include <cstdint>
#include <string>
#include "src/modelchecker/prctl/ForwardDeclarations.h"

namespace storm {
namespace property {
namespace prctl {

template <class T> class BoundedUntil;

/*!
 *  @brief Interface class for model checkers that support BoundedUntil.
 *   
 *  All model checkers that support the formula class BoundedUntil must inherit
 *  this pure virtual class.
 */
template <class T>
class IBoundedUntilModelChecker {
    public:
		/*!
         *  @brief Evaluates BoundedUntil formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T> checkBoundedUntil(const BoundedUntil<T>& obj, bool qualitative) const = 0;
};

/*!
 * @brief
 * Class for an abstract (path) formula tree with a BoundedUntil node as root.
 *
 * Has two Abstract state formulas as sub formulas/trees.
 *
 * @par Semantics
 * The formula holds iff in at most \e bound steps, formula \e right (the right subtree) holds, and before,
 * \e left holds.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractPathFormula
 * @see AbstractPrctlFormula
 */
template <class T>
class BoundedUntil : public AbstractPathFormula<T> {

public:

	/*!
	 * Empty constructor
	 */
	BoundedUntil() : left(nullptr), right(nullptr), bound(0){
		// Intentionally left empty.
	}

	/*!
	 * Constructor
	 *
	 * @param left The left formula subtree
	 * @param right The left formula subtree
	 * @param bound The maximal number of steps
	 */
	BoundedUntil(std::shared_ptr<AbstractStateFormula<T>> const & left, std::shared_ptr<AbstractStateFormula<T>> const & right, uint_fast64_t bound) : left(left), right(right), bound(bound) {
		// Intentionally left empty.
	}
	/*!
	 * Destructor.
	 *
	 * Deletes the subtrees iff this object is the only owner of them.
	 */
	virtual ~BoundedUntil() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedUntil-object that is identical the called object.
	 */
	virtual std::shared_ptr<AbstractPathFormula<T>> clone() const override {
		std::shared_ptr<BoundedUntil<T>> result(new BoundedUntil<T>());
		result->setBound(bound);
		if (this->leftIsSet()) {
			result->setLeft(left->clone());
		}
		if (this->rightIsSet()) {
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
		return modelChecker.template as<IBoundedUntilModelChecker>()->checkBoundedUntil(*this, qualitative);
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
		std::string result = left->toString();
		result += " U<=";
		result += std::to_string(bound);
		result += " ";
		result += right->toString();
		return result;
	}

	/*!
	 *  @brief Checks if all subtrees conform to some logic.
	 *
	 *  @param checker Formula checker object.
	 *  @return true iff all subtrees conform to some logic.
	 */
	virtual bool validate(const AbstractFormulaChecker<T>& checker) const override {
		return checker.validate(this->left) && checker.validate(this->right);
	}

	/*!
	 * Sets the left child node.
	 *
	 * @param newLeft the new left child.
	 */
	void setLeft(std::shared_ptr<AbstractStateFormula<T>> const & newLeft) {
		left = newLeft;
	}

	/*!
	 * Sets the right child node.
	 *
	 * @param newRight the new right child.
	 */
	void setRight(std::shared_ptr<AbstractStateFormula<T>> const & newRight) {
		right = newRight;
	}

	/*!
	 * @returns a pointer to the left child node
	 */
	std::shared_ptr<AbstractStateFormula<T>> const & getLeft() const {
		return left;
	}

	/*!
	 * @returns a pointer to the right child node
	 */
	std::shared_ptr<AbstractStateFormula<T>> const & getRight() const {
		return right;
	}

	/*!
	 *
	 * @return True if the left child is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool leftIsSet() const {
		return left.get() != nullptr;
	}

	/*!
	 *
	 * @return True if the right child is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool rightIsSet() const {
		return right.get() != nullptr;
	}

	/*!
	 * @returns the maximally allowed number of steps for the bounded until operator
	 */
	uint_fast64_t getBound() const {
		return bound;
	}

	/*!
	 * Sets the maximally allowed number of steps for the bounded until operator
	 *
	 * @param bound the new bound.
	 */
	void setBound(uint_fast64_t bound) {
		this->bound = bound;
	}

private:
	std::shared_ptr<AbstractStateFormula<T>> left;
	std::shared_ptr<AbstractStateFormula<T>> right;
	uint_fast64_t bound;
};

} //namespace prctl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_BOUNDEDUNTIL_H_ */
