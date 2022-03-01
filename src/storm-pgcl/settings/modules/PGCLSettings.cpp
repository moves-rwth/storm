#include "PGCLSettings.h"

#include "storm/settings/Argument.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/SettingsManager.h"

#include "storm/exceptions/InvalidSettingsException.h"

namespace storm {
namespace settings {
namespace modules {
const std::string PGCLSettings::moduleName = "pgcl";

const std::string PGCLSettings::pgclFileOptionName = "pgclfile";
const std::string PGCLSettings::pgclFileOptionShortName = "pgcl";
const std::string PGCLSettings::pgclToJaniOptionName = "to-jani";
const std::string PGCLSettings::pgclToJaniOptionShortName = "tj";
const std::string PGCLSettings::programGraphToDotOptionName = "draw-program-graph";
const std::string PGCLSettings::programGraphToDotShortOptionName = "pg";
const std::string PGCLSettings::programVariableRestrictionsOptionName = "variable-restrictions";
const std::string PGCLSettings::programVariableRestrictionShortOptionName = "rvar";
const std::string PGCLSettings::propertyOptionName = "prop";
const std::string PGCLSettings::propertyOptionShortName = "prop";

PGCLSettings::PGCLSettings() : ModuleSettings(moduleName) {
    this->addOption(storm::settings::OptionBuilder(moduleName, pgclFileOptionName, false, "Parses the pgcl program.")
                        .setShortName(pgclFileOptionShortName)
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file")
                                         .addValidatorString(ArgumentValidatorFactory::createExistingFileValidator())
                                         .build())
                        .build());
    this->addOption(storm::settings::OptionBuilder(moduleName, pgclToJaniOptionName, false, "Transform to JANI.")
                        .setShortName(pgclToJaniOptionShortName)
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file")
                                         .addValidatorString(ArgumentValidatorFactory::createWritableFileValidator())
                                         .build())
                        .build());
    this->addOption(storm::settings::OptionBuilder(moduleName, programGraphToDotOptionName, false, "Destination for the program graph dot output.")
                        .setShortName(programGraphToDotShortOptionName)
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").build())
                        .build());
    this->addOption(storm::settings::OptionBuilder(moduleName, programVariableRestrictionsOptionName, false, "Restrictions of program variables")
                        .setShortName(programVariableRestrictionShortOptionName)
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("description", "description of the variable restrictions").build())
                        .build());
    this->addOption(
        storm::settings::OptionBuilder(moduleName, propertyOptionName, false, "Specifies the properties to be checked on the model.")
            .setShortName(propertyOptionShortName)
            .addArgument(
                storm::settings::ArgumentBuilder::createStringArgument("property or filename", "The formula or the file containing the formulas.").build())
            .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filter", "The names of the properties to check.")
                             .setDefaultValueString("all")
                             .makeOptional()
                             .build())
            .build());
}

bool PGCLSettings::isPgclFileSet() const {
    return this->getOption(pgclFileOptionName).getHasOptionBeenSet();
}

std::string PGCLSettings::getPgclFilename() const {
    return this->getOption(pgclFileOptionName).getArgumentByName("filename").getValueAsString();
}

bool PGCLSettings::isToJaniSet() const {
    return this->getOption(pgclToJaniOptionName).getHasOptionBeenSet();
}

std::string PGCLSettings::getWriteToJaniFilename() const {
    return this->getOption(pgclToJaniOptionName).getArgumentByName("filename").getValueAsString();
}

bool PGCLSettings::isProgramGraphToDotSet() const {
    return this->getOption(programGraphToDotOptionName).getHasOptionBeenSet();
}

std::string PGCLSettings::getProgramGraphDotOutputFilename() const {
    return this->getOption(programGraphToDotOptionName).getArgumentByName("filename").getValueAsString();
}

bool PGCLSettings::isProgramVariableRestrictionSet() const {
    return this->getOption(programVariableRestrictionsOptionName).getHasOptionBeenSet();
}

std::string PGCLSettings::getProgramVariableRestrictions() const {
    return this->getOption(programVariableRestrictionsOptionName).getArgumentByName("description").getValueAsString();
}

bool PGCLSettings::isPropertyInputSet() const {
    return this->getOption(propertyOptionName).getHasOptionBeenSet();
}

std::string PGCLSettings::getPropertyInput() const {
    return this->getOption(propertyOptionName).getArgumentByName("property or filename").getValueAsString();
}

std::string PGCLSettings::getPropertyInputFilter() const {
    return this->getOption(propertyOptionName).getArgumentByName("filter").getValueAsString();
}

void PGCLSettings::finalize() {}

bool PGCLSettings::check() const {
    return true;
}
}  // namespace modules
}  // namespace settings
}  // namespace storm
