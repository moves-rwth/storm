#include "src/settings/modules/CounterexampleGeneratorSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            CounterexampleGeneratorSettings::CounterexampleGeneratorSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager) {
                std::vector<std::string> techniques;
                techniques.push_back("sat");
                techniques.push_back("milp");
                settingsManager.addOption(storm::settings::OptionBuilder("CounterexampleGenerator", "mincmd", "", "Computes a counterexample for the given symbolic model in terms of a minimal command set.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("propertyFile", "The file containing the properties for which counterexamples are to be generated.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build())
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("method", "Sets which technique is used to derive the counterexample. Must be either \"milp\" or \"sat\".").setDefaultValueString("sat").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(techniques)).build()).build());
                settingsManager.addOption(storm::settings::OptionBuilder("CounterexampleGenerator", "stats", "", "Sets whether to display statistics for certain functionalities.").build());
                settingsManager.addOption(storm::settings::OptionBuilder("CounterexampleGenerator", "encreach", "", "Sets whether to encode reachability for SAT-based minimal command counterexample generation.").build());
                settingsManager.addOption(storm::settings::OptionBuilder("CounterexampleGenerator", "schedcuts", "", "Sets whether to add the scheduler cuts for MILP-based minimal command counterexample generation.").build());
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm