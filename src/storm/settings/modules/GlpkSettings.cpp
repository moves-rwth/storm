#include "src/storm/settings/modules/GlpkSettings.h"
#include "src/storm/settings/Option.h"
#include "src/storm/settings/OptionBuilder.h"
#include "src/storm/settings/ArgumentBuilder.h"
#include "src/storm/settings/Argument.h"

#include "src/storm/settings/SettingsManager.h"
#include "src/storm/settings/modules/CoreSettings.h"
#include "src/storm/solver/SolverSelectionOptions.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string GlpkSettings::moduleName = "glpk";
            const std::string GlpkSettings::integerToleranceOption = "inttol";
            const std::string GlpkSettings::outputOptionName = "output";
            
            GlpkSettings::GlpkSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, outputOptionName, true, "If set, the glpk output will be printed to the command line.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, integerToleranceOption, true, "Sets glpk's precision for integer variables.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
            }
            
            bool GlpkSettings::isOutputSet() const {
                return this->getOption(outputOptionName).getHasOptionBeenSet();
            }
            
            bool GlpkSettings::isIntegerToleranceSet() const {
                return this->getOption(integerToleranceOption).getHasOptionBeenSet();
            }
            
            double GlpkSettings::getIntegerTolerance() const {
                return this->getOption(integerToleranceOption).getArgumentByName("value").getValueAsDouble();
            }
            
            bool GlpkSettings::check() const {
                if (isOutputSet() || isIntegerToleranceSet()) {
                    STORM_LOG_WARN_COND(storm::settings::getModule<storm::settings::modules::CoreSettings>().getLpSolver() == storm::solver::LpSolverType::Glpk, "glpk is not selected as the preferred LP solver, so setting options for glpk might have no effect.");
                }
                
                return true;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm
