#pragma once

#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for the abstraction procedures.
             */
            class AbstractionSettings : public ModuleSettings {
            public:
                /*!
                 * Creates a new set of abstraction settings.
                 */
                AbstractionSettings();
                
                /*!
                 * Retrieves whether the option to add all guards was set.
                 *
                 * @return True iff the option was set.
                 */
                bool isAddAllGuardsSet() const;

                /*!
                 * Retrieves whether the option to split predicates to atoms was set.
                 *
                 * @return True iff the option was set.
                 */
                bool isSplitPredicatesSet() const;
                
                const static std::string moduleName;
                
            private:
                const static std::string addAllGuardsOptionName;
                const static std::string splitPredicatesOptionName;
            };
            
        }
    }
}
