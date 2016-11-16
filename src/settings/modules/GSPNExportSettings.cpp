#include "GSPNExportSettings.h"
#include "JaniExportSettings.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/SettingMemento.h"
#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/Argument.h"

#include "src/exceptions/InvalidSettingsException.h"

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