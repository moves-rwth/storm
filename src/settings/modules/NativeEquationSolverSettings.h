#ifndef STORM_SETTINGS_MODULES_NATIVEEQUATIONSOLVERSETTINGS_H_
#define STORM_SETTINGS_MODULES_NATIVEEQUATIONSOLVERSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for the native equation solver.
             */
            class NativeEquationSolverSettings : public ModuleSettings {
            public:
                NativeEquationSolverSettings(storm::settings::SettingsManager& settingsManager);
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_NATIVEEQUATIONSOLVERSETTINGS_H_ */
