#ifndef STORM_STORAGE_DD_DDMETAVARIBLE_H_
#define STORM_STORAGE_DD_DDMETAVARIBLE_H_

#include <vector>

#include "src/storage/dd/DdType.h"

namespace storm {
    namespace dd {
        // Declare DdMetaVariable class so we can then specialize it for the different DD types.
        template<DdType LibraryType>
        class DdMetaVariable {
        public:
            Bdd<LibraryType> getCube() const;
            uint_fast64_t getNumberOfDdVariables() const;
        };
    }
}

#endif /* STORM_STORAGE_DD_DDMETAVARIBLE_H_ */