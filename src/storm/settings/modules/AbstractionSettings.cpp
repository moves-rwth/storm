#include "storm/settings/modules/AbstractionSettings.h"

#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"

namespace storm {
    namespace settings {
        namespace modules {
         
            const std::string AbstractionSettings::moduleName = "abstraction";
            const std::string AbstractionSettings::addAllGuardsOptionName = "allguards";
            const std::string AbstractionSettings::splitPredicatesOptionName = "split-preds";
            
            AbstractionSettings::AbstractionSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, addAllGuardsOptionName, true, "Sets whether all guards are added as initial predicates.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, splitPredicatesOptionName, true, "Sets whether the predicates are split into atoms before they are added.").build());
            }
            
            bool AbstractionSettings::isAddAllGuardsSet() const {
                return this->getOption(addAllGuardsOptionName).getHasOptionBeenSet();
            }

            bool AbstractionSettings::isSplitPredicatesSet() const {
                return this->getOption(splitPredicatesOptionName).getHasOptionBeenSet();
            }
            
        }
    }
}
