#include "src/settings/modules/DebugSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string DebugSettings::moduleName = "debug";
            const std::string DebugSettings::debugOptionName = "debug";
            const std::string DebugSettings::traceOptionName = "trace";
            const std::string DebugSettings::logfileOptionName = "logfile";
            const std::string DebugSettings::logfileOptionShortName = "l";

            DebugSettings::DebugSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, debugOptionName, false, "Print debug output.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, traceOptionName, false, "Print even more debug output.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, logfileOptionName, false, "If specified, the log output will also be written to this file.").setShortName(logfileOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file to write the log.").build()).build());
            }
            
            bool DebugSettings::isDebugSet() const {
                return this->getOption(debugOptionName).getHasOptionBeenSet();
            }
            
            bool DebugSettings::isTraceSet() const {
                return this->getOption(traceOptionName).getHasOptionBeenSet();
            }
            
            bool DebugSettings::isLogfileSet() const {
                return this->getOption(logfileOptionName).getHasOptionBeenSet();
            }
            
            std::string DebugSettings::getLogfilename() const {
                return this->getOption(traceOptionName).getArgumentByName("filename").getValueAsString();
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm