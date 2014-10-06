#include "src/settings/modules/ParametricSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string ParametricSettings::moduleName = "parametric";
            const std::string ParametricSettings::entryStatesLastOptionName = "entrylast";
            
            ParametricSettings::ParametricSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, entryStatesLastOptionName, true, "Sets whether the entry states are eliminated last.").build());
            }
            
            bool ParametricSettings::isEliminateEntryStatesLastSet() const {
                return this->getOption(entryStatesLastOptionName).getHasOptionBeenSet();
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm