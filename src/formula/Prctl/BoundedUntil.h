/*
 * BoundedUntil.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PRCTL_BOUNDEDUNTIL_H_
#define STORM_FORMULA_PRCTL_BOUNDEDUNTIL_H_

#include "src/formula/abstract/BoundedUntil.h"
#include "src/formula/Prctl/AbstractPathFormula.h"
#include "src/formula/Prctl/AbstractStateFormula.h"
#include "boost/integer/integer_mask.hpp"
#include <string>
#include "src/modelchecker/ForwardDeclarations.h"

namespace storm {
namespace formula {
namespace prctl {

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
        virtual std::vector<T>* checkBoundedUntil(const BoundedUntil<T>& obj, bool qualitative) const = 0;
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
 * @see AbstractPrctlFormula
 */
template <class T>
class BoundedUntil : public storm::formula::abstract::BoundedUntil<T, AbstractStateFormula<T>>,
							public AbstractPathFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	BoundedUntil() {
		//Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param left The left formula subtree
	 * @param right The left formula subtree
	 * @param bound The maximal number of steps
	 */
	BoundedUntil(AbstractStateFormula<T>* left, AbstractStateFormula<T>* right,
					 uint_fast64_t bound) :
					 storm::formula::abstract::BoundedUntil<T, AbstractStateFormula<T>>(left,right,bound) {
		//intentionally left empty
	}

	/*!
	 * Destructor.
	 *
	 * Also deletes the subtrees.
	 * (this behaviour can be prevented by setting the subtrees to NULL before deletion)
	 */
	virtual ~BoundedUntil() {
	  //intentionally left empty
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
		result->setBound(this->getBound());
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
	virtual std::vector<T> *check(const storm::modelchecker::AbstractModelChecker<T>& modelChecker, bool qualitative) const {
		return modelChecker.template as<IBoundedUntilModelChecker>()->checkBoundedUntil(*this, qualitative);
	}
};

} //namespace prctl
} //namespace formula
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_BOUNDEDUNTIL_H_ */
