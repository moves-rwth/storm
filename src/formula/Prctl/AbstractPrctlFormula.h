/*
 * AbstractPrctlFormula.h
 *
 *  Created on: 16.04.2013
 *      Author: thomas
 */

#ifndef STORM_FORMULA_PRCTL_ABSTRACTPRCTLFORMULA_H_
#define STORM_FORMULA_PRCTL_ABSTRACTPRCTLFORMULA_H_

#include "src/formula/abstract/AbstractFormula.h"

namespace storm {
namespace formula {
namespace prctl {

template<class T>
class AbstractPrctlFormula : public virtual storm::formula::abstract::AbstractFormula<T> {
public:
	virtual ~AbstractPrctlFormula() {
		// Intentionally left empty
	}
};

} /* namespace prctl */
} /* namespace formula */
} /* namespace storm */
#endif /* ABSTRACTPRCTLFORMULA_H_ */
