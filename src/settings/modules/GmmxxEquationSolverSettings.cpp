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
                this->addOption(storm::settings::OptionBuilder(moduleName, techniqueOptionName, true, "The method to be used for solving linear equation systems with the gmm++ engine. Available are {bicgstab, qmr, gmres, jacobi}.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the method to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(methods)).setDefaultValueString("gmres").build()).build());
                
                // Register available preconditioners.
                std::vector<std::string> preconditioner = {"ilu", "diagonal", "none"};
                this->addOption(storm::settings::OptionBuilder(moduleName, preconditionOptionName, true, "The preconditioning technique used for solving linear equation systems. Available are {ilu, diagonal, none}.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the preconditioning method.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(preconditioner)).setDefaultValueString("ilu").build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, restartOptionName, true, "The number of iteration until restarted methods are actually restarted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of iterations.").setDefaultValueUnsignedInteger(50).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, maximalIterationsOptionName, true, "The maximal number of iterations to perform before iterative solving is aborted.").setShortName(maximalIterationsOptionShortName).addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(20000).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, true, "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, absoluteOptionName, true, "Sets whether the relative or the absolute error is considered for detecting convergence.").build());
            }
            
            GmmxxEquationSolverSettings::LinearEquationTechnique GmmxxEquationSolverSettings::getLinearEquationSystemTechnique() const {
                std::string linearEquationSystemTechniqueAsString = this->getOption(techniqueOptionName).getArgumentByName("name").getValueAsString();
                if (linearEquationSystemTechniqueAsString == "bicgstab") {
                    return GmmxxEquationSolverSettings::LinearEquationTechnique::Bicgstab;
                } else if (linearEquationSystemTechniqueAsString == "qmr") {
                    return GmmxxEquationSolverSettings::LinearEquationTechnique::Qmr;
                } else if (linearEquationSystemTechniqueAsString == "gmres") {
                    return GmmxxEquationSolverSettings::LinearEquationTechnique::Gmres;
                } else if (linearEquationSystemTechniqueAsString == "jacobi") {
                    return GmmxxEquationSolverSettings::LinearEquationTechnique::Jacobi;
                }
                LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown solution technique '" << linearEquationSystemTechniqueAsString << "' selected.");
            }
            
            GmmxxEquationSolverSettings::PreconditioningTechnique GmmxxEquationSolverSettings::getPreconditioningTechnique() const {
                std::string preconditioningTechniqueAsString = this->getOption(preconditionOptionName).getArgumentByName("name").getValueAsString();
                if (preconditioningTechniqueAsString == "ilu") {
                    return GmmxxEquationSolverSettings::PreconditioningTechnique::Ilu;
                } else if (preconditioningTechniqueAsString == "diagonal") {
                    return GmmxxEquationSolverSettings::PreconditioningTechnique::Diagonal;
                } else if (preconditioningTechniqueAsString == "none") {
                    return GmmxxEquationSolverSettings::PreconditioningTechnique::None;
                }
                LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown preconditioning technique '" << preconditioningTechniqueAsString << "' selected.");
            }
            
            uint_fast64_t GmmxxEquationSolverSettings::getRestartIterationCount() const {
                return this->getOption(restartOptionName).getArgumentByName("count").getValueAsUnsignedInteger();
            }
            
            uint_fast64_t GmmxxEquationSolverSettings::getMaximalIterationCount() const {
                return this->getOption(maximalIterationsOptionName).getArgumentByName("count").getValueAsUnsignedInteger();
            }
            
            double GmmxxEquationSolverSettings::getPrecision() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
            }
            
            GmmxxEquationSolverSettings::ConvergenceCriterion GmmxxEquationSolverSettings::getConvergenceCriterion() const {
                return this->getOption(absoluteOptionName).getHasOptionBeenSet() ? GmmxxEquationSolverSettings::ConvergenceCriterion::Absolute : GmmxxEquationSolverSettings::ConvergenceCriterion::Relative;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm