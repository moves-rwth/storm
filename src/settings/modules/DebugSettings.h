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
                /*!
                 * Creates a new set of debug settings that is managed by the given manager.
                 *
                 * @param settingsManager The responsible manager.
                 */
                DebugSettings(storm::settings::SettingsManager& settingsManager);
                
                /*!
                 * Retrieves whether the debug option was set.
                 *
                 * @return True iff the debug option was set.
                 */
                bool isDebugSet() const;
                
                /*!
                 * Retrieves whether the trace option was set.
                 *
                 * @return True iff the trace option was set.
                 */
                bool isTraceSet() const;
                
                /*!
                 * Retrieves whether the logfile option was set.
                 *
                 * @return True iff the logfile option was set.
                 */
                bool isLogfileSet() const;
                
                /*!
                 * Retrieves the name of the log file if the logfile option was set.
                 *
                 * @return The name of the log file.
                 */
                std::string getLogfilename() const;
                
            private:
                // Define the string names of the options as constants.
                static const std::string moduleName;
                static const std::string debugOptionName;
                static const std::string traceOptionName;
                static const std::string logfileOptionName;
                static const std::string logfileOptionShortName;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_DEBUGSETTINGS_H_ */