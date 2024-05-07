#include "storm-pars/settings/modules/RegionVerificationSettings.h"

#include "storm-pars/modelchecker/region/RegionSplittingStrategy.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/OptionBuilder.h"

#include "storm/exceptions/IllegalArgumentValueException.h"
#include "storm/utility/macros.h"

namespace storm::settings::modules {

const std::string RegionVerificationSettings::moduleName = "regionverif";
const std::string splittingThresholdName = "splitting-threshold";
const std::string splittingStrategyName = "splitting-strategy";
const std::string checkEngineOptionName = "engine";

RegionVerificationSettings::RegionVerificationSettings() : ModuleSettings(moduleName) {
    this->addOption(
        storm::settings::OptionBuilder(moduleName, splittingThresholdName, false, "Sets the threshold for number of parameters in which to split regions.")
            .addArgument(
                storm::settings::ArgumentBuilder::createIntegerArgument("splitting-threshold", "The threshold for splitting, should be an integer > 0").build())
            .build());

    std::vector<std::string> strategies = {"estimate", "roundrobin"};
    this->addOption(storm::settings::OptionBuilder(moduleName, splittingStrategyName, false, "Sets which strategy is used for splitting regions.")
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the strategy to use.")
                                         .addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(strategies))
                                         .setDefaultValueString("estimate")
                                         .build())
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


storm::modelchecker::RegionSplittingStrategy::Heuristic RegionVerificationSettings::getRegionSplittingStrategy() const {
    std::string strategyString = this->getOption(splittingStrategyName).getArgumentByName("name").getValueAsString();

    storm::modelchecker::RegionSplittingStrategy::Heuristic result;
    if (strategyString == "estimate") {
        result = storm::modelchecker::RegionSplittingStrategy::Heuristic::EstimateBased;
    } else if (strategyString == "roundrobin") {
        result = storm::modelchecker::RegionSplittingStrategy::Heuristic::RoundRobin;
    } else {
        STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown splitting strategy '" << strategyString << "'.");
    }

    return result;
}

}  // namespace storm::settings::modules
