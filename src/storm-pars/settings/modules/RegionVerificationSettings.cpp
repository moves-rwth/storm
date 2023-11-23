#include "storm-pars/settings/modules/RegionVerificationSettings.h"

#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/OptionBuilder.h"

#include "storm/exceptions/IllegalArgumentValueException.h"
#include "storm/utility/macros.h"

namespace storm::settings::modules {

const std::string RegionVerificationSettings::moduleName = "regionverif";
const std::string splittingThresholdName = "splitting-threshold";
const std::string checkEngineOptionName = "engine";

RegionVerificationSettings::RegionVerificationSettings() : ModuleSettings(moduleName) {
    this->addOption(
        storm::settings::OptionBuilder(moduleName, splittingThresholdName, false, "Sets the threshold for number of parameters in which to split regions.")
            .addArgument(
                storm::settings::ArgumentBuilder::createIntegerArgument("splitting-threshold", "The threshold for splitting, should be an integer > 0").build())
            .build());

    std::vector<std::string> engines = {"pl", "exactpl", "validatingpl"};
    this->addOption(storm::settings::OptionBuilder(moduleName, checkEngineOptionName, true, "Sets which engine is used for analyzing regions.")
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the engine to use.")
                                         .addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(engines))
                                         .setDefaultValueString("pl")
                                         .build())
                        .build());
}

int RegionVerificationSettings::getSplittingThreshold() const {
    return this->getOption(splittingThresholdName).getArgumentByName("splitting-threshold").getValueAsInteger();
}

bool RegionVerificationSettings::isSplittingThresholdSet() const {
    return this->getOption(splittingThresholdName).getHasOptionBeenSet();
}

storm::modelchecker::RegionCheckEngine RegionVerificationSettings::getRegionCheckEngine() const {
    std::string engineString = this->getOption(checkEngineOptionName).getArgumentByName("name").getValueAsString();

    storm::modelchecker::RegionCheckEngine result;
    if (engineString == "pl") {
        result = storm::modelchecker::RegionCheckEngine::ParameterLifting;
    } else if (engineString == "exactpl") {
        result = storm::modelchecker::RegionCheckEngine::ExactParameterLifting;
    } else if (engineString == "validatingpl") {
        result = storm::modelchecker::RegionCheckEngine::ValidatingParameterLifting;
    } else {
        STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown region check engine '" << engineString << "'.");
    }

    return result;
}

}  // namespace storm::settings::modules
