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
                
                bool isAllowEdgeAssignmentsSet() const;

                bool isExportFlattenedSet() const;

                bool isLocationVariablesSet() const;
                
                bool isGlobalVarsSet() const;
                
                bool isLocalVarsSet() const;
                
                bool isCompactJsonSet() const;
                
                bool isEliminateArraysSet() const;
                
                bool isEliminateFunctionsSet() const;
                
                bool isReplaceUnassignedVariablesWithConstantsSet() const;
                
                bool isSimplifyCompositionSet() const;

                std::vector<std::pair<std::string, std::string>> getLocationVariables() const;

                bool check() const override;
                void finalize() override;
                
                static const std::string moduleName;
                
            private:
                static const std::string edgeAssignmentsOptionName;
                static const std::string exportFlattenOptionName;
                static const std::string locationVariablesOptionName;
                static const std::string globalVariablesOptionName;
                static const std::string localVariablesOptionName;
                static const std::string compactJsonOptionName;
                static const std::string eliminateArraysOptionName;
                static const std::string eliminateFunctionsOptionName;
                static const std::string replaceUnassignedVariablesWithConstantsOptionName;
                static const std::string simplifyCompositionOptionName;
                
            };
        }
    }
}
