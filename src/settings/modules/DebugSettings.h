#ifndef STORM_SETTINGS_MODULES_DEBUGSETTINGS_H_
#define STORM_SETTINGS_MODULES_DEBUGSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the debug settings.
             */
            class DebugSettings : public ModuleSettings {
            public:
                DebugSettings(storm::settings::SettingsManager& settingsManager);
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_DEBUGSETTINGS_H_ */