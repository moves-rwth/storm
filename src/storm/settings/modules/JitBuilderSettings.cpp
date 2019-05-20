#include "storm/settings/modules/JitBuilderSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

#include "storm/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace settings {
        namespace modules {
            const std::string JitBuilderSettings::moduleName = "jitbuilder";
            
            const std::string JitBuilderSettings::doctorOptionName = "doctor";
            const std::string JitBuilderSettings::compilerOptionName = "compiler";
            const std::string JitBuilderSettings::stormIncludeDirectoryOptionName = "storm";
            const std::string JitBuilderSettings::boostIncludeDirectoryOptionName = "boost";
            const std::string JitBuilderSettings::carlIncludeDirectoryOptionName = "carl";
            const std::string JitBuilderSettings::compilerFlagsOptionName = "cxxflags";
            const std::string JitBuilderSettings::optimizationLevelOptionName = "opt";

            JitBuilderSettings::JitBuilderSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, doctorOptionName, false, "Show debugging information on why the jit-based model builder is not working on your system.").setIsAdvanced().build());
                this->addOption(storm::settings::OptionBuilder(moduleName, compilerOptionName, false, "The compiler in the jit-based model builder.").setIsAdvanced()
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the executable. Defaults to c++.").setDefaultValueString("c++").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, stormIncludeDirectoryOptionName, false, "The include directory of storm.").setIsAdvanced()
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("dir", "The directory that contains the headers of storm.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, boostIncludeDirectoryOptionName, false, "The include directory of boost.").setIsAdvanced()
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("dir", "The directory containing the boost headers version >= 1.61.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, carlIncludeDirectoryOptionName, false, "The include directory of carl.").setIsAdvanced()
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("dir", "The directory containing the carl headers.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, compilerFlagsOptionName, false, "The flags passed to the compiler.").setIsAdvanced()
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("flags", "The compiler flags.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, optimizationLevelOptionName, false, "Sets the optimization level.").setIsAdvanced()
                                .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("level", "The level to use.").setDefaultValueUnsignedInteger(3).build()).build());
            }
            
            bool JitBuilderSettings::isCompilerSet() const {
                return this->getOption(compilerOptionName).getHasOptionBeenSet();
            }
            
            std::string JitBuilderSettings::getCompiler() const {
                return this->getOption(compilerOptionName).getArgumentByName("name").getValueAsString();
            }
            
            bool JitBuilderSettings::isStormIncludeDirectorySet() const {
                return this->getOption(stormIncludeDirectoryOptionName).getHasOptionBeenSet();
            }
            
            std::string JitBuilderSettings::getStormIncludeDirectory() const {
                return this->getOption(stormIncludeDirectoryOptionName).getArgumentByName("dir").getValueAsString();
            }
            
            bool JitBuilderSettings::isBoostIncludeDirectorySet() const {
                return this->getOption(boostIncludeDirectoryOptionName).getHasOptionBeenSet();
            }
            
            std::string JitBuilderSettings::getBoostIncludeDirectory() const {
                return this->getOption(boostIncludeDirectoryOptionName).getArgumentByName("dir").getValueAsString();
            }

            bool JitBuilderSettings::isCarlIncludeDirectorySet() const {
                return this->getOption(carlIncludeDirectoryOptionName).getHasOptionBeenSet();
            }
            
            std::string JitBuilderSettings::getCarlIncludeDirectory() const {
                return this->getOption(carlIncludeDirectoryOptionName).getArgumentByName("dir").getValueAsString();
            }

            bool JitBuilderSettings::isDoctorSet() const {
                return this->getOption(doctorOptionName).getHasOptionBeenSet();
            }
            
            bool JitBuilderSettings::isCompilerFlagsSet() const {
                return this->getOption(compilerFlagsOptionName).getHasOptionBeenSet();
            }
            
            std::string JitBuilderSettings::getCompilerFlags() const {
                return this->getOption(compilerFlagsOptionName).getArgumentByName("flags").getValueAsString();
            }
            
            uint64_t JitBuilderSettings::getOptimizationLevel() const {
                return this->getOption(optimizationLevelOptionName).getArgumentByName("level").getValueAsUnsignedInteger();
            }
            
            void JitBuilderSettings::finalize() {
                // Intentionally left empty.
            }
            
            bool JitBuilderSettings::check() const {
                return true;
            }
        }
    }
}
