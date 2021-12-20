#include "storm/settings/modules/SylvanSettings.h"

#include "storm/settings/SettingsManager.h"

#include "storm/settings/Argument.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"

namespace storm {
namespace settings {
namespace modules {

const std::string SylvanSettings::moduleName = "sylvan";
const std::string SylvanSettings::maximalMemoryOptionName = "maxmem";
const std::string SylvanSettings::threadCountOptionName = "threads";

SylvanSettings::SylvanSettings() : ModuleSettings(moduleName) {
    this->addOption(storm::settings::OptionBuilder(moduleName, maximalMemoryOptionName, true, "Sets the upper bound of memory available to Sylvan in MB.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("value", "The memory available to Sylvan.")
                                         .setDefaultValueUnsignedInteger(4096)
                                         .build())
                        .build());
    this->addOption(storm::settings::OptionBuilder(moduleName, threadCountOptionName, true, "Sets the number of threads used by Sylvan.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument(
                                         "value", "The number of threads available to Sylvan (0 means 'auto-detect').")
                                         .build())
                        .build());
}

uint_fast64_t SylvanSettings::getMaximalMemory() const {
    return this->getOption(maximalMemoryOptionName).getArgumentByName("value").getValueAsUnsignedInteger();
}

bool SylvanSettings::isNumberOfThreadsSet() const {
    return this->getOption(threadCountOptionName).getArgumentByName("value").getHasBeenSet();
}

uint_fast64_t SylvanSettings::getNumberOfThreads() const {
    return this->getOption(threadCountOptionName).getArgumentByName("value").getValueAsUnsignedInteger();
}

}  // namespace modules
}  // namespace settings
}  // namespace storm
