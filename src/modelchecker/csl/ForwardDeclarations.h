/*
 * ForwardDeclarations.h
 *
 *  Created on: 14.01.2013
 *      Author: Thomas Heinemann
 */

#ifndef STORM_MODELCHECKER_FORWARDDECLARATIONS_H_
#define STORM_MODELCHECKER_FORWARDDECLARATIONS_H_

// Forward declare the abstract model checker. This is used by the formula classes that need this declaration for
// the callback methods (i.e., the check methods).
namespace storm {
namespace modelchecker {
namespace csl {

template <class Type>
class AbstractModelChecker;

} //namespace csl
} //namespace modelchecker
} //namespace storm

#endif /* STORM_MODELCHECKER_FORWARDDECLARATIONS_H_ */
