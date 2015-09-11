#include "src/storage/prism/menu_games/AbstractionDdInformation.h"

#include "src/storage/dd/DdManager.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractionDdInformation<DdType, ValueType>::AbstractionDdInformation(std::shared_ptr<storm::dd::DdManager<DdType>> const& manager) : ddManager(manager) {
                // Intentionally left empty.
            }
            
        }
    }
}