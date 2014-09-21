#include "src/settings/modules/NativeEquationSolverSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string NativeEquationSolverSettings::moduleName = "native";
            const std::string NativeEquationSolverSettings::techniqueOptionName = "tech";
            const std::string NativeEquationSolverSettings::maximalIterationsOptionName = "maxiter";
            const std::string NativeEquationSolverSettings::maximalIterationsOptionShortName = "i";
            const std::string NativeEquationSolverSettings::precisionOptionName = "precision";
            const std::string NativeEquationSolverSettings::absoluteOptionName = "absolute";
            
            NativeEquationSolverSettings::NativeEquationSolverSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                std::vector<std::string> methods = { "jacobi" };
                this->addAndRegisterOption(storm::settings::OptionBuilder(moduleName, techniqueOptionName, true, "The method to be used for solving linear equation systems with the native engine. Available are: { jacobi }.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the method to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(methods)).setDefaultValueString("jacobi").build()).build());
                
                this->addAndRegisterOption(storm::settings::OptionBuilder(moduleName, maximalIterationsOptionName, true, "The maximal number of iterations to perform before iterative solving is aborted.").setShortName(maximalIterationsOptionShortName).addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(10000).build()).build());
                
                this->addAndRegisterOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, true, "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                this->addAndRegisterOption(storm::settings::OptionBuilder(moduleName, absoluteOptionName, true, "Sets whether the relative or the absolute error is considered for detecting convergence.").build());
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm