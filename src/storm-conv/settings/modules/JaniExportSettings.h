#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"


namespace storm {
    namespace settings {
        namespace modules {
            class JaniExportSettings : public ModuleSettings {
            public:
                /*!
                 * Creates a new JaniExport setting
                 */
                JaniExportSettings();
                
                bool isExportAsStandardJaniSet() const;

                bool isExportFlattenedSet() const;

                bool isLocationVariablesSet() const;
                
                bool isGlobalVarsSet() const;
                
                bool isCompactJsonSet() const;

                std::vector<std::pair<std::string, std::string>> getLocationVariables() const;

                bool check() const override;
                void finalize() override;
                
                static const std::string moduleName;
                
            private:
                static const std::string standardCompliantOptionName;
                static const std::string standardCompliantOptionShortName;
                static const std::string exportFlattenOptionName;
                static const std::string locationVariablesOptionName;
                static const std::string globalVariablesOptionName;
                static const std::string compactJsonOptionName;
                
            };
        }
    }
}
