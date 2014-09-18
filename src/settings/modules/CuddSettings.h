#ifndef STORM_SETTINGS_MODULES_CUDDSETTINGS_H_
#define STORM_SETTINGS_MODULES_CUDDSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for CUDD.
             */
            class CuddSettings : public ModuleSettings {
            public:
                CuddSettings(storm::settings::SettingsManager& settingsManager);
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_CUDDSETTINGS_H_ */
