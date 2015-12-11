#include "src/settings/modules/BisimulationSettings.h"
#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/Argument.h"
#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string BisimulationSettings::moduleName = "bisimulation";
            const std::string BisimulationSettings::typeOptionName = "type";
            
            BisimulationSettings::BisimulationSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                std::vector<std::string> types = { "strong", "weak" };
                this->addOption(storm::settings::OptionBuilder(moduleName, typeOptionName, true, "Sets the kind of bisimulation quotienting used. Available are: { strong, weak }.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the type to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(types)).setDefaultValueString("strong").build()).build());
            }
            
            bool BisimulationSettings::isStrongBisimulationSet() const {
                if (this->getOption(typeOptionName).getArgumentByName("name").getValueAsString() == "strong") {
                    return true;
                }
                return false;
            }
            
            bool BisimulationSettings::isWeakBisimulationSet() const {
                if (this->getOption(typeOptionName).getArgumentByName("name").getValueAsString() == "weak") {
                    return true;
                }
                return false;
            }
            
            bool BisimulationSettings::check() const {
                bool optionsSet = this->getOption(typeOptionName).getHasOptionBeenSet();
                STORM_LOG_WARN_COND(storm::settings::generalSettings().isBisimulationSet() || !optionsSet, "Bisimulation minimization is not selected, so setting options for bisimulation has no effect.");
                return true;
            }
        } // namespace modules
    } // namespace settings
} // namespace storm