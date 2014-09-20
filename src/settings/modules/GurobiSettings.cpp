#include "src/settings/modules/GurobiSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string GurobiSettings::moduleName = "gurobi";
            const std::string GurobiSettings::integerToleranceOption = "inttol";
            const std::string GurobiSettings::threadsOption = "threads";
            const std::string GurobiSettings::outputOption = "output";
            
            GurobiSettings::GurobiSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager) {
                // First, we need to create all options of this module.
                std::vector<std::shared_ptr<Option>> options;
                options.push_back(storm::settings::OptionBuilder(moduleName, threadsOption, true, "The number of threads that may be used by Gurobi.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of threads.").setDefaultValueUnsignedInteger(1).build()).build());
                
                options.push_back(storm::settings::OptionBuilder(moduleName, outputOption, true, "If set, the Gurobi output will be printed to the command line.").build());
                
                options.push_back(storm::settings::OptionBuilder(moduleName, integerToleranceOption, true, "Sets Gurobi's precision for integer variables.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                // Finally, register all options that we just created.
                settingsManager.registerModule(moduleName, options);
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm