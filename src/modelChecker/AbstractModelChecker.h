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

#include <iostream>

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
	
public:
	template <template <class T> class Target>
	const Target<Type>* as() const {
		try {
			const Target<Type>* target = dynamic_cast<const Target<Type>*>(this);
			return target;
		} catch (std::bad_cast& bc) {
			std::cerr << "Bad cast: tried to cast " << typeid(*this).name() << " to " << typeid(Target<Type>).name() << std::endl;
		}
		return nullptr;
	}
};

} //namespace modelChecker

} //namespace storm

#endif /* STORM_MODELCHECKER_DTMCPRCTLMODELCHECKER_H_ */
