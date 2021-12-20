#pragma once

#include "storm/settings/modules/ModuleSettings.h"

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

    bool isStormIncludeDirectorySet() const;
    std::string getStormIncludeDirectory() const;

    bool isBoostIncludeDirectorySet() const;
    std::string getBoostIncludeDirectory() const;

    bool isCarlIncludeDirectorySet() const;
    std::string getCarlIncludeDirectory() const;

    bool isCompilerFlagsSet() const;
    std::string getCompilerFlags() const;

    uint64_t getOptimizationLevel() const;

    bool check() const override;
    void finalize() override;

    static const std::string moduleName;

   private:
    static const std::string compilerOptionName;
    static const std::string stormIncludeDirectoryOptionName;
    static const std::string boostIncludeDirectoryOptionName;
    static const std::string carlIncludeDirectoryOptionName;
    static const std::string compilerFlagsOptionName;
    static const std::string doctorOptionName;
    static const std::string optimizationLevelOptionName;
};

}  // namespace modules
}  // namespace settings
}  // namespace storm
