/*
 * DtmcPrctlModelChecker.h
 *
 *  Created on: 22.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_
#define STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_

namespace storm { namespace modelChecker {
template <class Type> class AbstractModelChecker;
}}

//#include "src/formula/Formulas.h"
#include "src/formula/Or.h"
#include "src/formula/Ap.h"
#include "src/storage/BitVector.h"

namespace storm {
namespace modelChecker {

/*!
 * @brief
 * Interface for model checker classes.
 *
 * This class provides basic functions that are the same for all subclasses, but mainly only declares
 * abstract methods that are to be implemented in concrete instances.
 *
 * @attention This class is abstract.
 */
template<class Type>
class AbstractModelChecker :
	public virtual storm::formula::IOrModelChecker<Type>,
	public virtual storm::formula::IApModelChecker<Type>
	{
};

} //namespace modelChecker

} //namespace storm

#endif /* STORM_MODELCHECKER_DTMCPRCTLMODELCHECKER_H_ */
