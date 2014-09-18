#include "src/settings/modules/NativeEquationSolverSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            NativeEquationSolverSettings::NativeEquationSolverSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager) {
                // Offer all available methods as a command line option.
                std::vector<std::string> methods;
                methods.clear();
                methods.push_back("jacobi");
                settingsManager.addOption(storm::settings::OptionBuilder("NativeLinearEquationSolver", "nativelin", "", "The method to be used for solving linear equation systems with the native engine. Available are: jacobi.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the method to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(methods)).setDefaultValueString("jacobi").build()).build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("NativeLinearEquationSolver", "maxiter", "i", "The maximal number of iterations to perform before iterative solving is aborted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(10000).build()).build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("NativeLinearEquationSolver", "precision", "", "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("NativeLinearEquationSolver", "absolute", "", "Whether the relative or the absolute error is considered for deciding convergence.").build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("NativeNondeterminsticLinearEquationSolver", "maxiter", "i", "The maximal number of iterations to perform before iterative solving is aborted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(10000).build()).build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("NativeNondeterminsticLinearEquationSolver", "precision", "", "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("NativeNondeterminsticLinearEquationSolver", "absolute", "", "Whether the relative or the absolute error is considered for deciding convergence.").build());
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm