#include "storm/settings/modules/BisimulationSettings.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"
#include "storm/settings/SettingsManager.h"

#include "storm/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string BisimulationSettings::moduleName = "bisimulation";
            const std::string BisimulationSettings::typeOptionName = "type";
            
            BisimulationSettings::BisimulationSettings() : ModuleSettings(moduleName) {
                std::vector<std::string> types = { "strong", "weak" };
                this->addOption(storm::settings::OptionBuilder(moduleName, typeOptionName, true, "Sets the kind of bisimulation quotienting used.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the type to use.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(types)).setDefaultValueString("strong").build()).build());
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
                STORM_LOG_WARN_COND(storm::settings::getModule<storm::settings::modules::GeneralSettings>().isBisimulationSet() || !optionsSet, "Bisimulation minimization is not selected, so setting options for bisimulation has no effect.");
                return true;
            }
        } // namespace modules
    } // namespace settings
} // namespace storm
