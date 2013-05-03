#ifndef STORM_FORMULA_PRCTLFORMULACHECKER_H_
#define STORM_FORMULA_PRCTLFORMULACHECKER_H_

#include "src/formula/AbstractFormulaChecker.h"
#include "src/formula/Prctl.h"

#include <iostream>

namespace storm {
namespace formula {

/*!
 *	@brief Checks formulas if they are within PRCTL.
 *
 *	This class implements AbstractFormulaChecker to check if a given formula
 *	is part of PRCTL logic.
 */
template <class T>
class PrctlFormulaChecker : public AbstractFormulaChecker<T> {
	public:
		/*!
		 *	Implementation of AbstractFormulaChecker::conforms() using code
		 *	looking exactly like the sample code given there.
		 */
		virtual bool conforms(const storm::formula::abstract::AbstractFormula<T>* formula) const {
			// What to support: Principles of Model Checking Def. 10.76 + syntactic sugar
			if (
					dynamic_cast<const storm::formula::prctl::And<T>*>(formula) ||
					dynamic_cast<const storm::formula::prctl::Ap<T>*>(formula) ||
					dynamic_cast<const storm::formula::prctl::BoundedUntil<T>*>(formula) ||
					dynamic_cast<const storm::formula::prctl::Eventually<T>*>(formula) ||
					dynamic_cast<const storm::formula::prctl::Globally<T>*>(formula) ||
					dynamic_cast<const storm::formula::prctl::Next<T>*>(formula) ||
					dynamic_cast<const storm::formula::prctl::Not<T>*>(formula) ||
					dynamic_cast<const storm::formula::prctl::Or<T>*>(formula) ||
					dynamic_cast<const storm::formula::prctl::ProbabilisticNoBoundOperator<T>*>(formula) ||
					dynamic_cast<const storm::formula::prctl::ProbabilisticBoundOperator<T>*>(formula) ||
					dynamic_cast<const storm::formula::prctl::Until<T>*>(formula)
				) {
				return formula->conforms(*this);
			}
			return false;
		}
};

} // namespace formula
} // namespace storm

#endif
