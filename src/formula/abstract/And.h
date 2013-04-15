/*
 * And.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACT_AND_H_
#define STORM_FORMULA_ABSTRACT_AND_H_

#include "src/formula/abstract/AbstractFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include "src/modelchecker/ForwardDeclarations.h"
#include <string>

namespace storm {
namespace formula {
namespace abstract {

/*!
 * @brief
 * Class for a Abstract formula tree with AND node as root.
 *
 * Has two Abstract state formulas as sub formulas/trees.
 *
 * As AND is commutative, the order is \e theoretically not important, but will influence the order in which
 * the model checker works.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractFormula
 * @see AbstractFormula
 */
template <class T>
class And : public AbstractFormula<T> {

public:
	/*!
	 * Empty constructor.
	 * Will create an AND-node without subnotes. Will not represent a complete formula!
	 */
	And() {
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
	And(AbstractFormula<T>* left, AbstractFormula<T>* right) {
		this->left = left;
		this->right = right;
	}

	/*!
	 * Destructor.
	 *
	 * The subtrees are deleted with the object
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~And() {
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

#endif /* STORM_FORMULA_ABSTRACT_AND_H_ */
