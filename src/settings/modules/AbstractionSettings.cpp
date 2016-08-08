#include "src/settings/modules/AbstractionSettings.h"

#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"

namespace storm {
    namespace settings {
        namespace modules {
         
            const std::string AbstractionSettings::moduleName = "abstraction";
            const std::string AbstractionSettings::addAllGuardsOptionName = "allguards";
            
            AbstractionSettings::AbstractionSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, addAllGuardsOptionName, true, "Sets whether all guards are added as initial predicates.").build());
            }
            
            bool AbstractionSettings::isAddAllGuardsSet() const {
                return this->getOption(addAllGuardsOptionName).getHasOptionBeenSet();
            }
            
        }
    }
}