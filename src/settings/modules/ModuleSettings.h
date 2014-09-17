#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This is the base class of the settings for a particular module.
             */
            class ModuleSettings {
            public:
                /*!
                 * Constructs a new settings object.
                 *
                 * @param settingsManager The manager responsible for these settings.
                 */
                ModuleSettings(storm::settings::SettingsManager& settingsManager);
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm