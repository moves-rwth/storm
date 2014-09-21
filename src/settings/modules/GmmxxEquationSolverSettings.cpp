#include "src/settings/modules/GmmxxEquationSolverSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string GmmxxEquationSolverSettings::moduleName = "gmm++";
            const std::string GmmxxEquationSolverSettings::techniqueOptionName = "tech";
            const std::string GmmxxEquationSolverSettings::preconditionOptionName = "precond";
            const std::string GmmxxEquationSolverSettings::restartOptionName = "restart";
            const std::string GmmxxEquationSolverSettings::maximalIterationsOptionName = "maxiter";
            const std::string GmmxxEquationSolverSettings::maximalIterationsOptionShortName = "i";
            const std::string GmmxxEquationSolverSettings::precisionOptionName = "precision";
            const std::string GmmxxEquationSolverSettings::absoluteOptionName = "absolute";

            GmmxxEquationSolverSettings::GmmxxEquationSolverSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                std::vector<std::string> methods = {"bicgstab", "qmr", "gmres", "jacobi"};
                this->addAndRegisterOption(storm::settings::OptionBuilder(moduleName, techniqueOptionName, true, "The method to be used for solving linear equation systems with the gmm++ engine. Available are {bicgstab, qmr, gmres, jacobi}.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the method to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(methods)).setDefaultValueString("gmres").build()).build());
                
                // Register available preconditioners.
                std::vector<std::string> preconditioner = {"ilu", "diagonal", "ildlt", "none"};
                this->addAndRegisterOption(storm::settings::OptionBuilder(moduleName, preconditionOptionName, true, "The preconditioning technique used for solving linear equation systems. Available are {ilu, diagonal, none}.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the preconditioning method.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(preconditioner)).setDefaultValueString("ilu").build()).build());
                
                this->addAndRegisterOption(storm::settings::OptionBuilder(moduleName, restartOptionName, true, "The number of iteration until restarted methods are actually restarted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of iterations.").setDefaultValueUnsignedInteger(50).build()).build());
                
                this->addAndRegisterOption(storm::settings::OptionBuilder(moduleName, maximalIterationsOptionName, true, "The maximal number of iterations to perform before iterative solving is aborted.").setShortName(maximalIterationsOptionShortName).addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("iterations", "The maximal iteration count.").setDefaultValueUnsignedInteger(10000).build()).build());
                
                this->addAndRegisterOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, true, "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                this->addAndRegisterOption(storm::settings::OptionBuilder(moduleName, absoluteOptionName, true, "Sets whether the relative or the absolute error is considered for detecting convergence.").build());
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm