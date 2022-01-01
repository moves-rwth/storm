#include "storm/settings/modules/MultiplierSettings.h"

#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"

#include "storm/exceptions/IllegalArgumentValueException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace settings {
namespace modules {

const std::string MultiplierSettings::moduleName = "multiplier";
const std::string MultiplierSettings::multiplierTypeOptionName = "type";

MultiplierSettings::MultiplierSettings() : ModuleSettings(moduleName) {
    std::vector<std::string> multiplierTypes = {"native", "gmmxx"};
    this->addOption(storm::settings::OptionBuilder(moduleName, multiplierTypeOptionName, true, "Sets which type of multiplier is preferred.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of a multiplier.")
                                         .addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(multiplierTypes))
                                         .setDefaultValueString("gmmxx")
                                         .build())
                        .build());
}

storm::solver::MultiplierType MultiplierSettings::getMultiplierType() const {
    std::string type = this->getOption(multiplierTypeOptionName).getArgumentByName("name").getValueAsString();
    if (type == "native") {
        return storm::solver::MultiplierType::Native;
    } else if (type == "gmmxx") {
        return storm::solver::MultiplierType::Gmmxx;
    }

    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown multiplier type '" << type << "'.");
}

bool MultiplierSettings::isMultiplierTypeSetFromDefaultValue() const {
    return !this->getOption(multiplierTypeOptionName).getArgumentByName("name").getHasBeenSet() ||
           this->getOption(multiplierTypeOptionName).getArgumentByName("name").wasSetFromDefaultValue();
}
}  // namespace modules
}  // namespace settings
}  // namespace storm
