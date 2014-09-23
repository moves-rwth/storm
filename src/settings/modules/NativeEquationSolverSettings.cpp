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
                this->addOption(storm::settings::OptionBuilder(moduleName, techniqueOptionName, true, "The method to be used for solving linear equation systems with the native engine. Available are: { jacobi }.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the method to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(methods)).setDefaultValueString("jacobi").build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, maximalIterationsOptionName, false, "The maximal number of iterations to perform before iterative solving is aborted.").setShortName(maximalIterationsOptionShortName).addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(20000).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, false, "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, absoluteOptionName, false, "Sets whether the relative or the absolute error is considered for detecting convergence.").build());
            }
            
            NativeEquationSolverSettings::LinearEquationTechnique NativeEquationSolverSettings::getLinearEquationSystemTechnique() const {
                std::string linearEquationSystemTechniqueAsString = this->getOption(techniqueOptionName).getArgumentByName("name").getValueAsString();
                if (linearEquationSystemTechniqueAsString == "jacobi") {
                    return NativeEquationSolverSettings::LinearEquationTechnique::Jacobi;
                }
                LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown solution technique '" << linearEquationSystemTechniqueAsString << "' selected.");
            }
            
            uint_fast64_t NativeEquationSolverSettings::getMaximalIterationCount() const {
                return this->getOption(maximalIterationsOptionName).getArgumentByName("count").getValueAsUnsignedInteger();
            }
            
            double NativeEquationSolverSettings::getPrecision() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
            }
            
            NativeEquationSolverSettings::ConvergenceCriterion NativeEquationSolverSettings::getConvergenceCriterion() const {
                return this->getOption(absoluteOptionName).getHasOptionBeenSet() ? NativeEquationSolverSettings::ConvergenceCriterion::Absolute : NativeEquationSolverSettings::ConvergenceCriterion::Relative;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm