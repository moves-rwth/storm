#include "storm/settings/modules/ModelCheckerSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"


namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string ModelCheckerSettings::moduleName = "modelchecker";
            const std::string ModelCheckerSettings::filterRewZeroOptionName = "filterrewzero";
            const std::string ModelCheckerSettings::ltl2daName = "ltl2da";

            ModelCheckerSettings::ModelCheckerSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, filterRewZeroOptionName, false, "If set, states with reward zero are filtered out, potentially reducing the size of the equation system").setIsAdvanced().build());
                this->addOption(storm::settings::OptionBuilder(moduleName, ltl2daName, false, "If set, use an external tool to convert LTL formulas to state-based deterministic automata in HOA format").setIsAdvanced().addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "A script that can be called with a prefix formula and a name for the output automaton.").build()).build());
            }
            
            bool ModelCheckerSettings::isFilterRewZeroSet() const {
                return this->getOption(filterRewZeroOptionName).getHasOptionBeenSet();
            }

            bool ModelCheckerSettings::isLtl2daSet() const {
                return this->getOption(ltl2daName).getHasOptionBeenSet();
            }

            std::string ModelCheckerSettings::getLtl2da() const {
                return this->getOption(ltl2daName).getArgumentByName("filename").getValueAsString();
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm
