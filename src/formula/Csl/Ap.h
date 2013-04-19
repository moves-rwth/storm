/*
 * Ap.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_CSL_AP_H_
#define STORM_FORMULA_CSL_AP_H_

#include "AbstractStateFormula.h"
#include "src/formula/abstract/Ap.h"
#include "src/formula/AbstractFormulaChecker.h"
#include "src/modelchecker/ForwardDeclarations.h"

namespace storm {
namespace formula {
namespace csl {

template <class T> class Ap;

/*!
 *  @brief Interface class for model checkers that support Ap.
 *
 *  All model checkers that support the formula class Ap must inherit
 *  this pure virtual class.
 */
template <class T>
class IApModelChecker {
    public:
		/*!
         *  @brief Evaluates Ap formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual storm::storage::BitVector* checkAp(const Ap<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for a Abstract formula tree with atomic proposition as root.
 *
 * This class represents the leaves in the formula tree.
 *
 * @see AbstractFormula
 * @see AbstractFormula
 */
template <class T>
class Ap : public storm::formula::abstract::Ap<T>,
			  public AbstractStateFormula<T> {

public:
	/*!
	 * Constructor
	 *
	 * Creates a new atomic proposition leaf, with the label Ap
	 *
	 * @param ap The string representing the atomic proposition
	 */
	Ap(std::string ap)
		: storm::formula::abstract::Ap<T>(ap) {
		// Intentionally left empty
	}

	/*!
	 * Destructor.
	 * At this time, empty...
	 */
	virtual ~Ap() {
		// Intentionally left empty
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual AbstractStateFormula<T>* clone() const {
		return new Ap(this->getAp());
	}

	/*!
	 * Calls the model checker to check this formula.
	 * Needed to infer the correct type of formula class.
	 *
	 * @note This function should only be called in a generic check function of a model checker class. For other uses,
	 *       the methods of the model checker should be used.
	 *
	 * @returns A bit vector indicating all states that satisfy the formula represented by the called object.
	 */
	virtual storm::storage::BitVector *check(const storm::modelchecker::AbstractModelChecker<T>& modelChecker) const {
		return modelChecker.template as<IApModelChecker>()->checkAp(*this);
	}

};

} //namespace abstract

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_CSL_AP_H_ */
