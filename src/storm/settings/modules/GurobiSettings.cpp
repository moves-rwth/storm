#include "storm/settings/modules/GurobiSettings.h"

#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/solver/SolverSelectionOptions.h"
namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string GurobiSettings::moduleName = "gurobi";
            const std::string GurobiSettings::integerToleranceOption = "inttol";
            const std::string GurobiSettings::threadsOption = "threads";
            const std::string GurobiSettings::outputOption = "output";
            
            GurobiSettings::GurobiSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, threadsOption, true, "The number of threads that may be used by Gurobi.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of threads.").setDefaultValueUnsignedInteger(1).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, outputOption, true, "If set, the Gurobi output will be printed to the command line.").build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, integerToleranceOption, true, "Sets Gurobi's precision for integer variables.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
            }
            
            bool GurobiSettings::isIntegerToleranceSet() const {
                return this->getOption(integerToleranceOption).getHasOptionBeenSet();
            }
            
            double GurobiSettings::getIntegerTolerance() const {
                return this->getOption(integerToleranceOption).getArgumentByName("value").getValueAsDouble();
            }
            
            bool GurobiSettings::isNumberOfThreadsSet() const {
                return this->getOption(threadsOption).getHasOptionBeenSet();
            }
            
            uint_fast64_t GurobiSettings::getNumberOfThreads() const {
                return this->getOption(threadsOption).getArgumentByName("count").getValueAsUnsignedInteger();
            }
            
            bool GurobiSettings::isOutputSet() const {
                return this->getOption(outputOption).getHasOptionBeenSet();
            }
            
            bool GurobiSettings::check() const {
                if (isOutputSet() || isIntegerToleranceSet() || isNumberOfThreadsSet()) {
                    STORM_LOG_WARN_COND(storm::settings::getModule<storm::settings::modules::CoreSettings>().getLpSolver() == storm::solver::LpSolverType::Gurobi, "Gurobi is not selected as the preferred LP solver, so setting options for Gurobi might have no effect.");
                }
                
                return true;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm
