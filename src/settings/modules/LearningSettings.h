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
                // An enumeration of all available precomputation types.
                enum class PrecomputationType { Local, Global };
                
                /*!
                 * Creates a new set of learning settings that is managed by the given manager.
                 *
                 * @param settingsManager The responsible manager.
                 */
                LearningSettings(storm::settings::SettingsManager& settingsManager);
                
                /*!
                 * Retrieves whether local precomputation is to be used.
                 *
                 * @return True iff local precomputation is to be used.
                 */
                bool isLocalPrecomputationSet() const;
                
                /*!
                 * Retrieves whether global precomputation is to be used.
                 *
                 * @return True iff global precomputation is to be used.
                 */
                bool isGlobalPrecomputationSet() const;
                
                /*!
                 * Retrieves the selected precomputation type.
                 *
                 * @return The selected precomputation type.
                 */
                PrecomputationType getPrecomputationType() const;
                
                /*!
                 * Retrieves the number of exploration steps to perform until a precomputation is triggered.
                 *
                 * @return The number of exploration steps to perform until a precomputation is triggered.
                 */
                uint_fast64_t getNumberOfExplorationStepsUntilPrecomputation() const;
                
                virtual bool check() const override;
                
                // The name of the module.
                static const std::string moduleName;
                
            private:
                // Define the string names of the options as constants.
                static const std::string precomputationTypeOptionName;
                static const std::string numberOfExplorationStepsUntilPrecomputationOptionName;
            };
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_LEARNINGSETTINGS_H_ */
