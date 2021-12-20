#ifndef STORM_STORAGE_DD_DDFORWARDITERATOR_H_
#define STORM_STORAGE_DD_DDFORWARDITERATOR_H_

#include "storm/storage/dd/DdType.h"

namespace storm {
namespace dd {
// Declare DdIterator class so we can then specialize it for the different DD types.
template<DdType Type, typename ValueType>
class AddIterator;
}  // namespace dd
}  // namespace storm

#endif /* STORM_STORAGE_DD_DDFORWARDITERATOR_H_ */
