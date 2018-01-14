#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"


namespace storm {
    namespace settings {
        namespace modules {
            class GSPNSettings : public ModuleSettings {
            public:
                /*!
                 * Creates a new PGCL setting
                 */
                GSPNSettings();
                
                /**
                 * Retrievew whether the pgcl file option was set
                 */
                bool isGspnFileSet() const;
                
                /**
                 * Retrieves the gspn file name
                 */
                std::string getGspnFilename() const;
                
                /**
                 * Whether the gspn should be transformed to Jani
                 */
                bool isToJaniSet() const;
                
                /**
                 * Retrievew whether the pgcl file option was set
                 */
                bool isCapacitiesFileSet() const;
                
                /**
                 * Retrieves the gspn file name
                 */
                std::string getCapacitiesFilename() const;
                
                
                bool check() const override;
                void finalize() override;
                
                static const std::string moduleName;
                
            private:
                static const std::string gspnFileOptionName;
                static const std::string gspnFileOptionShortName;
                static const std::string gspnToJaniOptionName;
                static const std::string gspnToJaniOptionShortName;
                static const std::string capacitiesFileOptionName;
                static const std::string capacitiesFileOptionShortName;
                
            };
        }
    }
}
