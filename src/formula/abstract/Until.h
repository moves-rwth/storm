/*
 * Until.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACT_UNTIL_H_
#define STORM_FORMULA_ABSTRACT_UNTIL_H_

#include "src/formula/abstract/AbstractFormula.h"
#include "src/formula/abstract/AbstractFormula.h"
#include "src/formula/AbstractFormulaChecker.h"

namespace storm {
namespace formula {
namespace abstract {

/*!
 * @brief
 * Class for a Abstract (path) formula tree with an Until node as root.
 *
 * Has two Abstract state formulas as sub formulas/trees.
 *
 * @par Semantics
 * The formula holds iff eventually, formula \e right (the right subtree) holds, and before,
 * \e left holds always.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractFormula
 * @see AbstractFormula
 */
template <class T>
class Until : public AbstractFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	Until() {
		this->left = NULL;
		this->right = NULL;
	}

	/*!
	 * Constructor
	 *
	 * @param left The left formula subtree
	 * @param right The left formula subtree
	 */
	Until(AbstractFormula<T>* left, AbstractFormula<T>* right) {
		this->left = left;
		this->right = right;
	}

	/*!
	 * Destructor.
	 *
	 * Also deletes the subtrees.
	 * (this behaviour can be prevented by setting the subtrees to NULL before deletion)
	 */
	virtual ~Until() {
	  if (left != NULL) {
		  delete left;
	  }
	  if (right != NULL) {
		  delete right;
	  }
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = left->toString();
		result += " U ";
		result += right->toString();
		return result;
	}

	/*!
     *  @brief Checks if all subtrees conform to some logic.
     *
     *  @param checker Formula checker object.
     *  @return true iff all subtrees conform to some logic.
     */
	virtual bool conforms(const AbstractFormulaChecker<T>& checker) const {
        return checker.conforms(this->left) && checker.conforms(this->right);
    }

protected:
	/*!
	 * Sets the left child node.
	 *
	 * @param newLeft the new left child.
	 */
	void setLeft(AbstractFormula<T>* newLeft) {
		left = newLeft;
	}

	/*!
	 * Sets the right child node.
	 *
	 * @param newRight the new right child.
	 */
	void setRight(AbstractFormula<T>* newRight) {
		right = newRight;
	}

	/*!
	 * @returns a pointer to the left child node
	 */
	const AbstractFormula<T>& getLeft() const {
		return *left;
	}

	/*!
	 * @returns a pointer to the right child node
	 */
	const AbstractFormula<T>& getRight() const {
		return *right;
	}

private:
	AbstractFormula<T>* left;
	AbstractFormula<T>* right;
};

} //namespace abstract

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_UNTIL_H_ */
