#ifndef STORM_SETTINGS_MODULES_BISIMULATIONSETTINGS_H_
#define STORM_SETTINGS_MODULES_BISIMULATIONSETTINGS_H_

#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the bisimulation settings.
             */
            class BisimulationSettings : public ModuleSettings {
            public:
                // An enumeration of all available bisimulation types.
                enum class BisimulationType { Strong, Weak };
                
                enum class CachingStrategy { FullDirect, FullLate, Granularity, Minimal };
                
                /*!
                 * Creates a new set of bisimulation settings.
                 */
                BisimulationSettings();
                
                /*!
                 * Retrieves whether strong bisimulation is to be used.
                 *
                 * @return True iff strong bisimulation is to be used.
                 */
                bool isStrongBisimulationSet() const;

                /*!
                 * Retrieves whether weak bisimulation is to be used.
                 *
                 * @return True iff weak bisimulation is to be used.
                 */
                bool isWeakBisimulationSet() const;

                virtual bool check() const override;
                
                // The name of the module.
                static const std::string moduleName;
                
            private:
                // Define the string names of the options as constants.
                static const std::string typeOptionName;
            };
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_BISIMULATIONSETTINGS_H_ */
