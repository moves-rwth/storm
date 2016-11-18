#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"


namespace storm {
    namespace settings {
        namespace modules {
            class GSPNExportSettings : public ModuleSettings {
            public:
                /*!
                 * Creates a new JaniExport setting
                 */
                GSPNExportSettings();
                
                /**
                 * Retrievew whether the pgcl file option was set
                 */
                bool isWriteToDotSet() const;
                
                /**
                 * Retrieves the pgcl file name
                 */
                std::string getWriteToDotFilename() const;
                
                bool isWriteToPnmlSet() const;
                
                /**
                 *
                 */
                std::string getWriteToPnmlFilename() const;
                
                bool isWriteToPnproSet() const;
                
                /**
                 *
                 */
                std::string getWriteToPnproFilename() const;
                
                
                bool check() const override;
                void finalize() override;
                
                static const std::string moduleName;
                
            private:
                static const std::string writeToDotOptionName;
                static const std::string writeToPnmlOptionName;
                static const std::string writeToPnproOptionName;
                //static const std::string writeToDotOptionShortName;
                
            };
        }
    }
}
