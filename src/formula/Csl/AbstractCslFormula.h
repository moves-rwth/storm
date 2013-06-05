/*
 * AbstractCslFormula.h
 *
 *  Created on: 19.04.2013
 *      Author: Thomas Heinemann
 */

#ifndef ABSTRACTCSLFORMULA_H_
#define ABSTRACTCSLFORMULA_H_

#include "src/formula/abstract/AbstractFormula.h"

namespace storm {
namespace property {
namespace csl {

/*!
 * Abstract base class for all CSL root formulas.
 */
template <class T>
class AbstractCslFormula : public virtual storm::property::abstract::AbstractFormula<T>{
public:
	virtual ~AbstractCslFormula() {
		// Intentionally left empty
	}
};

} /* namespace csl */
} /* namespace property */
} /* namespace storm */
#endif /* ABSTRACTCSLFORMULA_H_ */
