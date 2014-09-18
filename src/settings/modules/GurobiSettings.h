#ifndef STORM_SETTINGS_MODULES_GUROBISETTINGS_H_
#define STORM_SETTINGS_MODULES_GUROBISETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for Gurobi.
             */
            class GurobiSettings : public ModuleSettings {
            public:
                GurobiSettings(storm::settings::SettingsManager& settingsManager);
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_GUROBISETTINGS_H_ */
