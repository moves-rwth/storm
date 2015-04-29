#ifndef STORM_STORAGE_DD_ADD_H_
#define STORM_STORAGE_DD_ADD_H_

#include "src/storage/dd/Dd.h"
#include "src/storage/dd/DdType.h"

namespace storm {
    namespace dd {
        // Declare Add class so we can then specialize it for the different ADD types.
        template<DdType Type> class Add;
    }
}

#endif /* STORM_STORAGE_DD_ADD_H_ */