#ifndef STORM_FORMULA_PRCTLFORMULACHECKER_H_
#define STORM_FORMULA_PRCTLFORMULACHECKER_H_

#include "src/formula/AbstractFormulaChecker.h"
#include "src/formula/Prctl.h"

#include <iostream>

namespace storm {
namespace property {

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
		 *	Implementation of AbstractFormulaChecker::validate() using code
		 *	looking exactly like the sample code given there.
		 */
		virtual bool validate(const storm::property::abstract::AbstractFormula<T>* formula) const {
			// What to support: Principles of Model Checking Def. 10.76 + syntactic sugar
			if (
					dynamic_cast<const storm::property::prctl::And<T>*>(formula) ||
					dynamic_cast<const storm::property::prctl::Ap<T>*>(formula) ||
					dynamic_cast<const storm::property::prctl::BoundedUntil<T>*>(formula) ||
					dynamic_cast<const storm::property::prctl::Eventually<T>*>(formula) ||
					dynamic_cast<const storm::property::prctl::Globally<T>*>(formula) ||
					dynamic_cast<const storm::property::prctl::Next<T>*>(formula) ||
					dynamic_cast<const storm::property::prctl::Not<T>*>(formula) ||
					dynamic_cast<const storm::property::prctl::Or<T>*>(formula) ||
					dynamic_cast<const storm::property::prctl::ProbabilisticNoBoundOperator<T>*>(formula) ||
					dynamic_cast<const storm::property::prctl::ProbabilisticBoundOperator<T>*>(formula) ||
					dynamic_cast<const storm::property::prctl::Until<T>*>(formula)
				) {
				return formula->validate(*this);
			}
			return false;
		}
};

} // namespace property
} // namespace storm

#endif
