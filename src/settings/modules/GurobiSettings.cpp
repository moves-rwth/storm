#include "src/settings/modules/GurobiSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string GurobiSettings::moduleName = "gurobi";
            const std::string GurobiSettings::integerToleranceOption = "inttol";
            const std::string GurobiSettings::threadsOption = "threads";
            const std::string GurobiSettings::outputOption = "output";
            
            GurobiSettings::GurobiSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, threadsOption, true, "The number of threads that may be used by Gurobi.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of threads.").setDefaultValueUnsignedInteger(1).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, outputOption, true, "If set, the Gurobi output will be printed to the command line.").build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, integerToleranceOption, true, "Sets Gurobi's precision for integer variables.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
            }
            
            double GurobiSettings::getIntegerTolerance() const {
                return this->getOption(integerToleranceOption).getArgumentByName("value").getValueAsDouble();
            }
            
            uint_fast64_t GurobiSettings::getNumberOfThreads() const {
                return this->getOption(threadsOption).getArgumentByName("count").getValueAsUnsignedInteger();
            }
            
            bool GurobiSettings::isOutputSet() const {
                return this->getOption(outputOption).getHasOptionBeenSet();
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm