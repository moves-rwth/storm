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

/*!
 * Interface class for all PRCTL root formulas.
 */
template<class T>
class AbstractPrctlFormula : public virtual storm::property::AbstractFormula<T> {
public:
	virtual ~AbstractPrctlFormula() {
		// Intentionally left empty
	}
};

} /* namespace prctl */
} /* namespace property */
} /* namespace storm */
#endif /* ABSTRACTPRCTLFORMULA_H_ */
