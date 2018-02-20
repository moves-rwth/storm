#pragma once

#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for Sylvan.
             */
            class MarkovAutomatonSettings : public ModuleSettings {
            public:
                enum class BoundedReachabilityTechnique { Imca, UnifPlus };
                
                /*!
                 * Creates a new set of Markov automaton settings.
                 */
                MarkovAutomatonSettings();
                
                /*!
                 * Retrieves the technique to use to solve bounded reachability properties.
                 *
                 * @return The selected technique.
                 */
                BoundedReachabilityTechnique getTechnique() const;
                
                // The name of the module.
                static const std::string moduleName;
                
            private:
                // Define the string names of the options as constants.
                static const std::string techniqueOptionName;
            };
            
        }
    }
}
