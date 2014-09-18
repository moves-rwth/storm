#include "src/settings/modules/GmmxxSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            GmmxxSettings::GmmxxSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager) {
                settingsManager.addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "digiprecision", "", "Precision used for iterative solving of linear equation systems").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("precision value", "Precision").setDefaultValueDouble(1e-4).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                // Offer all available methods as a command line option.
                std::vector<std::string> methods;
                methods.push_back("bicgstab");
                methods.push_back("qmr");
                methods.push_back("gmres");
                methods.push_back("jacobi");
                settingsManager.addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "gmmlin", "", "The method to be used for solving linear equation systems with the gmm++ engine. Available are: bicgstab, qmr, gmres, jacobi.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the method to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(methods)).setDefaultValueString("gmres").build()).build());
                
                // Register available preconditioners.
                std::vector<std::string> preconditioner;
                preconditioner.push_back("ilu");
                preconditioner.push_back("diagonal");
                preconditioner.push_back("ildlt");
                preconditioner.push_back("none");
                settingsManager.addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "gmmpre", "", "The preconditioning technique used for solving linear equation systems with the gmm++ engine. Available are: ilu, diagonal, none.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the preconditioning method.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(preconditioner)).setDefaultValueString("ilu").build()).build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "gmmrestart", "", "The number of iteration until restarted methods are actually restarted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of iterations.").setDefaultValueUnsignedInteger(50).build()).build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "maxiter", "i", "The maximal number of iterations to perform before iterative solving is aborted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(10000).build()).build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "precision", "", "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "absolute", "", "Whether the relative or the absolute error is considered for deciding convergence.").build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("GmmxxNondeterminsticLinearEquationSolver", "maxiter", "i", "The maximal number of iterations to perform before iterative solving is aborted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(10000).build()).build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("GmmxxNondeterminsticLinearEquationSolver", "precision", "", "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-6).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("GmmxxNondeterminsticLinearEquationSolver", "absolute", "", "Whether the relative or the absolute error is considered for deciding convergence.").build());
                

            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm