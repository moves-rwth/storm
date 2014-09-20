#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            ModuleSettings::ModuleSettings(storm::settings::SettingsManager& settingsManager) : settingsManager(settingsManager) {
                // Intentionally left empty.
            }
            
            bool ModuleSettings::check() const {
                return true;
            }
            
            storm::settings::SettingsManager const& ModuleSettings::getSettingsManager() const {
                return this->settingsManager;
            }
            
            void ModuleSettings::set(std::string const& name) const {
                return this->getOption(longName).setHasOptionBeenSet();
            }
            
            void ModuleSettings::unset(std::string const& longName) const {
                return this->getOption(longName).setHasOptionBeenSet(false);
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm