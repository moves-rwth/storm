#include "storm/settings/modules/ModelCheckerSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"


namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string ModelCheckerSettings::moduleName = "modelchecker";
            const std::string ModelCheckerSettings::filterRewZeroOptionName = "filterrewzero";

            ModelCheckerSettings::ModelCheckerSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, filterRewZeroOptionName, false, "If set, states with reward zero are filtered out, potentially reducing the size of the equation system").setIsAdvanced().build());
            }
            
            bool ModelCheckerSettings::isFilterRewZeroSet() const {
                return this->getOption(filterRewZeroOptionName).getHasOptionBeenSet();
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm
