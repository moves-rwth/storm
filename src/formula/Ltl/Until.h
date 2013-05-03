/*
 * Until.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_LTL_UNTIL_H_
#define STORM_FORMULA_LTL_UNTIL_H_

#include "AbstractLtlFormula.h"
#include "src/formula/abstract/Until.h"
#include "src/formula/AbstractFormulaChecker.h"

namespace storm {
namespace property {
namespace ltl {

template <class T> class Until;

/*!
 *  @brief Interface class for model checkers that support Until.
 *
 *  All model checkers that support the formula class Until must inherit
 *  this pure virtual class.
 */
template <class T>
class IUntilModelChecker {
    public:
		/*!
         *  @brief Evaluates Until formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T>* checkUntil(const Until<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for an abstract (path) formula tree with an Until node as root.
 *
 * Has two Abstract LTL formulas as sub formulas/trees.
 *
 * @par Semantics
 * The formula holds iff eventually, formula \e right (the right subtree) holds, and before,
 * \e left holds always.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractLtlFormula
 */
template <class T>
class Until : public storm::property::abstract::Until<T, AbstractLtlFormula<T>>,
				  public AbstractLtlFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	Until() {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param left The left formula subtree
	 * @param right The left formula subtree
	 */
	Until(AbstractLtlFormula<T>* left, AbstractLtlFormula<T>* right)
		: storm::property::abstract::Until<T, AbstractLtlFormula<T>>(left, right) {
		// Intentionally left empty
	}

	/*!
	 * Destructor.
	 *
	 * Also deletes the subtrees.
	 * (this behaviour can be prevented by setting the subtrees to NULL before deletion)
	 */
	virtual ~Until() {
	  // Intentionally left empty
	}

	/*!
	 *	@brief Return string representation of this formula.
	 *
	 * In LTL, brackets are needed around the until, as Until may appear nested (in other logics, Until always is the
	 * root of a path formula); hence this function is overwritten in this class.
	 *
	 * @return A string representation of the formula.
	 */
	virtual std::string toString() const {
		return "(" + storm::property::abstract::Until<T, AbstractLtlFormula<T>>::toString() + ")";
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedUntil-object that is identical the called object.
	 */
	virtual AbstractLtlFormula<T>* clone() const {
		Until<T>* result = new Until();
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
	 * @returns A vector indicating the probability that the formula holds for each state.
	 */
	virtual std::vector<T> *check(const storm::modelchecker::AbstractModelChecker<T>& modelChecker) const {
		return modelChecker.template as<IUntilModelChecker>()->checkUntil(*this);
	}
};

} //namespace ltl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_LTL_UNTIL_H_ */
