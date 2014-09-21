#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            ModuleSettings::ModuleSettings(storm::settings::SettingsManager& settingsManager, std::string const& moduleName) : settingsManager(settingsManager), moduleName(moduleName) {
                // Intentionally left empty.
            }
            
            bool ModuleSettings::check() const {
                return true;
            }
            
            storm::settings::SettingsManager const& ModuleSettings::getSettingsManager() const {
                return this->settingsManager;
            }
            
            void ModuleSettings::set(std::string const& name) {
                return this->getOption(name).setHasOptionBeenSet();
            }
            
            void ModuleSettings::unset(std::string const& name) {
                return this->getOption(name).setHasOptionBeenSet(false);
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm