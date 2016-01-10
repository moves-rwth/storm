#ifndef STORM_SETTINGS_MODULES_SYLVANSETTINGS_H_
#define STORM_SETTINGS_MODULES_SYLVANSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for Sylvan.
             */
            class SylvanSettings : public ModuleSettings {
            public:
                /*!
                 * Creates a new set of CUDD settings that is managed by the given manager.
                 *
                 * @param settingsManager The responsible manager.
                 */
                SylvanSettings(storm::settings::SettingsManager& settingsManager);
                
                /*!
                 * Retrieves the maximal amount of memory (in megabytes) that Sylvan can occupy.
                 *
                 * @return The maximal amount of memory to use.
                 */
                uint_fast64_t getMaximalMemory() const;
                
                /*!
                 * Retrieves the amount of threads available to Sylvan. Note that a value of zero means that the number
                 * of threads is auto-detected to fit the current machine.
                 *
                 * @rreturn The number of threads.
                 */
                uint_fast64_t getNumberOfThreads() const;
                
                // The name of the module.
                static const std::string moduleName;
                
            private:
                // Define the string names of the options as constants.
                static const std::string maximalMemoryOptionName;
                static const std::string threadCountOptionName;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_SYLVANSETTINGS_H_ */
