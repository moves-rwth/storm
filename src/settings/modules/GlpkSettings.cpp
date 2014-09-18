#include "src/settings/modules/GlpkSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            GlpkSettings::GlpkSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager) {
                settingsManager.addOption(storm::settings::OptionBuilder("GlpkLpSolver", "glpkoutput", "", "If set, the glpk output will be printed to the command line.").build());
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm