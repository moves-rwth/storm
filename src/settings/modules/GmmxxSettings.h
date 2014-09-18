#ifndef STORM_SETTINGS_MODULES_GMMXXSETTINGS_H_
#define STORM_SETTINGS_MODULES_GMMXXSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for gmm++.
             */
            class GmmxxSettings : public ModuleSettings {
            public:
                GmmxxSettings(storm::settings::SettingsManager& settingsManager);
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_GMMXXSETTINGS_H_ */
