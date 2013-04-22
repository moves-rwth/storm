/*
 * AbstractCslFormula.h
 *
 *  Created on: 19.04.2013
 *      Author: thomas
 */

#ifndef ABSTRACTCSLFORMULA_H_
#define ABSTRACTCSLFORMULA_H_

#include "src/formula/abstract/AbstractFormula.h"

namespace storm {
namespace formula {
namespace csl {

/*!
 * Abstract base class for all CSL root formulas.
 */
template <class T>
class AbstractCslFormula : public virtual storm::formula::abstract::AbstractFormula<T>{
public:
	virtual ~AbstractCslFormula() {
		// Intentionally left empty
	}
};

} /* namespace csl */
} /* namespace formula */
} /* namespace storm */
#endif /* ABSTRACTCSLFORMULA_H_ */
