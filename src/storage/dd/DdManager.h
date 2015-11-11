#ifndef STORM_STORAGE_DD_DDMANAGER_H_
#define STORM_STORAGE_DD_DDMANAGER_H_

#include "src/storage/dd/DdType.h"
#include "src/storage/dd/DdMetaVariable.h"
#include "src/storage/expressions/Variable.h"

namespace storm {
    namespace dd {
        // Declare DdManager class so we can then specialize it for the different DD types.
        template<DdType LibraryType>
        class DdManager {
        public:
            Bdd<LibraryType> getBddOne() const;
            Bdd<LibraryType> getBddZero() const;
            DdMetaVariable<LibraryType> const& getMetaVariable(storm::expressions::Variable const& variable) const;
        };
    }
}

#endif /* STORM_STORAGE_DD_DDMANAGER_H_ */