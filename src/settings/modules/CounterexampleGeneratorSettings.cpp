#include "src/settings/modules/CounterexampleGeneratorSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string CounterexampleGeneratorSettings::moduleName = "counterexample";
            const std::string CounterexampleGeneratorSettings::minimalCommandSetOptionName = "mincmd";
            const std::string CounterexampleGeneratorSettings::encodeReachabilityOptionName = "encreach";
            const std::string CounterexampleGeneratorSettings::schedulerCutsOptionName = "schedcuts";
            
            CounterexampleGeneratorSettings::CounterexampleGeneratorSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                std::vector<std::string> techniques = {"maxsat", "milp"};
                this->addOption(storm::settings::OptionBuilder(moduleName, minimalCommandSetOptionName, true, "Computes a counterexample for the given model in terms of a minimal command set. Note that this requires the model to be given in a symbolic format.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("method", "Sets which technique is used to derive the counterexample. Available are {milp, maxsat}").setDefaultValueString("maxsat").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(techniques)).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, encodeReachabilityOptionName, true, "Sets whether to encode reachability for MAXSAT-based minimal command counterexample generation.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, schedulerCutsOptionName, true, "Sets whether to add the scheduler cuts for MILP-based minimal command counterexample generation.").build());
            }
            
            bool CounterexampleGeneratorSettings::isMinimalCommandGenerationSet() const {
                return this->getOption(minimalCommandSetOptionName).getHasOptionBeenSet();
            }
            
            
            bool CounterexampleGeneratorSettings::isUseMilpBasedMinimalCommandSetGenerationSet() const {
                return this->getOption(minimalCommandSetOptionName).getArgumentByName("method").getValueAsString() == "milp";
            }
            
            bool CounterexampleGeneratorSettings::isUseMaxSatBasedMinimalCommandSetGenerationSet() const {
                return this->getOption(minimalCommandSetOptionName).getArgumentByName("method").getValueAsString() == "maxsat";
            }
            
            bool CounterexampleGeneratorSettings::isEncodeReachabilitySet() const {
                return this->getOption(encodeReachabilityOptionName).getHasOptionBeenSet();
            }
            
            bool CounterexampleGeneratorSettings::isUseSchedulerCutsSet() const {
                return this->getOption(schedulerCutsOptionName).getHasOptionBeenSet();
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm