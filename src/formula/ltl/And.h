/*
 * And.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_LTL_AND_H_
#define STORM_FORMULA_LTL_AND_H_

#include "AbstractLtlFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include "src/modelchecker/ltl/ForwardDeclarations.h"
#include <string>

namespace storm {
namespace property {
namespace ltl {

template <class T> class And;

/*!
 *	@brief Interface class for model checkers that support And.
 *
 *	All model checkers that support the formula class And must inherit
 *	this pure virtual class.
 */
template <class T>
class IAndModelChecker {
	public:
		/*!
		 *	@brief Evaluates And formula within a model checker.
		 *
		 *	@param obj Formula object with subformulas.
		 *	@return Result of the formula for every node.
		 */
		virtual std::vector<T> checkAnd(const And<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for an abstract formula tree with AND node as root.
 *
 * Has two Abstract LTL formulas as sub formulas/trees.
 *
 * As AND is commutative, the order is \e theoretically not important, but will influence the order in which
 * the model checker works.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractLtlFormula
 */
template <class T>
class And : public AbstractLtlFormula<T> {

public:

	/*!
	 * Empty constructor.
	 * Will create an AND-node without subnotes. Will not represent a complete formula!
	 */
	And() : left(nullptr), right(nullptr){
		// Intentionally left empty.
	}

	/*!
	 * Constructor.
	 * Creates an AND note with the parameters as subtrees.
	 *
	 * @param left The left sub formula
	 * @param right The right sub formula
	 */
	And(std::shared_ptr<AbstractLtlFormula<T>> left, std::shared_ptr<AbstractLtlFormula<T>> right) : left(left), right(right) {
		// Intentionally left empty.
	}

	/*!
	 * Destructor.
	 *
	 * The subtrees are deleted with the object
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~And() {
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
		std::shared_ptr<And<T>> result(new And());
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
	 * @returns A bit vector indicating all states that satisfy the formula represented by the called object.
	 */
	virtual std::vector<T> check(const storm::modelchecker::ltl::AbstractModelChecker<T>& modelChecker) const override {
		return modelChecker.template as<IAndModelChecker>()->checkAnd(*this);
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
		std::string result = "(";
		result += left->toString();
		result += " & ";
		result += right->toString();
		result += ")";
		return result;
	}

	/*!
	 *	@brief Checks if all subtrees conform to some logic.
	 *
	 *	@param checker Formula checker object.
	 *	@return true iff all subtrees conform to some logic.
	 */
	virtual bool validate(AbstractFormulaChecker<T> const & checker) const override {
        return checker.validate(this->left) && checker.validate(this->right);
    }


	/*!
	 * Sets the left child node.
	 *
	 * @param newLeft the new left child.
	 */
	void setLeft(std::shared_ptr<AbstractLtlFormula<T>> const & newLeft) {
		left = newLeft;
	}

	/*!
	 * Sets the right child node.
	 *
	 * @param newRight the new right child.
	 */
	void setRight(std::shared_ptr<AbstractLtlFormula<T>> const & newRight) {
		right = newRight;
	}

	/*!
	 * @returns a pointer to the left child node
	 */
	std::shared_ptr<AbstractLtlFormula<T>> const & getLeft() const {
		return left;
	}

	/*!
	 * @returns a pointer to the right child node
	 */
	std::shared_ptr<AbstractLtlFormula<T>> const & getRight() const {
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

private:
	std::shared_ptr<AbstractLtlFormula<T>> left;
	std::shared_ptr<AbstractLtlFormula<T>> right;

};

} //namespace ltl

} //namespace property

} //namespace storm

#endif /* STORM_FORMULA_LTL_AND_H_ */
