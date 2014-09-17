#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the general options.
             */
            class GeneralSettings : public ModuleSettings {
                GeneralSettings(storm::settings::SettingsManager& settingsManager);
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm