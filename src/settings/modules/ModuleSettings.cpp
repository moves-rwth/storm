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
            
        } // namespace modules
    } // namespace settings
} // namespace storm