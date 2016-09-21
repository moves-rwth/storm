#pragma once

#include "storm-config.h"
#include "src/settings/modules/ModuleSettings.h"


namespace storm {
    namespace settings {
        namespace modules {
            class JaniExportSettings : public ModuleSettings {
            public:
                /*!
                 * Creates a new JaniExport setting
                 */
                JaniExportSettings();
                
                /**
                 * Retrievew whether the pgcl file option was set
                 */
                bool isJaniFileSet() const;
                
                /**
                 * Retrieves the pgcl file name
                 */
                std::string getJaniFilename() const;
                
                bool check() const override;
                void finalize() override;
                
                static const std::string moduleName;
                
            private:
                static const std::string janiFileOptionName;
                static const std::string janiFileOptionShortName;
            };
        }
    }
}