#include "GSPNExportSettings.h"
#include "JaniExportSettings.h"

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
            const std::string GSPNExportSettings::moduleName = "exportGspn";
            
            const std::string GSPNExportSettings::writeToDotOptionName = "gspn-dot-output";
            
            const std::string GSPNExportSettings::writeToPnmlOptionName = "to-pnml";
            const std::string GSPNExportSettings::writeToPnproOptionName = "to-pnpro";
            
            //const std::string GSPNExportSettings::janiFileOptionShortName = "dotoutput";
            
            
            GSPNExportSettings::GSPNExportSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, writeToDotOptionName, false, "Destination for the dot output.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, writeToPnmlOptionName, false, "Destination for the pnml output").addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, writeToPnproOptionName, false, "Destination for the pnpro output").addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").build()).build());
            }
            
            bool GSPNExportSettings::isWriteToDotSet() const {
                return this->getOption(writeToDotOptionName).getHasOptionBeenSet();
            }
            
            std::string GSPNExportSettings::getWriteToDotFilename() const {
                return this->getOption(writeToDotOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool GSPNExportSettings::isWriteToPnmlSet() const {
                return this->getOption(writeToPnmlOptionName).getHasOptionBeenSet();
            }
            
            std::string GSPNExportSettings::getWriteToPnmlFilename() const {
                return this->getOption(writeToPnmlOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool GSPNExportSettings::isWriteToPnproSet() const {
                return this->getOption(writeToPnproOptionName).getHasOptionBeenSet();
            }
            
            std::string GSPNExportSettings::getWriteToPnproFilename() const {
                return this->getOption(writeToPnproOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            
            void GSPNExportSettings::finalize() {
                
            }
            
            bool GSPNExportSettings::check() const {
                return true;
            }
        }
    }
}
