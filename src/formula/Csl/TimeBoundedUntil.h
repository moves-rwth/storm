/*
 * TimeBoundedUntil.h
 *
 *  Created on: 10.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_TIMEBOUNDEDUNTIL_H_
#define STORM_FORMULA_TIMEBOUNDEDUNTIL_H_

#include "TimeBoundedOperator.h"

namespace storm {
namespace formula {

template <class T> class TimeBoundedUntil;

/*!
 *  @brief Interface class for model checkers that support TimeBoundedUntil.
 *
 *  All model checkers that support the formula class BoundedEventually must inherit
 *  this pure virtual class.
 */
template <class T>
class ITimeBoundedUntilModelChecker {
    public:
		/*!
         *  @brief Evaluates TimeBoundedUntil formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T>* checkTimeBoundedUntil(const TimeBoundedUntil<T>& obj, bool qualitative) const = 0;
};

template <class T>
class TimeBoundedUntil: public storm::formula::TimeBoundedOperator<T> {
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
	TimeBoundedUntil(T lowerBound, T upperBound, AbstractStateFormula<T>* left, AbstractStateFormula<T>* right) :
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
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedUntil-object that is identical the called object.
	 */
	virtual AbstractPathFormula<T>* clone() const {
		TimeBoundedUntil<T>* result = new TimeBoundedUntil<T>(this->getLowerBound(), this->getUpperBound());
		if (left != nullptr) {
			result->setLeft(left->clone());
		}
		if (right != nullptr) {
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
	virtual std::vector<T>* check(const storm::modelchecker::AbstractModelChecker<T>& modelChecker, bool qualitative) const {
		return modelChecker.template as<ITimeBoundedUntilModelChecker>()->checkTimeBoundedUntil(*this, qualitative);
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
	AbstractStateFormula<T>* left;
	AbstractStateFormula<T>* right;
};

} /* namespace formula */
} /* namespace storm */
#endif /* STORM_FORMULA_TIMEBOUNDEDUNTIL_H_ */
