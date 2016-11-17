#pragma once

#include "src/storm/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            class JitBuilderSettings : public ModuleSettings {
            public:
                /*!
                 * Creates a new JitBuilder settings object.
                 */
                JitBuilderSettings();

                bool isDoctorSet() const;
                
                bool isCompilerSet() const;
                std::string getCompiler() const;
                
                bool isStormRootSet() const;
                std::string getStormRoot() const;

                bool isStormConfigDirectorySet() const;
                std::string getStormConfigDirectory() const;
                
                bool isBoostIncludeDirectorySet() const;
                std::string getBoostIncludeDirectory() const;

                bool isCarlIncludeDirectorySet() const;
                std::string getCarlIncludeDirectory() const;

                bool isCompilerFlagsSet() const;
                std::string getCompilerFlags() const;
                
                bool check() const override;
                void finalize() override;
                
                static const std::string moduleName;
                
            private:
                static const std::string compilerOptionName;
                static const std::string stormRootOptionName;
                static const std::string stormConfigDirectoryOptionName;
                static const std::string boostIncludeDirectoryOptionName;
                static const std::string carlIncludeDirectoryOptionName;
                static const std::string compilerFlagsOptionName;
                static const std::string doctorOptionName;
            };
            
        }
    }
}
