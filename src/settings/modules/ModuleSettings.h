#ifndef STORM_SETTINGS_MODULES_MODULESETTINGS_H_
#define STORM_SETTINGS_MODULES_MODULESETTINGS_H_

#include <string>
#include <unordered_map>

#include "src/settings/Option.h"
#include "src/settings/SettingMemento.h"

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
                virtual bool check() const;
                
                /*!
                 * Sets the option with the given name to the required status. This requires the option to take no
                 * arguments. As a result, a pointer to an object is returned such that when the object is destroyed
                 * (i.e. the smart pointer goes out of scope), the option is reset to its original status.
                 *
                 * @param name The name of the option to (unset).
                 * @param requiredStatus The status that is to be set for the option.
                 * @return A pointer to an object that resets the change upon destruction.
                 */
                std::unique_ptr<storm::settings::SettingMemento> overrideOption(std::string const& name, bool requiredStatus);
                
            protected:
                /*!
                 * Retrieves the manager responsible for the settings.
                 *
                 * @return The manager responsible for the settings.
                 */
                storm::settings::SettingsManager const& getSettingsManager() const;
                
                /*!
                 * Adds the option with the given long name to the list of options of this module.
                 *
                 * @param longName The long name of the option.
                 * @param option The actual option associated with the given name.
                 */
                void addOption(std::string const& longName, std::shared_ptr<Option> const& option);
                
                /*!
                 * Retrieves the option with the given long name. If no such option exists, an exception is thrown.
                 *
                 * @param longName The long name of the option to retrieve.
                 * @return The option associated with the given option name.
                 */
                Option& getOption(std::string const& longName);
                
                /*!
                 * Retrieves whether the option with the given name was set.
                 *
                 * @param The name of the option.
                 * @return True iff the option was set.
                 */
                bool isSet(std::string const& optionName) const;
                
                /*!
                 * Sets the option with the specified name. This requires the option to not have any arguments. This
                 * should be used with care and is primarily meant to be used by the SettingMemento.
                 *
                 * @param name The name of the option to set.
                 */
                void set(std::string const& name) const;
                
                /*!
                 * Unsets the option with the specified name. This requires the option to not have any arguments. This
                 * should be used with care and is primarily meant to be used by the SettingMemento.
                 *
                 * @param name The name of the option to unset.
                 */
                void unset(std::string const& longName) const;
                
            private:
                // The settings manager responsible for the settings.
                storm::settings::SettingsManager const& settingsManager;
                
                // A mapping of option names of the module to the actual options.
                std::unordered_map<std::string, std::shared_ptr<Option>> options;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_MODULESETTINGS_H_ */