#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the resource settings.
             */
            class ResourceSettings : public ModuleSettings {
            public:
                
                /*!
                 * Creates a new set of general settings.
                 */
                ResourceSettings();

                /*!
                 * Retrieves whether the timeout option was set.
                 *
                 * @return True if the timeout option was set.
                 */
                bool isTimeoutSet() const;
                
                /*!
                 * Retrieves the time after which the computation has to be aborted in case the timeout option was set.
                 *
                 * @return The number of seconds after which to timeout.
                 */
                uint_fast64_t getTimeoutInSeconds() const;

                // The name of the module.
                static const std::string moduleName;

            private:
                // Define the string names of the options as constants.
                static const std::string timeoutOptionName;
                static const std::string timeoutOptionShortName;
            };
        }
    }
}
