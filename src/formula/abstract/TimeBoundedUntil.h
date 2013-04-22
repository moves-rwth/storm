/*
 * TimeBoundedUntil.h
 *
 *  Created on: 10.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_ABSTRACT_TIMEBOUNDEDUNTIL_H_
#define STORM_FORMULA_ABSTRACT_TIMEBOUNDEDUNTIL_H_

#include "TimeBoundedOperator.h"

namespace storm {
namespace formula {
namespace abstract {


/**
 * @brief
 * Class for a Abstract formula tree with an time bounded until operator as root.
 *
 * @tparam FormulaType The type of the subformula.
 * 		  The instantiation of FormulaType should be a subclass of AbstractFormula, as the functions
 * 		  "toString" and "conforms" of the subformulas are needed.
 */
template <class T, class FormulaType>
class TimeBoundedUntil: public TimeBoundedOperator<T> {
public:
	/**
	 * Constructor providing bounds only;
	 * Sub formulas are set to null.
	 *
	 * @param lowerBound
	 * @param upperBound
	 */
	TimeBoundedUntil(T lowerBound, T upperBound) :
		TimeBoundedOperator<T>(lowerBound, upperBound) {
		this->left = nullptr;
		this->right = nullptr;
	}


	/**
	 * Full constructor
	 * @param lowerBound
	 * @param upperBound
	 * @param left
	 * @param right
	 */
	TimeBoundedUntil(T lowerBound, T upperBound, FormulaType* left, FormulaType* right) :
		TimeBoundedOperator<T>(lowerBound, upperBound) {
		this->left = left;
		this->right = right;
	}

	virtual ~TimeBoundedUntil() {
	   if (left != nullptr) {
	   	delete left;
	   }
	   if (right != nullptr) {
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
	virtual std::string toString() const {
		std::string result = left->toString();
		result += " U";
		result += TimeBoundedOperator<T>::toString();
		result += " ";
		result += right->toString();
		return result;
	}

	/*!
     *  @brief Checks if the subtree conforms to some logic.
     *
     *  @param checker Formula checker object.
     *  @return true iff the subtree conforms to some logic.
     */
	virtual bool conforms(const AbstractFormulaChecker<T>& checker) const {
		return checker.conforms(this->left) && checker.conforms(this->right);
	}

private:
	FormulaType* left;
	FormulaType* right;
};

} /* namespace abstract */
} /* namespace formula */
} /* namespace storm */

#endif /* STORM_FORMULA_ABSTRACT_TIMEBOUNDEDUNTIL_H_ */
