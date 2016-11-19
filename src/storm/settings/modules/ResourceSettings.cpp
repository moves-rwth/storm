#include "storm/settings/modules/ResourceSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string ResourceSettings::moduleName = "resources";
            const std::string ResourceSettings::timeoutOptionName = "timeout";
            const std::string ResourceSettings::timeoutOptionShortName = "t";
            
            ResourceSettings::ResourceSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, timeoutOptionName, false, "If given, computation will abort after the timeout has been reached.").setShortName(timeoutOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("time", "The number of seconds after which to timeout.").setDefaultValueUnsignedInteger(0).build()).build());
            }
            
            bool ResourceSettings::isTimeoutSet() const {
                return this->getOption(timeoutOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t ResourceSettings::getTimeoutInSeconds() const {
                return this->getOption(timeoutOptionName).getArgumentByName("time").getValueAsUnsignedInteger();
            }
            
        }
    }
}
