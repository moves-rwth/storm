#ifndef STORM_SETTINGS_MODULES_GENERALSETTINGS_H_
#define STORM_SETTINGS_MODULES_GENERALSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the general settings.
             */
            class GeneralSettings : public ModuleSettings {
            public:
                GeneralSettings(storm::settings::SettingsManager& settingsManager);
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_GENERALSETTINGS_H_ */
