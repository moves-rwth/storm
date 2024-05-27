#include "storm/settings/modules/DebugSettings.h"

#include "storm/settings/Argument.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/SettingsManager.h"

namespace storm {
namespace settings {
namespace modules {

const std::string DebugSettings::moduleName = "debug";
const std::string DebugSettings::debugOptionName = "debug";
const std::string DebugSettings::traceOptionName = "trace";
const std::string DebugSettings::additionalChecksOptionName = "additional-checks";
const std::string DebugSettings::logfileOptionName = "logfile";
const std::string DebugSettings::logfileOptionShortName = "l";
const std::string DebugSettings::testOptionName = "test";
const std::string DebugSettings::forceMECDecompositionAlgorithmName = "benchmarkForceMECDecompositionAlgorithm";

DebugSettings::DebugSettings() : ModuleSettings(moduleName) {
    this->addOption(storm::settings::OptionBuilder(moduleName, debugOptionName, false, "Print debug output.").build());
    this->addOption(storm::settings::OptionBuilder(moduleName, traceOptionName, false, "Print even more debug output.").build());
    this->addOption(
        storm::settings::OptionBuilder(moduleName, additionalChecksOptionName, false, "If set, additional sanity checks are performed during execution.")
            .build());
    this->addOption(storm::settings::OptionBuilder(moduleName, logfileOptionName, false, "If specified, the log output will also be written to this file.")
                        .setShortName(logfileOptionShortName)
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file to write the log.").build())
                        .build());
    this->addOption(storm::settings::OptionBuilder(moduleName, testOptionName, false, "Activate a test setting.").setIsAdvanced().build());
    this->addOption(storm::settings::OptionBuilder(moduleName, forceMECDecompositionAlgorithmName, false,
                                                   "Forces symbolic model on MDP with an mec decomposition with a specific symbolic decomposition algorithm.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument(
                                         "value", "1\t\tNaive\n2\t\tLockstep\n3\t\tCollapsing\n4\t\tMyalgo") // TODO change myalgo to nice name
                                         .build())
                        .build());
}

bool DebugSettings::isDebugSet() const {
    return this->getOption(debugOptionName).getHasOptionBeenSet();
}

bool DebugSettings::isTraceSet() const {
    return this->getOption(traceOptionName).getHasOptionBeenSet();
}

bool DebugSettings::isAdditionalChecksSet() const {
    return this->getOption(additionalChecksOptionName).getHasOptionBeenSet();
}

bool DebugSettings::isLogfileSet() const {
    return this->getOption(logfileOptionName).getHasOptionBeenSet();
}

std::string DebugSettings::getLogfilename() const {
    return this->getOption(logfileOptionName).getArgumentByName("filename").getValueAsString();
}

bool DebugSettings::isTestSet() const {
    return this->getOption(testOptionName).getHasOptionBeenSet();
}

uint_fast64_t DebugSettings::forceMECDecompositionAlgorithm() const {
    if (this->getOption(forceMECDecompositionAlgorithmName).getHasOptionBeenSet()) {
        return this->getOption(forceMECDecompositionAlgorithmName).getArgumentByName("value").getValueAsUnsignedInteger();
    } else {
        return 0;
    }
}

}  // namespace modules
}  // namespace settings
}  // namespace storm
