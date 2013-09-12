/*
 * BoundedNaryUntil.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PRCTL_BOUNDEDNARYUNTIL_H_
#define STORM_FORMULA_PRCTL_BOUNDEDNARYUNTIL_H_

#include "src/formula/abstract/BoundedNaryUntil.h"
#include "src/formula/Prctl/AbstractPathFormula.h"
#include "src/formula/Prctl/AbstractStateFormula.h"
#include <cstdint>
#include <string>
#include <vector>
#include <tuple>
#include <sstream>
#include "src/modelchecker/prctl/ForwardDeclarations.h"

namespace storm {
namespace property {
namespace prctl {


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
        virtual std::vector<T>* checkBoundedNaryUntil(const BoundedNaryUntil<T>& obj, bool qualitative) const = 0;
};

/*!
 * @brief
 * Class for an abstract (path) formula tree with a BoundedNaryUntil node as root.
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
 * @see AbstractPrctlFormula
 */
template <class T>
class BoundedNaryUntil : public storm::property::abstract::BoundedNaryUntil<T, AbstractStateFormula<T>>,
								 public AbstractPathFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	BoundedNaryUntil() {

	}

	/*!
	 * Constructor
	 *
	 * @param left The left formula subtree
	 * @param right The left formula subtree
	 */
	BoundedNaryUntil(AbstractStateFormula<T>* left, std::vector<std::tuple<AbstractStateFormula<T>*,T,T>>* right) :
		storm::property::abstract::BoundedNaryUntil<T, AbstractStateFormula<T>>(left, right){

	}

	/*!
	 * Destructor.
	 *
	 * Also deletes the subtrees.
	 * (this behaviour can be prevented by setting the subtrees to NULL before deletion)
	 */
	virtual ~BoundedNaryUntil() {
		//intentionally left empty
	}


	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedNaryUntil-object that is identical the called object.
	 */
	virtual AbstractPathFormula<T>* clone() const override {
		BoundedNaryUntil<T>* result = new BoundedNaryUntil<T>();
		if (this->leftIsSet()) {
			result->setLeft(this->getLeft().clone());
		}
		if (this->rightIsSet()) {
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
	virtual std::vector<T>* check(const storm::modelchecker::prctl::AbstractModelChecker<T>& modelChecker, bool qualitative) const override {
		return modelChecker.template as<IBoundedNaryUntilModelChecker>()->checkBoundedNaryUntil(*this, qualitative);
	}

};

} //namespace prctl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_BOUNDEDNARYUNTIL_H_ */
