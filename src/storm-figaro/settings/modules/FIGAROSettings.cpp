#include "FIGAROSettings.h"

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
            const std::string FIGAROSettings::moduleName = "figaro";
            
            const std::string FIGAROSettings::figaroFileOptionName = "figarofile";
            const std::string FIGAROSettings::figaroFileOptionShortame = "fi";
            const std::string FIGAROSettings::xmlFileOptionName = "xml";
            const std::string FIGAROSettings::xmlFileOptionShortName = "xml";
            const std::string FIGAROSettings::figaroToDotOptionName = "to-dot";
            const std::string FIGAROSettings::figaroToDotOptionShortName = "dot";
            const std::string FIGAROSettings::figaroToExplicitOptionName = "to-explicit";
            const std::string FIGAROSettings::figaroToExplicitOptionShortName = "drn";
            const std::string FIGAROSettings::propertyOptionName = "prop";
            const std::string FIGAROSettings::propertyOptionShortName = "prop";
            
            FIGAROSettings::FIGAROSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, figaroFileOptionName, false, "Parses the figaro program.").setShortName(figaroFileOptionShortame).addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").addValidatorString(ArgumentValidatorFactory::createExistingFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, xmlFileOptionName, false, "Parse the XML file").setShortName(xmlFileOptionShortName).addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").addValidatorString(ArgumentValidatorFactory::createWritableFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, figaroToDotOptionName, false, "Destination for the figaro model dot output.").setShortName(figaroToDotOptionShortName).addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, figaroToExplicitOptionName, false, "Destination for the figaro model drn output.").setShortName(figaroToExplicitOptionShortName).addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, propertyOptionName, false, "Specifies the properties to be checked on the model.").setShortName(propertyOptionShortName)
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("property or filename", "The formula or the file containing the formulas.").build())
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filter", "The names of the properties to check.").setDefaultValueString("all").makeOptional().build())
                        .build());
            }
            
            bool FIGAROSettings::isfigaroFileSet() const {
                return this->getOption(figaroFileOptionName).getHasOptionBeenSet();
            }
            
            std::string FIGAROSettings::getfigaroFilename() const {
                return this->getOption(figaroFileOptionName).getArgumentByName("filename").getValueAsString();
            }
//
            bool FIGAROSettings::isToDotSet() const {
                return this->getOption(figaroToDotOptionName).getHasOptionBeenSet();
            }
//
            std::string FIGAROSettings::getFigaroDotOutputFilename() const {
                return this->getOption(figaroToDotOptionName).getArgumentByName("filename").getValueAsString();
            }
//
//
            bool FIGAROSettings::isFigaroToExplicitSet() const {
                return this->getOption(figaroToExplicitOptionShortName).getHasOptionBeenSet();
            }

            std::string FIGAROSettings::getFigaroExplicitOutputFilename() const {
                return this->getOption(figaroToExplicitOptionName).getArgumentByName("filename").getValueAsString();
            }

//
            bool FIGAROSettings::isPropertyInputSet() const {
                return this->getOption(propertyOptionName).getHasOptionBeenSet();
            }
//
            std::string FIGAROSettings::getPropertyInput() const {
                return this->getOption(propertyOptionName).getArgumentByName("property or filename").getValueAsString();
            }

            std::string FIGAROSettings::getPropertyInputFilter() const {
                return this->getOption(propertyOptionName).getArgumentByName("filter").getValueAsString();
            }

            void FIGAROSettings::finalize() {

            }

            bool FIGAROSettings::check() const {
                return true;
            }
        }
    }
}
