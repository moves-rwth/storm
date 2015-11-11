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
            std::vector<std::string> getDdVariableNames() const;
            
            /*!
             * Retrieves the (sorted) list of the variable indices of the DD variables given by the meta variable set.
             *
             * @param manager The manager responsible for the DD.
             * @param metaVariable The set of meta variables for which to retrieve the index list.
             * @return The sorted list of variable indices.
             */
            static std::vector<uint_fast64_t> getSortedVariableIndices(std::set<storm::expressions::Variable> const& metaVariables);
        };
    }
}

#endif /* STORM_STORAGE_DD_DDMANAGER_H_ */