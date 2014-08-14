/*
 * AbstractPrctlFormula.h
 *
 *  Created on: 16.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_PRCTL_ABSTRACTPRCTLFORMULA_H_
#define STORM_FORMULA_PRCTL_ABSTRACTPRCTLFORMULA_H_

#include "src/formula/AbstractFormula.h"

namespace storm {
namespace property {
namespace prctl {

template <class T> class ProbabilisticBoundOperator;
template <class T> class Eventually;
template <class T> class Until;

}
}
}

namespace storm {
namespace property {
namespace prctl {

/*!
 * Interface class for all PRCTL root formulas.
 */
template<class T>
class AbstractPrctlFormula : public virtual storm::property::AbstractFormula<T> {
public:
	virtual ~AbstractPrctlFormula() {
		// Intentionally left empty
	}

	/*! Returns whether the formula is a probabilistic bound reachability formula.
	 *  Returns true iff the formula conforms to the following pattern.
	 *  Pattern: P[<,<=,>,>=]p ([psi U, E] phi) whith psi, phi propositional logic formulas (consisiting only of And, Or, Not and AP).
	 *  That is, a probabilistic bound operator as root with a single until or eventually formula directly below it, whose subformulas are propositional
	 *  (denoting some set of atomic propositions).
	 *
	 *  @return True iff this is a probabilistic bound reachability formula.
	 */
	bool isProbEventuallyAP() const {

		// Test if a probabilistic bound operator is at the root.
		if(dynamic_cast<storm::property::prctl::ProbabilisticBoundOperator<T> const *>(this) == nullptr) {
			return false;
		}

		auto probFormula = dynamic_cast<storm::property::prctl::ProbabilisticBoundOperator<T> const *>(this);

		// Check if the direct subformula of the probabilistic bound operator is an eventually or until formula.
		if(std::dynamic_pointer_cast<storm::property::prctl::Eventually<T>>(probFormula->getChild()).get() != nullptr) {

			// Get the subformula and check if its subformulas are propositional.
			auto eventuallyFormula = std::dynamic_pointer_cast<storm::property::prctl::Eventually<T>>(probFormula->getChild());
			return eventuallyFormula->getChild()->isPropositional();
		}
		else if(std::dynamic_pointer_cast<storm::property::prctl::Until<T>>(probFormula->getChild()).get() != nullptr) {

			// Get the subformula and check if its subformulas are propositional.
			auto untilFormula = std::dynamic_pointer_cast<storm::property::prctl::Until<T>>(probFormula->getChild());
			return untilFormula->getLeft()->isPropositional() && untilFormula->getRight()->isPropositional();
		}

		return false;
	}
};

} /* namespace prctl */
} /* namespace property */
} /* namespace storm */
#endif /* ABSTRACTPRCTLFORMULA_H_ */
