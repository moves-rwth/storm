#include "storm/settings/modules/MinMaxEquationSolverSettings.h"

#include "storm/settings/Option.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/OptionBuilder.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/IllegalArgumentValueException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string MinMaxEquationSolverSettings::moduleName = "minmax";
            const std::string MinMaxEquationSolverSettings::solvingMethodOptionName = "method";
            const std::string MinMaxEquationSolverSettings::maximalIterationsOptionName = "maxiter";
            const std::string MinMaxEquationSolverSettings::maximalIterationsOptionShortName = "i";
            const std::string MinMaxEquationSolverSettings::precisionOptionName = "precision";
            const std::string MinMaxEquationSolverSettings::absoluteOptionName = "absolute";
            const std::string MinMaxEquationSolverSettings::lraMethodOptionName = "lramethod";
            const std::string MinMaxEquationSolverSettings::valueIterationMultiplicationStyleOptionName = "vimult";

            MinMaxEquationSolverSettings::MinMaxEquationSolverSettings() : ModuleSettings(moduleName) {
                std::vector<std::string> minMaxSolvingTechniques = {"vi", "value-iteration", "pi", "policy-iteration", "linear-programming", "lp", "ratsearch"};
                this->addOption(storm::settings::OptionBuilder(moduleName, solvingMethodOptionName, false, "Sets which min/max linear equation solving technique is preferred.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of a min/max linear equation solving technique.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(minMaxSolvingTechniques)).setDefaultValueString("vi").build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, maximalIterationsOptionName, false, "The maximal number of iterations to perform before iterative solving is aborted.").setShortName(maximalIterationsOptionShortName).addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(20000).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, false, "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidatorDouble(ArgumentValidatorFactory::createDoubleRangeValidatorExcluding(0.0, 1.0)).build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, absoluteOptionName, false, "Sets whether the relative or the absolute error is considered for detecting convergence.").build());

                std::vector<std::string> lraMethods = {"vi", "value-iteration", "linear-programming", "lp"};
                this->addOption(storm::settings::OptionBuilder(moduleName, lraMethodOptionName, false, "Sets which method is preferred for computing long run averages.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of a long run average computation method.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(lraMethods)).setDefaultValueString("vi").build()).build());

                std::vector<std::string> multiplicationStyles = {"gaussseidel", "regular", "gs", "r"};
                this->addOption(storm::settings::OptionBuilder(moduleName, valueIterationMultiplicationStyleOptionName, false, "Sets which method multiplication style to prefer for value iteration.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of a multiplication style.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(multiplicationStyles)).setDefaultValueString("gaussseidel").build()).build());
            }
            
            storm::solver::MinMaxMethod MinMaxEquationSolverSettings::getMinMaxEquationSolvingMethod() const {
                std::string minMaxEquationSolvingTechnique = this->getOption(solvingMethodOptionName).getArgumentByName("name").getValueAsString();
                if (minMaxEquationSolvingTechnique == "value-iteration" || minMaxEquationSolvingTechnique == "vi") {
                    return storm::solver::MinMaxMethod::ValueIteration;
                } else if (minMaxEquationSolvingTechnique == "policy-iteration" || minMaxEquationSolvingTechnique == "pi") {
                    return storm::solver::MinMaxMethod::PolicyIteration;
                } else if (minMaxEquationSolvingTechnique == "linear-programming" || minMaxEquationSolvingTechnique == "lp") {
                    return storm::solver::MinMaxMethod::LinearProgramming;
                } else if (minMaxEquationSolvingTechnique == "ratsearch") {
                    return storm::solver::MinMaxMethod::RationalSearch;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown min/max equation solving technique '" << minMaxEquationSolvingTechnique << "'.");
            }
            
            bool MinMaxEquationSolverSettings::isMinMaxEquationSolvingMethodSetFromDefaultValue() const {
                return !this->getOption(solvingMethodOptionName).getArgumentByName("name").getHasBeenSet() || this->getOption(solvingMethodOptionName).getArgumentByName("name").wasSetFromDefaultValue();
            }
            
            bool MinMaxEquationSolverSettings::isMinMaxEquationSolvingMethodSet() const {
                return this->getOption(solvingMethodOptionName).getHasOptionBeenSet();
            }
            
            bool MinMaxEquationSolverSettings::isMaximalIterationCountSet() const {
                return this->getOption(maximalIterationsOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t MinMaxEquationSolverSettings::getMaximalIterationCount() const {
                return this->getOption(maximalIterationsOptionName).getArgumentByName("count").getValueAsUnsignedInteger();
            }
            
            bool MinMaxEquationSolverSettings::isPrecisionSet() const {
                return this->getOption(precisionOptionName).getHasOptionBeenSet();
            }
            
            double MinMaxEquationSolverSettings::getPrecision() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
            }
            
            bool MinMaxEquationSolverSettings::isConvergenceCriterionSet() const {
                return this->getOption(absoluteOptionName).getHasOptionBeenSet();
            }
            
            MinMaxEquationSolverSettings::ConvergenceCriterion MinMaxEquationSolverSettings::getConvergenceCriterion() const {
                return this->getOption(absoluteOptionName).getHasOptionBeenSet() ? MinMaxEquationSolverSettings::ConvergenceCriterion::Absolute : MinMaxEquationSolverSettings::ConvergenceCriterion::Relative;
            }
            
            storm::solver::LraMethod MinMaxEquationSolverSettings::getLraMethod() const {
                std::string lraMethodString = this->getOption(lraMethodOptionName).getArgumentByName("name").getValueAsString();
                if (lraMethodString == "value-iteration" || lraMethodString == "vi") {
                    return storm::solver::LraMethod::ValueIteration;
                } else if (lraMethodString == "linear-programming" || lraMethodString == "lp") {
                    return storm::solver::LraMethod::LinearProgramming;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown lra solving technique '" << lraMethodString << "'.");
            }

            storm::solver::MultiplicationStyle MinMaxEquationSolverSettings::getValueIterationMultiplicationStyle() const {
                std::string multiplicationStyleString = this->getOption(valueIterationMultiplicationStyleOptionName).getArgumentByName("name").getValueAsString();
                if (multiplicationStyleString == "gaussseidel" || multiplicationStyleString == "gs") {
                    return storm::solver::MultiplicationStyle::GaussSeidel;
                } else if (multiplicationStyleString == "regular" || multiplicationStyleString == "r") {
                    return storm::solver::MultiplicationStyle::Regular;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown multiplication style '" << multiplicationStyleString << "'.");
            }
            
        }
    }
}
