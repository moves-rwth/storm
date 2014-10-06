#ifndef STORM_SETTINGS_MODULES_PARAMETRICSETTINGS_H_
#define STORM_SETTINGS_MODULES_PARAMETRICSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {

            /*!
             * This class represents the settings for parametric model checking.
             */
            class ParametricSettings : public ModuleSettings {
            public:
                /*!
                 * Creates a new set of parametric model checking settings that is managed by the given manager.
                 *
                 * @param settingsManager The responsible manager.
                 */
                ParametricSettings(storm::settings::SettingsManager& settingsManager);
                
                /*!
                 * Retrieves whether the option to eliminate entry states in the very end is set.
                 *
                 * @return True iff the option is set.
                 */
                bool isEliminateEntryStatesLastSet() const;
                
                const static std::string moduleName;
                
            private:
                const static std::string entryStatesLastOptionName;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_PARAMETRICSETTINGS_H_ */