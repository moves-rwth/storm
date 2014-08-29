/*
 * Ap.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PRCTL_AP_H_
#define STORM_FORMULA_PRCTL_AP_H_

#include "src/properties/prctl/AbstractStateFormula.h"
#include "src/modelchecker/prctl/ForwardDeclarations.h"

namespace storm {
namespace properties {
namespace prctl {

// Forward declaration for the interface class.
template <class T> class Ap;

/*!
 * Interface class for model checkers that support Ap.
 *
 * All model checkers that support the formula class Ap must inherit
 * this pure virtual class.
 */
template <class T>
class IApModelChecker {
    public:

		/*!
		 * Empty virtual destructor.
		 */
		virtual ~IApModelChecker() {
			// Intentionally left empty
		}

		/*!
         * Evaluates an Ap formula within a model checker.
         *
         * @param obj Formula object with subformulas.
         * @return The modelchecking result of the formula for every state.
         */
        virtual storm::storage::BitVector checkAp(const Ap<T>& obj) const = 0;
};

/*!
 * Class for a Prctl formula tree with an atomic proposition as root.
 *
 * This class represents leaves in the formula tree.
 *
 * @see AbstractPrctlFormula
 * @see AbstractStateFormula
 */
template <class T>
class Ap : public AbstractStateFormula<T> {

public:

	/*!
	 * Creates a new atomic proposition leaf, with the given label.
	 *
	 * @param ap A string representing the atomic proposition.
	 */
	Ap(std::string ap) : ap(ap) {
		// Intentionally left empty.
	}

	/*!
	 * Empty virtual destructor.
	 */
	virtual ~Ap() {
		// Intentionally left empty
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones.
	 *
	 * @returns A new Ap object that is a deep copy of the called object.
	 */
	virtual std::shared_ptr<AbstractStateFormula<T>> clone() const override {
		auto result = std::make_shared<Ap<T>>(this->getAp());
		return result;
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
	virtual storm::storage::BitVector check(const storm::modelchecker::prctl::AbstractModelChecker<T>& modelChecker) const override {
		return modelChecker.template as<IApModelChecker>()->checkAp(*this);
	}

	/*!
	 * A string representing the atomic proposition.
	 *
	 * @returns A string representing the leaf.
	 */
	virtual std::string toString() const override {
		return getAp();
	}

	/*!
	 * Returns whether the formula is a propositional logic formula.
	 * That is, this formula and all its subformulas consist only of And, Or, Not and AP.
	 *
	 * @return True iff this is a propositional logic formula.
	 */
	virtual bool isPropositional() const override {
		return true;
	}

	/*!
	 * Gets the name of the atomic proposition.
	 *
	 * @returns The name of the atomic proposition.
	 */
	std::string const & getAp() const {
		return ap;
	}

private:

	// The atomic proposition referenced by this leaf.
	std::string ap;
};

} //namespace abstract

} //namespace properties

} //namespace storm

#endif /* STORM_FORMULA_PRCTL_AP_H_ */
