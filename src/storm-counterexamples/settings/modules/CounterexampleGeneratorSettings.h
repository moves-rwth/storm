#ifndef STORM_SETTINGS_MODULES_COUNTEREXAMPLEGENERATORSETTINGS_H_
#define STORM_SETTINGS_MODULES_COUNTEREXAMPLEGENERATORSETTINGS_H_

#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for counterexample generation.
             */
            class CounterexampleGeneratorSettings : public ModuleSettings {
            public:
                /*!
                 * Creates a new set of counterexample settings.
                 */
                CounterexampleGeneratorSettings();
                
                /*!
                 * Retrieves whether the option to generate a minimal command set was set.
                 *
                 * @return True iff a minimal command set counterexample is to be generated.
                 */
                bool isMinimalCommandSetGenerationSet() const;
                
                /*!
                 * Retrieves whether the MILP-based technique is to be used to generate a minimal command set
                 * counterexample.
                 *
                 * @return True iff the MILP-based technique is to be used.
                 */
                bool isUseMilpBasedMinimalCommandSetGenerationSet() const;

                /*!
                 * Retrieves whether the MAXSAT-based technique is to be used to generate a minimal command set
                 * counterexample.
                 *
                 * @return True iff the MAXSAT-based technique is to be used.
                 */
                bool isUseMaxSatBasedMinimalCommandSetGenerationSet() const;
                
                /*!
                 * Retrieves whether reachability of a target state is to be encoded if the MAXSAT-based technique is
                 * used to generate a minimal command set counterexample.
                 *
                 * @return True iff reachability of a target state is to be encoded.
                 */
                bool isEncodeReachabilitySet() const;
                
                /*!
                 * Retrieves whether scheduler cuts are to be used if the MAXSAT-based technique is used to generate a
                 * minimal command set counterexample.
                 *
                 * @return True iff scheduler cuts are to be used.
                 */
                bool isUseSchedulerCutsSet() const;
                
                /*!
                 * Retrieves whether to use the dynamic constraints in the MAXSAT-based technique.
                 *
                 * @return True iff dynamic constraints are to be used.
                 */
                bool isUseDynamicConstraintsSet() const;
                
                bool check() const override;
                
                // The name of the module.
                static const std::string moduleName;
                
            private:
                // Define the string names of the options as constants.
                static const std::string minimalCommandSetOptionName;
                static const std::string encodeReachabilityOptionName;
                static const std::string schedulerCutsOptionName;
                static const std::string noDynamicConstraintsOptionName;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_COUNTEREXAMPLEGENERATORSETTINGS_H_ */
