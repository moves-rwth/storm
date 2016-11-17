#include "src/settings/modules/TopologicalValueIterationEquationSolverSettings.h"

#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/Argument.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"
#include "src/solver/SolverSelectionOptions.h"

namespace storm {
    namespace settings {
        namespace modules {
            
			const std::string TopologicalValueIterationEquationSolverSettings::moduleName = "topologicalValueIteration";
			const std::string TopologicalValueIterationEquationSolverSettings::maximalIterationsOptionName = "maxiter";
			const std::string TopologicalValueIterationEquationSolverSettings::maximalIterationsOptionShortName = "i";
			const std::string TopologicalValueIterationEquationSolverSettings::precisionOptionName = "precision";
			const std::string TopologicalValueIterationEquationSolverSettings::absoluteOptionName = "absolute";

			TopologicalValueIterationEquationSolverSettings::TopologicalValueIterationEquationSolverSettings() : ModuleSettings(moduleName) {
                
                this->addOption(storm::settings::OptionBuilder(moduleName, maximalIterationsOptionName, false, "The maximal number of iterations to perform before iterative solving is aborted.").setShortName(maximalIterationsOptionShortName).addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(20000).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, false, "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, absoluteOptionName, false, "Sets whether the relative or the absolute error is considered for detecting convergence.").build());
            }
            
			bool TopologicalValueIterationEquationSolverSettings::isMaximalIterationCountSet() const {
                return this->getOption(maximalIterationsOptionName).getHasOptionBeenSet();
            }
            
			uint_fast64_t TopologicalValueIterationEquationSolverSettings::getMaximalIterationCount() const {
                return this->getOption(maximalIterationsOptionName).getArgumentByName("count").getValueAsUnsignedInteger();
            }
            
			bool TopologicalValueIterationEquationSolverSettings::isPrecisionSet() const {
                return this->getOption(precisionOptionName).getHasOptionBeenSet();
            }
            
			double TopologicalValueIterationEquationSolverSettings::getPrecision() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
			}

			bool TopologicalValueIterationEquationSolverSettings::isConvergenceCriterionSet() const {
				return this->getOption(absoluteOptionName).getHasOptionBeenSet();
			}

			TopologicalValueIterationEquationSolverSettings::ConvergenceCriterion TopologicalValueIterationEquationSolverSettings::getConvergenceCriterion() const {
				return this->getOption(absoluteOptionName).getHasOptionBeenSet() ? TopologicalValueIterationEquationSolverSettings::ConvergenceCriterion::Absolute : TopologicalValueIterationEquationSolverSettings::ConvergenceCriterion::Relative;
			}
            
			bool TopologicalValueIterationEquationSolverSettings::check() const {
                return true;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm