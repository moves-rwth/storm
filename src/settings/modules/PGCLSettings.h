#pragma once

#include "storm-config.h"
#include "src/settings/modules/ModuleSettings.h"


namespace storm {
    namespace settings {
        namespace modules {
            class PGCLSettings : public ModuleSettings {
            public:
                /*!
                 * Creates a new PGCL setting
                 */
                PGCLSettings();
                
                /**
                 * Retrievew whether the pgcl file option was set
                 */
                bool isPgclFileSet() const;
                
                /**
                 * Retrieves the pgcl file name
                 */
                std::string getPgclFilename() const;
                
                /**
                 *
                 */
                bool isToJaniSet() const;
                
                bool check() const override;
                void finalize() override;
                
                static const std::string moduleName;
                
            private:
                static const std::string pgclFileOptionName;
                static const std::string pgclFileOptionShortName;
                static const std::string pgclToJaniOptionName;
                static const std::string pgclToJaniOptionShortName;
                
            };
        }
    }
}