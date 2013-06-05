/*
 * Or.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACT_OR_H_
#define STORM_FORMULA_ABSTRACT_OR_H_

#include "src/formula/abstract/AbstractFormula.h"
#include "src/formula/AbstractFormulaChecker.h"

namespace storm {
namespace property {
namespace abstract {

/*!
 * @brief
 * Class for an abstract formula tree with OR node as root.
 *
 * Has two formulas as sub formulas/trees.
 *
 * As OR is commutative, the order is \e theoretically not important, but will influence the order in which
 * the model checker works.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @tparam FormulaType The type of the subformula.
 * 		  The instantiation of FormulaType should be a subclass of AbstractFormula, as the functions
 * 		  "toString" and "conforms" of the subformulas are needed.
 *
 * @see AbstractFormula
 */
template <class T, class FormulaType>
class Or : public virtual AbstractFormula<T> {

	// Throw a compiler error if FormulaType is not a subclass of AbstractFormula.
	static_assert(std::is_base_of<AbstractFormula<T>, FormulaType>::value,
				  "Instantiaton of FormulaType for storm::property::abstract::Or<T,FormulaType> has to be a subtype of storm::property::abstract::AbstractFormula<T>");

public:
	/*!
	 * Empty constructor.
	 * Will create an AND-node without subnotes. Will not represent a complete formula!
	 */
	Or() {
		left = NULL;
		right = NULL;
	}

	/*!
	 * Constructor.
	 * Creates an AND note with the parameters as subtrees.
	 *
	 * @param left The left sub formula
	 * @param right The right sub formula
	 */
	Or(FormulaType* left, FormulaType* right) {
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
	 * Sets the left child node.
	 *
	 * @param newLeft the new left child.
	 */
	void setLeft(FormulaType* newLeft) {
		left = newLeft;
	}

	/*!
	 * Sets the right child node.
	 *
	 * @param newRight the new right child.
	 */
	void setRight(FormulaType* newRight) {
		right = newRight;
	}

	/*!
	 * @returns a pointer to the left child node
	 */
	const FormulaType& getLeft() const {
		return *left;
	}

	/*!
	 * @returns a pointer to the right child node
	 */
	const FormulaType& getRight() const {
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

private:
	FormulaType* left;
	FormulaType* right;
};

} //namespace abstract

} //namespace property

} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_OR_H_ */
