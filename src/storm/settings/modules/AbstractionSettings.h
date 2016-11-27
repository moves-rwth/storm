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

                /*!
                 * Retrieves whether the option to split the initially added guards to atoms was set.
                 *
                 * @return True iff the option was set.
                 */
                bool isSplitInitialGuardsSet() const;

                /*!
                 * Retrieves whether the option to split guards derived later to atoms was set.
                 *
                 * @return True iff the option was set.
                 */
                bool isSplitGuardsSet() const;

                const static std::string moduleName;
                
            private:
                const static std::string addAllGuardsOptionName;
                const static std::string splitPredicatesOptionName;
                const static std::string splitInitialGuardsOptionName;
                const static std::string splitGuardsOptionName;
            };
            
        }
    }
}
