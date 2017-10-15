#include "storm/settings/modules/NativeEquationSolverSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"
#include "storm/solver/SolverSelectionOptions.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/IllegalArgumentValueException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string NativeEquationSolverSettings::moduleName = "native";
            const std::string NativeEquationSolverSettings::techniqueOptionName = "method";
            const std::string NativeEquationSolverSettings::omegaOptionName = "soromega";
            const std::string NativeEquationSolverSettings::maximalIterationsOptionName = "maxiter";
            const std::string NativeEquationSolverSettings::maximalIterationsOptionShortName = "i";
            const std::string NativeEquationSolverSettings::precisionOptionName = "precision";
            const std::string NativeEquationSolverSettings::absoluteOptionName = "absolute";
            const std::string NativeEquationSolverSettings::powerMethodMultiplicationStyleOptionName = "powmult";

            NativeEquationSolverSettings::NativeEquationSolverSettings() : ModuleSettings(moduleName) {
                std::vector<std::string> methods = { "jacobi", "gaussseidel", "sor", "walkerchae", "power", "ratsearch" };
                this->addOption(storm::settings::OptionBuilder(moduleName, techniqueOptionName, true, "The method to be used for solving linear equation systems with the native engine.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the method to use.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(methods)).setDefaultValueString("jacobi").build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, maximalIterationsOptionName, false, "The maximal number of iterations to perform before iterative solving is aborted.").setShortName(maximalIterationsOptionShortName).addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(20000).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, false, "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidatorDouble(ArgumentValidatorFactory::createDoubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, omegaOptionName, false, "The omega used for SOR.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The value of the SOR parameter.").setDefaultValueDouble(0.9).addValidatorDouble(ArgumentValidatorFactory::createDoubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, absoluteOptionName, false, "Sets whether the relative or the absolute error is considered for detecting convergence.").build());
                
                std::vector<std::string> multiplicationStyles = {"gaussseidel", "regular", "gs", "r"};
                this->addOption(storm::settings::OptionBuilder(moduleName, powerMethodMultiplicationStyleOptionName, false, "Sets which method multiplication style to prefer for the power method.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of a multiplication style.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(multiplicationStyles)).setDefaultValueString("gaussseidel").build()).build());
            }
            
            bool NativeEquationSolverSettings::isLinearEquationSystemTechniqueSet() const {
                return this->getOption(techniqueOptionName).getHasOptionBeenSet();
            }
            
            bool NativeEquationSolverSettings::isLinearEquationSystemTechniqueSetFromDefaultValue() const {
                return !this->getOption(techniqueOptionName).getHasOptionBeenSet() || this->getOption(techniqueOptionName).getArgumentByName("name").wasSetFromDefaultValue();
            }
            
            NativeEquationSolverSettings::LinearEquationMethod NativeEquationSolverSettings::getLinearEquationSystemMethod() const {
                std::string linearEquationSystemTechniqueAsString = this->getOption(techniqueOptionName).getArgumentByName("name").getValueAsString();
                if (linearEquationSystemTechniqueAsString == "jacobi") {
                    return NativeEquationSolverSettings::LinearEquationMethod::Jacobi;
                } else if (linearEquationSystemTechniqueAsString == "gaussseidel") {
                    return NativeEquationSolverSettings::LinearEquationMethod::GaussSeidel;
                } else if (linearEquationSystemTechniqueAsString == "sor") {
                    return NativeEquationSolverSettings::LinearEquationMethod::SOR;
                } else if (linearEquationSystemTechniqueAsString == "walkerchae") {
                    return NativeEquationSolverSettings::LinearEquationMethod::WalkerChae;
                } else if (linearEquationSystemTechniqueAsString == "power") {
                    return NativeEquationSolverSettings::LinearEquationMethod::Power;
                } else if (linearEquationSystemTechniqueAsString == "ratsearch") {
                    return NativeEquationSolverSettings::LinearEquationMethod::RationalSearch;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown solution technique '" << linearEquationSystemTechniqueAsString << "' selected.");
            }
            
            bool NativeEquationSolverSettings::isMaximalIterationCountSet() const {
                return this->getOption(maximalIterationsOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t NativeEquationSolverSettings::getMaximalIterationCount() const {
                return this->getOption(maximalIterationsOptionName).getArgumentByName("count").getValueAsUnsignedInteger();
            }
            
            bool NativeEquationSolverSettings::isPrecisionSet() const {
                return this->getOption(precisionOptionName).getHasOptionBeenSet();
            }
            
            double NativeEquationSolverSettings::getPrecision() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
            }

            double NativeEquationSolverSettings::getOmega() const {
                return this->getOption(omegaOptionName).getArgumentByName("value").getValueAsDouble();
            }
            
            bool NativeEquationSolverSettings::isConvergenceCriterionSet() const {
                return this->getOption(absoluteOptionName).getHasOptionBeenSet();
            }
            
            NativeEquationSolverSettings::ConvergenceCriterion NativeEquationSolverSettings::getConvergenceCriterion() const {
                return this->getOption(absoluteOptionName).getHasOptionBeenSet() ? NativeEquationSolverSettings::ConvergenceCriterion::Absolute : NativeEquationSolverSettings::ConvergenceCriterion::Relative;
            }
            
            storm::solver::MultiplicationStyle NativeEquationSolverSettings::getPowerMethodMultiplicationStyle() const {
                std::string multiplicationStyleString = this->getOption(powerMethodMultiplicationStyleOptionName).getArgumentByName("name").getValueAsString();
                if (multiplicationStyleString == "gaussseidel" || multiplicationStyleString == "gs") {
                    return storm::solver::MultiplicationStyle::GaussSeidel;
                } else if (multiplicationStyleString == "regular" || multiplicationStyleString == "r") {
                    return storm::solver::MultiplicationStyle::Regular;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown multiplication style '" << multiplicationStyleString << "'.");
            }
            
            bool NativeEquationSolverSettings::check() const {
                // This list does not include the precision, because this option is shared with other modules.
                bool optionSet = isLinearEquationSystemTechniqueSet() || isMaximalIterationCountSet() || isConvergenceCriterionSet();
                
                STORM_LOG_WARN_COND(storm::settings::getModule<storm::settings::modules::CoreSettings>().getEquationSolver() == storm::solver::EquationSolverType::Native || !optionSet, "Native is not selected as the preferred equation solver, so setting options for native might have no effect.");
                
                return true;
            }
            
            std::ostream& operator<<(std::ostream& out, NativeEquationSolverSettings::LinearEquationMethod const& method) {
                switch (method) {
                    case NativeEquationSolverSettings::LinearEquationMethod::Jacobi: out << "jacobi"; break;
                    case NativeEquationSolverSettings::LinearEquationMethod::GaussSeidel: out << "gaussseidel"; break;
                    case NativeEquationSolverSettings::LinearEquationMethod::SOR: out << "sor"; break;
                    case NativeEquationSolverSettings::LinearEquationMethod::WalkerChae: out << "walkerchae"; break;
                    case NativeEquationSolverSettings::LinearEquationMethod::Power: out << "power"; break;
                    case NativeEquationSolverSettings::LinearEquationMethod::RationalSearch: out << "ratsearch"; break;
                }
                return out;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm
