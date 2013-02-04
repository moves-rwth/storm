/*
 * BoundedNaryUntil.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_BOUNDEDNARYUNTIL_H_
#define STORM_FORMULA_BOUNDEDNARYUNTIL_H_

#include "src/formula/AbstractPathFormula.h"
#include "src/formula/AbstractStateFormula.h"
#include "src/modelChecker/AbstractModelChecker.h"
#include "boost/integer/integer_mask.hpp"
#include <string>
#include <vector>
#include <tuple>
#include <sstream>
#include "src/formula/AbstractFormulaChecker.h"

namespace storm {
namespace formula {

template <class T> class BoundedNaryUntil;

/*!
 *  @brief Interface class for model checkers that support BoundedNaryUntil.
 *   
 *  All model checkers that support the formula class BoundedNaryUntil must inherit
 *  this pure virtual class.
 */
template <class T>
class IBoundedNaryUntilModelChecker {
    public:
		/*!
         *  @brief Evaluates BoundedNaryUntil formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T>* checkBoundedNaryUntil(const BoundedNaryUntil<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for a Abstract (path) formula tree with a BoundedNaryUntil node as root.
 *
 * Has at least two Abstract state formulas as sub formulas and an interval
 * associated with all but the first sub formula. We'll call the first one
 * \e left and all other one \e right.
 *
 * @par Semantics
 * The formula holds iff \e left holds until eventually any of the \e right
 * formulas holds after a number of steps contained in the interval
 * associated with this formula.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractPathFormula
 * @see AbstractFormula
 */
template <class T>
class BoundedNaryUntil : public AbstractPathFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	BoundedNaryUntil() {
		this->left = NULL;
		this->right = new std::vector<std::tuple<AbstractStateFormula<T>*,T,T>>();
	}

	/*!
	 * Constructor
	 *
	 * @param left The left formula subtree
	 * @param right The left formula subtree
	 * @param bound The maximal number of steps
	 */
	BoundedNaryUntil(AbstractStateFormula<T>* left, AbstractStateFormula<T>* right,
					 uint_fast64_t bound) {
		this->left = left;
		this->right = right;
	}

	/*!
	 * Destructor.
	 *
	 * Also deletes the subtrees.
	 * (this behaviour can be prevented by setting the subtrees to NULL before deletion)
	 */
	virtual ~BoundedNaryUntil() {
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

	void setRight(std::vector<std::tuple<AbstractStateFormula<T>*,T,T>>* newRight) {
		right = newRight;
	}


	/*!
	 * Sets the right child node.
	 *
	 * @param newRight the new right child.
	 */
	void addRight(AbstractStateFormula<T>* newRight, T upperBound, T lowerBound) {
		this->right->push_back(std::tuple<AbstractStateFormula<T>*,T,T>(newRight, upperBound, lowerBound));
	}

	/*!
	 * @returns a pointer to the left child node
	 */
	const AbstractStateFormula<T>& getLeft() const {
		return *left;
	}

	/*!
	 * @returns a pointer to the right child nodes.
	 */
	const std::vector<std::tuple<AbstractStateFormula<T>*,T,T>>& getRight() const {
		return *right;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::stringstream result;
		result << "( " << left->toString();
		for (auto it = this->right->begin(); it != this->right->end(); ++it) {
			result << " U[" << std::get<1>(*it) << "," << std::get<2>(*it) << "] " << std::get<0>(*it)->toString();
		}
		result << ")";
		return result.str();
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedNaryUntil-object that is identical the called object.
	 */
	virtual AbstractPathFormula<T>* clone() const {
		BoundedNaryUntil<T>* result = new BoundedNaryUntil<T>();
		if (left != NULL) {
			result->setLeft(left->clone());
		}
		if (right != NULL) {
			std::vector<std::tuple<AbstractStateFormula<T>*,T,T>>* newright = new std::vector<std::tuple<AbstractStateFormula<T>*,T,T>>();
			for (auto it = this->right->begin(); it != this->right->end(); ++it) {
				newright->push_back(std::tuple<AbstractStateFormula<T>*,T,T>(std::get<0>(*it)->clone(), std::get<1>(*it), std::get<2>(*it)));
			}
			result->setRight(newright);
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
		return modelChecker.template as<IBoundedNaryUntilModelChecker>()->checkBoundedNaryUntil(*this);
	}
	
	/*!
     *  @brief Checks if all subtrees conform to some logic.
     * 
     *  @param checker Formula checker object.
     *  @return true iff all subtrees conform to some logic.
     */
	virtual bool conforms(const AbstractFormulaChecker<T>& checker) const {
		bool res = checker.conforms(this->left);
		for (auto it = this->right->begin(); it != this->right->end(); ++it) {
			res &= checker.conforms(std::get<0>(*it));
		}
		return res;
	}

private:
	AbstractStateFormula<T>* left;
	std::vector<std::tuple<AbstractStateFormula<T>*,T,T>>* right;
};

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_BOUNDEDNARYUNTIL_H_ */
