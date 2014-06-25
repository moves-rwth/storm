#ifndef STORM_STORAGE_DD_ODD_H_
#define STORM_STORAGE_DD_ODD_H_

#include "src/storage/dd/DdType.h"

namespace storm {
    namespace dd {
        // Declare Odd class so we can then specialize it for the different DD types.
        template<DdType Type> class Odd;
    }
}

#endif /* STORM_STORAGE_DD_ODD_H_ */