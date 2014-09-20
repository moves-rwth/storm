#ifndef STORM_SETTINGS_MODULES_COUNTEREXAMPLEGENERATORSETTINGS_H_
#define STORM_SETTINGS_MODULES_COUNTEREXAMPLEGENERATORSETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for counterexample generation.
             */
            class CounterexampleGeneratorSettings : public ModuleSettings {
            public:
                /*!
                 * Creates a new set of counterexample settings that is managed by the given manager.
                 *
                 * @param settingsManager The responsible manager.
                 */
                CounterexampleGeneratorSettings(storm::settings::SettingsManager& settingsManager);
                
                /*!
                 * Retrieves whether the option to generate a minimal command set was set.
                 *
                 * @return True iff a minimal command set counterexample is to be generated.
                 */
                bool isMinimalCommandGenerationSet() const;
                
                /*!
                 * Retrieves the name of the file that contains the properties for which a minimal command set
                 * counterexample is to be generated if the option to generate such a counterexample was set.
                 *
                 * @return The name of the file that contains the properties.
                 */
                std::string minimalCommandSetPropertyFilename() const;
                
                /*!
                 * Retrieves whether the MILP-based technique is to be used to generate a minimal command set
                 * counterexample.
                 *
                 * @return True iff the MILP-based technique is to be used.
                 */
                bool useMilpBasedMinimalCommandSetGeneration() const;

                /*!
                 * Retrieves whether the MAXSAT-based technique is to be used to generate a minimal command set
                 * counterexample.
                 *
                 * @return True iff the MAXSAT-based technique is to be used.
                 */
                bool useMaxSatBasedMinimalCommandSetGeneration() const;
                
                /*!
                 * Retrieves whether reachability of a target state is to be encoded if the MAXSAT-based technique is
                 * used to generate a minimal command set counterexample.
                 *
                 * @return True iff reachability of a target state is to be encoded.
                 */
                bool isEncodeReachabilitySet() const;
                
                /*!
                 * Retrieves whether scheduler cuts are to be used if the MAXSAT-based technique is used to generate a
                 * minimal command set counterexample
                 */
                bool useSchedulerCuts() const;
                
                /*!
                 * Retrieves whether statistics are to be shown for counterexample generation.
                 *
                 * @return True iff statistics are to be shown for counterexample generation.
                 */
                bool showStatistics() const;
                
            private:
                // Define the string names of the options as constants.
                static const std::string moduleName;
                static const std::string minimalCommandSetOptionName;
                static const std::string encodeReachabilityOptionName;
                static const std::string schedulerCutsOptionName;
                static const std::string statisticsOptionName;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_COUNTEREXAMPLEGENERATORSETTINGS_H_ */
