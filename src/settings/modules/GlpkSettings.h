#ifndef STORM_SETTINGS_MODULES_GLPKSETTINGS_H_
#define STORM_SETTINGS_MODULES_GLPKSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for glpk.
             */
            class GlpkSettings : public ModuleSettings {
            public:
                GlpkSettings(storm::settings::SettingsManager& settingsManager);
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_GLPKSETTINGS_H_ */
