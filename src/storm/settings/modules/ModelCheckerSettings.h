#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"

#include "storm/builder/ExplorationOrder.h"

namespace storm {
    namespace settings {
        namespace modules {

            /*!
             * This class represents the general settings.
             */
            class ModelCheckerSettings : public ModuleSettings {
            public:

                /*!
                 * Creates a new set of general settings.
                 */
                ModelCheckerSettings();
                
                bool isFilterRewZeroSet() const;

                // The name of the module.
                static const std::string moduleName;

            private:
                // Define the string names of the options as constants.
                static const std::string filterRewZeroOptionName;
            };

        } // namespace modules
    } // namespace settings
} // namespace storm

