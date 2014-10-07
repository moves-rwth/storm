#include "src/settings/modules/ParametricSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string ParametricSettings::moduleName = "parametric";
            const std::string ParametricSettings::entryStatesLastOptionName = "entrylast";
            const std::string ParametricSettings::maximalSccSizeOptionName = "sccsize";
            const std::string ParametricSettings::sortTrivialSccOptionName = "sorttrivial";
            
            ParametricSettings::ParametricSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, entryStatesLastOptionName, true, "Sets whether the entry states are eliminated last.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, sortTrivialSccOptionName, true, "Sets whether the trivial SCCs are to be eliminated in descending order with respect to their distances from the initial state.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, maximalSccSizeOptionName, true, "Sets the maximal size of the SCCs for which state elimination is applied.")
                                .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("maxsize", "The maximal size of an SCC on which state elimination is applied.").setDefaultValueUnsignedInteger(20).setIsOptional(true).build()).build());
            }
            
            bool ParametricSettings::isEliminateEntryStatesLastSet() const {
                return this->getOption(entryStatesLastOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t ParametricSettings::getMaximalSccSize() const {
                return this->getOption(maximalSccSizeOptionName).getArgumentByName("maxsize").getValueAsUnsignedInteger();
            }
            
            bool ParametricSettings::isSortTrivialSccsSet() const {
                return this->getOption(sortTrivialSccOptionName).getHasOptionBeenSet();
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm