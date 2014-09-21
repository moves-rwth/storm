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
                /*!
                 * Creates a new set of Gurobi settings that is managed by the given manager.
                 *
                 * @param settingsManager The responsible manager.
                 */
                GurobiSettings(storm::settings::SettingsManager& settingsManager);
                
                /*!
                 * Retrieves the integer tolerance to be used.
                 *
                 * @return The integer tolerance to be used.
                 */
                double getIntegerTolerance() const;
                
                /*!
                 * Retrieves the maximal number of threads Gurobi is allowed to use.
                 *
                 * @return The maximally allowed number of threads.
                 */
                uint_fast64_t getNumberOfThreads() const;
                
                /*!
                 * Retrieves whether the output option was set.
                 *
                 * @return True iff the output option was set.
                 */
                bool isOutputSet() const;
                
                // The name of the module.
                static const std::string moduleName;
                
            private:
                // Define the string names of the options as constants.
                static const std::string integerToleranceOption;
                static const std::string threadsOption;
                static const std::string outputOption;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_GUROBISETTINGS_H_ */
