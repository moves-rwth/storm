#include "src/settings/modules/GurobiSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            GurobiSettings::GurobiSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager) {
                settingsManager.addOption(storm::settings::OptionBuilder("GurobiLpSolver", "glpkinttol", "", "Sets glpk's precision for integer variables.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                
                settingsManager.addOption(storm::settings::OptionBuilder("GurobiLpSolver", "gurobithreads", "", "The number of threads that may be used by Gurobi.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of threads.").setDefaultValueUnsignedInteger(1).build()).build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("GurobiLpSolver", "gurobioutput", "", "If set, the Gurobi output will be printed to the command line.").build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("GurobiLpSolver", "gurobiinttol", "", "Sets Gurobi's precision for integer variables.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm