#ifndef STORM_STORAGE_DD_DD_H_
#define STORM_STORAGE_DD_DD_H_

#include "src/storage/dd/DdType.h"

namespace storm {
    namespace dd {
        // Declare Dd class so we can then specialize it for the different DD types.
        template<DdType Type> class Dd;
    }
}

#endif /* STORM_STORAGE_DD_DD_H_ */