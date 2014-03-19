#ifndef STORM_STORAGE_DD_DDMANAGER_H_
#define STORM_STORAGE_DD_DDMANAGER_H_

#include "src/storage/dd/DdType.h"

namespace storm {
    namespace dd {
        // Declare DdManager class so we can then specialize it for the different DD types.
        template<DdType Type> class DdManager;
    }
}

#endif /* STORM_STORAGE_DD_DDMANAGER_H_ */