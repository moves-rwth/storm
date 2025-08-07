#include "storm-pars/settings/modules/RegionSettings.h"

#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/OptionBuilder.h"

#include "storm/exceptions/IllegalArgumentValueException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace settings {
namespace modules {

const std::string RegionSettings::moduleName = "region";
const std::string regionOptionName = "region";
const std::string regionShortOptionName = "reg";
const std::string regionBoundOptionName = "regionbound";

RegionSettings::RegionSettings() : ModuleSettings(moduleName) {
    this->addOption(storm::settings::OptionBuilder(moduleName, regionOptionName, false, "Sets the region(s) considered for analysis.")
                        .setShortName(regionShortOptionName)
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument(
                                         "regioninput", "The region(s) given in format a<=x<=b,c<=y<=d seperated by ';'. Can also be a file.")
                                         .build())
                        .build());

    this->addOption(storm::settings::OptionBuilder(moduleName, regionBoundOptionName, false, "Sets the region bound considered for analysis.")
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument(
                                         "regionbound", "The bound for the region result for all variables: 0+bound <= var <=1-bound")
                                         .build())
                        .build());
}

bool RegionSettings::isRegionSet() const {
    return this->getOption(regionOptionName).getHasOptionBeenSet();
}

std::string RegionSettings::getRegionString() const {
    return this->getOption(regionOptionName).getArgumentByName("regioninput").getValueAsString();
}

bool RegionSettings::isRegionBoundSet() const {
    return this->getOption(regionBoundOptionName).getHasOptionBeenSet();
}

std::string RegionSettings::getRegionBoundString() const {
    return this->getOption(regionBoundOptionName).getArgumentByName("regionbound").getValueAsString();
}

}  // namespace modules
}  // namespace settings
}  // namespace storm
