#include "storm-gspn/settings/modules/GSPNExportSettings.h"
#include "storm/settings/modules/JaniExportSettings.h"

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
            const std::string GSPNExportSettings::writeToJsonOptionName = "to-json";
            const std::string GSPNExportSettings::writeStatsOptionName = "to-stats";
            const std::string GSPNExportSettings::displayStatsOptionName = "show-stats";
            
            //const std::string GSPNExportSettings::janiFileOptionShortName = "dotoutput";
            
            
            GSPNExportSettings::GSPNExportSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, writeToDotOptionName, false, "Destination for the dot output.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, writeToPnmlOptionName, false, "Destination for the pnml output").addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, writeToPnproOptionName, false, "Destination for the pnpro output").addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, writeToJsonOptionName, false, "Destination for the json output").addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, writeStatsOptionName, false, "Destination for the stats file").addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, displayStatsOptionName, false, "Print stats to stdout").build());
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

            bool GSPNExportSettings::isWriteToJsonSet() const {
                return this->getOption(writeToJsonOptionName).getHasOptionBeenSet();
            }

            std::string GSPNExportSettings::getWriteToJsonFilename() const {
                return this->getOption(writeToJsonOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool GSPNExportSettings::isDisplayStatsSet() const {
                return this->getOption(displayStatsOptionName).getHasOptionBeenSet();
            }
            
            bool GSPNExportSettings::isWriteStatsToFileSet() const {
                return this->getOption(writeStatsOptionName).getHasOptionBeenSet();
            }
            
            std::string GSPNExportSettings::getWriteStatsFilename() const {
                return this->getOption(writeStatsOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            void GSPNExportSettings::finalize() {
                
            }
            
            bool GSPNExportSettings::check() const {
                return true;
            }
        }
    }
}
