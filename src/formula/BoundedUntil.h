/*
 * BoundedUntil.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_BOUNDEDUNTIL_H_
#define STORM_FORMULA_BOUNDEDUNTIL_H_

#include "src/formula/AbstractPathFormula.h"
#include "src/formula/AbstractStateFormula.h"
#include "src/modelChecker/AbstractModelChecker.h"
#include "boost/integer/integer_mask.hpp"
#include <string>
#include "src/formula/AbstractFormulaChecker.h"

namespace storm {
namespace formula {

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
        virtual std::vector<T>* checkBoundedUntil(const BoundedUntil<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for a Abstract (path) formula tree with a BoundedUntil node as root.
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
 * @see AbstractFormula
 */
template <class T>
class BoundedUntil : public AbstractPathFormula<T> {

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
	BoundedUntil(AbstractStateFormula<T>* left, AbstractStateFormula<T>* right,
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
	virtual std::string toString() const {
		std::string result = left->toString();
		result += " U<=";
		result += std::to_string(bound);
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
		BoundedUntil<T>* result = new BoundedUntil<T>();
		result->setBound(bound);
		if (left != NULL) {
			result->setLeft(left->clone());
		}
		if (right != NULL) {
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
	virtual std::vector<T> *check(const storm::modelChecker::AbstractModelChecker<T>& modelChecker) const {
		return modelChecker.template as<IBoundedUntilModelChecker>()->checkBoundedUntil(*this);
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

private:
	AbstractStateFormula<T>* left;
	AbstractStateFormula<T>* right;
	uint_fast64_t bound;
};

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_BOUNDEDUNTIL_H_ */
