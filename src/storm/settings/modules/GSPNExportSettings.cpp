#include "GSPNExportSettings.h"
#include "JaniExportSettings.h"

#include "src/storm/settings/SettingsManager.h"
#include "src/storm/settings/SettingMemento.h"
#include "src/storm/settings/Option.h"
#include "src/storm/settings/OptionBuilder.h"
#include "src/storm/settings/ArgumentBuilder.h"
#include "src/storm/settings/Argument.h"

#include "src/storm/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace settings {
        namespace modules {
            const std::string GSPNExportSettings::moduleName = "exportGspn";
            
            const std::string GSPNExportSettings::writeToDotOptionName = "gspn-dot-output";
            //const std::string GSPNExportSettings::janiFileOptionShortName = "dotoutput";
            
            
            GSPNExportSettings::GSPNExportSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, writeToDotOptionName, false, "Destination for the dot output.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").build()).build());
            }
            
            bool GSPNExportSettings::isWriteToDotSet() const {
                return this->getOption(writeToDotOptionName).getHasOptionBeenSet();
            }
            
            std::string GSPNExportSettings::getWriteToDotFilename() const {
                return this->getOption(writeToDotOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            void GSPNExportSettings::finalize() {
                
            }
            
            bool GSPNExportSettings::check() const {
                return true;
            }
        }
    }
}
