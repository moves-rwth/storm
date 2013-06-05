/*
 * BoundedUntil.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACT_BOUNDEDUNTIL_H_
#define STORM_FORMULA_ABSTRACT_BOUNDEDUNTIL_H_

#include "src/formula/abstract/AbstractFormula.h"
#include "boost/integer/integer_mask.hpp"
#include <string>

namespace storm {
namespace property {
namespace abstract {

/*!
 * @brief
 * Class for an abstract (path) formula tree with a BoundedUntil node as root.
 *
 * Has two formulas as sub formulas/trees.
 *
 * @par Semantics
 * The formula holds iff in at most \e bound steps, formula \e right (the right subtree) holds, and before,
 * \e left holds.
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
class BoundedUntil : public virtual AbstractFormula<T> {

	// Throw a compiler error when FormulaType is not a subclass of AbstractFormula.
	static_assert(std::is_base_of<AbstractFormula<T>, FormulaType>::value,
				  "Instantiaton of FormulaType for storm::property::abstract::BoundedUntil<T,FormulaType> has to be a subtype of storm::property::abstract::AbstractFormula<T>");

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
	BoundedUntil(FormulaType* left, FormulaType* right,
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

private:
	FormulaType* left;
	FormulaType* right;
	uint_fast64_t bound;
};

} //namespace abstract

} //namespace property

} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_BOUNDEDUNTIL_H_ */
