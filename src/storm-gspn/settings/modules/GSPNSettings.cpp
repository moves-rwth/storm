#include "storm-gspn/settings/modules/GSPNSettings.h"

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
            const std::string GSPNSettings::moduleName = "gspn";
            
            const std::string GSPNSettings::gspnFileOptionName = "gspnfile";
            const std::string GSPNSettings::gspnFileOptionShortName = "gspn";
            const std::string GSPNSettings::gspnToJaniOptionName = "to-jani";
            const std::string GSPNSettings::gspnToJaniOptionShortName = "tj";
            const std::string GSPNSettings::capacitiesFileOptionName = "capacitiesfile";
            const std::string GSPNSettings::capacitiesFileOptionShortName = "capacities";
            
            
            GSPNSettings::GSPNSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, gspnFileOptionName, false, "Parses the GSPN.").setShortName(gspnFileOptionShortName).addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").addValidatorString(ArgumentValidatorFactory::createExistingFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, gspnToJaniOptionName, false, "Transform to JANI.").setShortName(gspnToJaniOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, capacitiesFileOptionName, false, "Capacaties as invariants for places.").setShortName(capacitiesFileOptionShortName).addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").addValidatorString(ArgumentValidatorFactory::createExistingFileValidator()).build()).build());
            }
            
            bool GSPNSettings::isGspnFileSet() const {
                return this->getOption(gspnFileOptionName).getHasOptionBeenSet();
            }
            
            std::string GSPNSettings::getGspnFilename() const {
                return this->getOption(gspnFileOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool GSPNSettings::isCapacitiesFileSet() const {
                return this->getOption(capacitiesFileOptionName).getHasOptionBeenSet();
            }
            
            std::string GSPNSettings::getCapacitiesFilename() const {
                return this->getOption(capacitiesFileOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            void GSPNSettings::finalize() {
                
            }
            
            bool GSPNSettings::check() const {
                if(!isGspnFileSet()) {
                    if(isCapacitiesFileSet()) {
                        return false;
                    }
                }
                return true;
            }
        }
    }
}
