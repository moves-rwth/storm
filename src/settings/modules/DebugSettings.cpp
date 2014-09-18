#include "src/settings/modules/DebugSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            DebugSettings::DebugSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager) {
                settingsManager.addOption(storm::settings::OptionBuilder("debug", "debug", "", "Be very verbose (intended for debugging).").build());
                settingsManager.addOption(storm::settings::OptionBuilder("debug", "trace", "", "Be extremly verbose (intended for debugging, heavy performance impacts).").build());
                settingsManager.addOption(storm::settings::OptionBuilder("debug", "logfile", "l", "If specified, the log output will also be written to this file.")
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("logFileName", "The path and name of the file to write to.").build()).build());
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm