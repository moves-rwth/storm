#include "storm/settings/modules/EigenEquationSolverSettings.h"

#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/solver/SolverSelectionOptions.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/IllegalArgumentValueException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string EigenEquationSolverSettings::moduleName = "eigen";
            const std::string EigenEquationSolverSettings::techniqueOptionName = "method";
            const std::string EigenEquationSolverSettings::preconditionOptionName = "precond";
            const std::string EigenEquationSolverSettings::maximalIterationsOptionName = "maxiter";
            const std::string EigenEquationSolverSettings::maximalIterationsOptionShortName = "i";
            const std::string EigenEquationSolverSettings::precisionOptionName = "precision";
            const std::string EigenEquationSolverSettings::restartOptionName = "restart";
            
            EigenEquationSolverSettings::EigenEquationSolverSettings() : ModuleSettings(moduleName) {
                std::vector<std::string> methods = {"sparselu", "bicgstab", "dgmres", "gmres"};
                this->addOption(storm::settings::OptionBuilder(moduleName, techniqueOptionName, true, "The method to be used for solving linear equation systems with the eigen solver.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the method to use.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(methods)).setDefaultValueString("gmres").build()).build());
                
                // Register available preconditioners.
                std::vector<std::string> preconditioner = {"ilu", "diagonal", "none"};
                this->addOption(storm::settings::OptionBuilder(moduleName, preconditionOptionName, true, "The preconditioning technique used for solving linear equation systems.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the preconditioning method.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(preconditioner)).setDefaultValueString("ilu").build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, restartOptionName, true, "The number of iteration until restarted methods are actually restarted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of iterations.").setDefaultValueUnsignedInteger(50).build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, maximalIterationsOptionName, false, "The maximal number of iterations to perform before iterative solving is aborted.").setShortName(maximalIterationsOptionShortName).addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(20000).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, false, "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidatorDouble(ArgumentValidatorFactory::createDoubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
            }
            
            bool EigenEquationSolverSettings::isLinearEquationSystemMethodSet() const {
                return this->getOption(techniqueOptionName).getHasOptionBeenSet();
            }
            
            EigenEquationSolverSettings::LinearEquationMethod EigenEquationSolverSettings::getLinearEquationSystemMethod() const {
                std::string linearEquationSystemTechniqueAsString = this->getOption(techniqueOptionName).getArgumentByName("name").getValueAsString();
                if (linearEquationSystemTechniqueAsString == "sparselu") {
                    return EigenEquationSolverSettings::LinearEquationMethod::SparseLU;
                } else if (linearEquationSystemTechniqueAsString == "bicgstab") {
                    return EigenEquationSolverSettings::LinearEquationMethod::BiCGSTAB;
                } else if (linearEquationSystemTechniqueAsString == "dgmres") {
                    return EigenEquationSolverSettings::LinearEquationMethod::DGMRES;
                } else if (linearEquationSystemTechniqueAsString == "gmres") {
                    return EigenEquationSolverSettings::LinearEquationMethod::GMRES;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown solution technique '" << linearEquationSystemTechniqueAsString << "' selected.");
            }
            
            bool EigenEquationSolverSettings::isPreconditioningMethodSet() const {
                return this->getOption(preconditionOptionName).getHasOptionBeenSet();
            }
            
            EigenEquationSolverSettings::PreconditioningMethod EigenEquationSolverSettings::getPreconditioningMethod() const {
                std::string PreconditioningMethodAsString = this->getOption(preconditionOptionName).getArgumentByName("name").getValueAsString();
                if (PreconditioningMethodAsString == "ilu") {
                    return EigenEquationSolverSettings::PreconditioningMethod::Ilu;
                } else if (PreconditioningMethodAsString == "diagonal") {
                    return EigenEquationSolverSettings::PreconditioningMethod::Diagonal;
                } else if (PreconditioningMethodAsString == "none") {
                    return EigenEquationSolverSettings::PreconditioningMethod::None;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown preconditioning technique '" << PreconditioningMethodAsString << "' selected.");
            }
            
            bool EigenEquationSolverSettings::isRestartIterationCountSet() const {
                return this->getOption(restartOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t EigenEquationSolverSettings::getRestartIterationCount() const {
                return this->getOption(restartOptionName).getArgumentByName("count").getValueAsUnsignedInteger();
            }
            
            bool EigenEquationSolverSettings::isMaximalIterationCountSet() const {
                return this->getOption(maximalIterationsOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t EigenEquationSolverSettings::getMaximalIterationCount() const {
                return this->getOption(maximalIterationsOptionName).getArgumentByName("count").getValueAsUnsignedInteger();
            }
            
            bool EigenEquationSolverSettings::isPrecisionSet() const {
                return this->getOption(precisionOptionName).getHasOptionBeenSet();
            }
            
            double EigenEquationSolverSettings::getPrecision() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
            }
            
            bool EigenEquationSolverSettings::check() const {
                // This list does not include the precision, because this option is shared with other modules.
                bool optionsSet = isLinearEquationSystemMethodSet() || isPreconditioningMethodSet() || isMaximalIterationCountSet();
                
                STORM_LOG_WARN_COND(storm::settings::getModule<storm::settings::modules::CoreSettings>().getEquationSolver() == storm::solver::EquationSolverType::Gmmxx || !optionsSet, "eigen is not selected as the preferred equation solver, so setting options for eigen might have no effect.");
                
                return true;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm
