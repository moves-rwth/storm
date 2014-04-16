/*
 * Or.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_CSL_OR_H_
#define STORM_FORMULA_CSL_OR_H_

#include "src/formula/Csl/AbstractStateFormula.h"
#include "src/formula/AbstractFormulaChecker.h"

namespace storm {
namespace property {
namespace csl {

template <class T> class Or;

/*!
 *  @brief Interface class for model checkers that support Or.
 *   
 *  All model checkers that support the formula class Or must inherit
 *  this pure virtual class.
 */
template <class T>
class IOrModelChecker {
	public:
		/*!
         *  @brief Evaluates Or formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
		virtual storm::storage::BitVector checkOr(const Or<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for an abstract formula tree with OR node as root.
 *
 * Has two Abstract state formulas as sub formulas/trees.
 *
 * As OR is commutative, the order is \e theoretically not important, but will influence the order in which
 * the model checker works.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractStateFormula
 * @see AbstractCslFormula
 */
template <class T>
class Or : public AbstractStateFormula<T> {

public:

	/*!
		 * Empty constructor.
		 * Will create an OR-node without subnotes. Will not represent a complete formula!
		 */
		Or() {
			left = NULL;
			right = NULL;
		}

		/*!
		 * Constructor.
		 * Creates an OR note with the parameters as subtrees.
		 *
		 * @param left The left sub formula
		 * @param right The right sub formula
		 */
		Or(AbstractStateFormula<T>* left, AbstractStateFormula<T>* right) {
			this->left = left;
			this->right = right;
		}

		/*!
		 * Destructor.
		 *
		 * The subtrees are deleted with the object
		 * (this behavior can be prevented by setting them to NULL before deletion)
		 */
		virtual ~Or() {
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
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual AbstractStateFormula<T>* clone() const override {
		Or<T>* result = new Or();
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
	 * @returns A bit vector indicating all states that satisfy the formula represented by the called object.
	 */
	virtual storm::storage::BitVector check(const storm::modelchecker::csl::AbstractModelChecker<T>& modelChecker) const override {
		return modelChecker.template as<IOrModelChecker>()->checkOr(*this);
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
		std::string result = "(";
		result += left->toString();
		result += " | ";
		result += right->toString();
		result += ")";
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
	void setLeft(AbstractStateFormula<T>* newLeft) {
		left = newLeft;
	}

	/*!
	 * Sets the right child node.
	 *
	 * @param newRight the new right child.
	 */
	void setRight(AbstractStateFormula<T>* newRight) {
		right = newRight;
	}

	/*!
	 * @returns a pointer to the left child node
	 */
	const AbstractStateFormula<T>& getLeft() const {
		return *left;
	}

	/*!
	 * @returns a pointer to the right child node
	 */
	const AbstractStateFormula<T>& getRight() const {
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

private:
	AbstractStateFormula<T>* left;
	AbstractStateFormula<T>* right;

};

} //namespace csl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_CSL_OR_H_ */
