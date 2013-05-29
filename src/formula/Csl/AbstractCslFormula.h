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

/*!
 * @brief An encapsulation type for the Return Type of the CslParser. As a pure virtual Class can not be an r-value return type, it has to be encapsulated.
 * @see CslParser
 */
template <typename T>
using AbstractCslFormularRef_t = std::reference_wrapper<storm::property::csl::AbstractCslFormula<T>>;

} /* namespace csl */
} /* namespace property */
} /* namespace storm */
#endif /* ABSTRACTCSLFORMULA_H_ */
