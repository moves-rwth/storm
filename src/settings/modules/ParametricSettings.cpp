#include "src/settings/modules/ParametricSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string ParametricSettings::moduleName = "parametric";
            const std::string ParametricSettings::entryStatesLastOptionName = "entrylast";
            const std::string ParametricSettings::maximalSccSizeOptionName = "sccsize";
            
            ParametricSettings::ParametricSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, entryStatesLastOptionName, true, "Sets whether the entry states are eliminated last.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, maximalSccSizeOptionName, true, "Sets the maximal size of the SCCs for which state elimination is applied.")
                                .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("maxsize", "The maximal size of an SCC on which state elimination is applied.").setDefaultValueUnsignedInteger(50).setIsOptional(true).build()).build());
            }
            
            bool ParametricSettings::isEliminateEntryStatesLastSet() const {
                return this->getOption(entryStatesLastOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t ParametricSettings::getMaximalSccSize() const {
                return this->getOption(maximalSccSizeOptionName).getArgumentByName("maxsize").getValueAsUnsignedInteger();
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm