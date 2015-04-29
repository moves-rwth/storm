#ifndef STORM_STORAGE_DD_BDD_H_
#define STORM_STORAGE_DD_BDD_H_

#include "src/storage/dd/Dd.h"
#include "src/storage/dd/DdType.h"

namespace storm {
    namespace dd {
        // Declare Bdd class so we can then specialize it for the different BDD types.
        template<DdType Type> class Bdd;
    }
}

#endif /* STORM_STORAGE_DD_BDD_H_ */