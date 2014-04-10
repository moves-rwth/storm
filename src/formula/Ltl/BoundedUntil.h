/*
 * BoundedUntil.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_LTL_BOUNDEDUNTIL_H_
#define STORM_FORMULA_LTL_BOUNDEDUNTIL_H_

#include "src/formula/Ltl/AbstractLtlFormula.h"
#include <cstdint>
#include <string>
#include "src/modelchecker/ltl/ForwardDeclarations.h"

namespace storm {
namespace property {
namespace ltl {

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
        virtual std::vector<T> checkBoundedUntil(const BoundedUntil<T>& obj) const = 0;
};

/*!
 *	@brief Interface class for visitors that support BoundedUntil.
 *
 *	All visitors that support the formula class BoundedUnitl must inherit
 *	this pure virtual class.
 */
template <class T>
class IBoundedUntilVisitor {
	public:
		/*!
		 *	@brief Visits BoundedUntil formula.
		 *
		 *	@param obj Formula object with subformulas.
		 *	@return Result of the formula for every node.
		 */
		virtual void visitBoundedUntil(const BoundedUntil<T>& obj) = 0;
};

/*!
 * @brief
 * Class for an abstract (path) formula tree with a BoundedUntil node as root.
 *
 * Has two Abstract LTL formulas as sub formulas/trees.
 *
 * @par Semantics
 * The formula holds iff in at most \e bound steps, formula \e right (the right subtree) holds, and before,
 * \e left holds.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractLtlFormula
 */
template <class T>
class BoundedUntil : public AbstractLtlFormula<T> {

public:

	/*!
	 * Empty constructor
	 */
	BoundedUntil() {
		this->left = NULL;
		this->right = NULL;
		bound = 0;
	}

	/*!
	 * Constructor
	 *
	 * @param left The left formula subtree
	 * @param right The left formula subtree
	 * @param bound The maximal number of steps
	 */
	BoundedUntil(AbstractLtlFormula<T>* left, AbstractLtlFormula<T>* right,
					 uint_fast64_t bound) {
		this->left = left;
		this->right = right;
		this->bound = bound;
	}

	/*!
	 * Destructor.
	 *
	 * Also deletes the subtrees.
	 * (this behaviour can be prevented by setting the subtrees to NULL before deletion)
	 */
	virtual ~BoundedUntil() {
	  if (left != NULL) {
		  delete left;
	  }
	  if (right != NULL) {
		  delete right;
	  }
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedUntil-object that is identical the called object.
	 */
	virtual AbstractLtlFormula<T>* clone() const override {
		BoundedUntil<T>* result = new BoundedUntil<T>();
		result->setBound(this->getBound());
		if (this->leftIsSet()) {
			result->setLeft(this->getLeft().clone());
		}
		if (this->rightIsSet()) {
			result->setRight(this->getRight().clone());
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
		return modelChecker.template as<IBoundedUntilModelChecker>()->checkBoundedUntil(*this);
	}

	virtual void visit(visitor::AbstractLtlFormulaVisitor<T>& visitor) const override {
		visitor.template as<IBoundedUntilVisitor>()->visitBoundedUntil(*this);
	}

	/*!
	 *	@brief Return string representation of this formula.
	 *
	 * In LTL, brackets are needed around the until, as Until may appear nested (in other logics, Until always is the
	 * root of a path formula); hence this function is overwritten in this class.
	 *
	 * @return A string representation of the formula.
	 */
	virtual std::string toString() const override {
		std::string result = "(" + left->toString();
		result += " U<=";
		result += std::to_string(bound);
		result += " ";
		result += right->toString() + ")";
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
	void setLeft(AbstractLtlFormula<T>* newLeft) {
		left = newLeft;
	}

	/*!
	 * Sets the right child node.
	 *
	 * @param newRight the new right child.
	 */
	void setRight(AbstractLtlFormula<T>* newRight) {
		right = newRight;
	}

	/*!
	 * @returns a pointer to the left child node
	 */
	const AbstractLtlFormula<T>& getLeft() const {
		return *left;
	}

	/*!
	 * @returns a pointer to the right child node
	 */
	const AbstractLtlFormula<T>& getRight() const {
		return *right;
	}

	/*!
	 *
	 * @return True if the left child is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool leftIsSet() const {
		return left != nullptr;
	}

	/*!
	 *
	 * @return True if the right child is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool rightIsSet() const {
		return right != nullptr;
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
	AbstractLtlFormula<T>* left;
	AbstractLtlFormula<T>* right;
	uint_fast64_t bound;
};

} //namespace ltl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_LTL_BOUNDEDUNTIL_H_ */
