#ifndef STORM_SETTINGS_MODULES_LEARNINGSETTINGS_H_
#define STORM_SETTINGS_MODULES_LEARNINGSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the learning settings.
             */
            class LearningSettings : public ModuleSettings {
            public:
                // An enumeration of all available EC-detection types.
                enum class ECDetectionType { Local, Global };
                
                /*!
                 * Creates a new set of learning settings that is managed by the given manager.
                 *
                 * @param settingsManager The responsible manager.
                 */
                LearningSettings(storm::settings::SettingsManager& settingsManager);
                
                /*!
                 * Retrieves whether local EC-detection is to be used.
                 *
                 * @return True iff local EC-detection is to be used.
                 */
                bool isLocalECDetectionSet() const;
                
                /*!
                 * Retrieves whether global EC-detection is to be used.
                 *
                 * @return True iff global EC-detection is to be used.
                 */
                bool isGlobalECDetectionSet() const;
                
                /*!
                 * Retrieves the selected EC-detection type.
                 *
                 * @return The selected EC-detection type.
                 */
                ECDetectionType getECDetectionType() const;
                
                virtual bool check() const override;
                
                // The name of the module.
                static const std::string moduleName;
                
            private:
                // Define the string names of the options as constants.
                static const std::string ecDetectionTypeOptionName;
            };
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_LEARNINGSETTINGS_H_ */
