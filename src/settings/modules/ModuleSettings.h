#ifndef STORM_SETTINGS_MODULES_MODULESETTINGS_H_
#define STORM_SETTINGS_MODULES_MODULESETTINGS_H_

#include <string>

namespace storm {
    namespace settings {
        // Forward-declare SettingsManager class.
        class SettingsManager;
        
        namespace modules {
            
            /*!
             * This is the base class of the settings for a particular module.
             */
            class ModuleSettings {
            public:
                /*!
                 * Constructs a new settings object.
                 *
                 * @param settingsManager The manager responsible for these settings.
                 */
                ModuleSettings(storm::settings::SettingsManager& settingsManager);
                
                /*!
                 * Checks whether the settings are consistent. If they are inconsistent, an exception is thrown.
                 *
                 * @return True if the settings are consistent.
                 */
                virtual bool check() const = 0;
                
            protected:
                /*!
                 * Retrieves the manager responsible for the settings.
                 *
                 * @return The manager responsible for the settings.
                 */
                storm::settings::SettingsManager const& getSettingsManager() const;
                
                /*!
                 * Retrieves whether the option with the given name was set.
                 */
                bool isSet(std::string const& optionName) const;
                
            private:
                // The settings manager responsible for the settings.
                storm::settings::SettingsManager const& settingsManager;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_MODULESETTINGS_H_ */