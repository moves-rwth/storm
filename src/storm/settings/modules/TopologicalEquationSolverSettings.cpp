#include "storm/settings/modules/TopologicalEquationSolverSettings.h"


#include "storm/settings/modules/CoreSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"
#include "storm/solver/SolverSelectionOptions.h"

#include "storm/storage/dd/DdType.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/IllegalArgumentValueException.h"
#include "storm/exceptions/InvalidOptionException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string TopologicalEquationSolverSettings::moduleName = "topological";
            const std::string TopologicalEquationSolverSettings::underlyingEquationSolverOptionName = "eqsolver";
            
            TopologicalEquationSolverSettings::TopologicalEquationSolverSettings() : ModuleSettings(moduleName) {
                std::vector<std::string> linearEquationSolver = {"gmm++", "native", "eigen", "elimination"};
                this->addOption(storm::settings::OptionBuilder(moduleName, underlyingEquationSolverOptionName, true, "Sets which solver is considered for solving the underlying equation systems.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the used solver.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(linearEquationSolver)).setDefaultValueString("gmm++").build()).build());
            }

            bool TopologicalEquationSolverSettings::isUnderlyingEquationSolverTypeSet() const {
                return this->getOption(underlyingEquationSolverOptionName).getHasOptionBeenSet();
            }
            
            bool TopologicalEquationSolverSettings::isUnderlyingEquationSolverTypeSetFromDefaultValue() const {
                return !this->getOption(underlyingEquationSolverOptionName).getHasOptionBeenSet() || this->getOption(underlyingEquationSolverOptionName).getArgumentByName("name").wasSetFromDefaultValue();
            }

            storm::solver::EquationSolverType  TopologicalEquationSolverSettings::getUnderlyingEquationSolverType() const {
                std::string equationSolverName = this->getOption(underlyingEquationSolverOptionName).getArgumentByName("name").getValueAsString();
                if (equationSolverName == "gmm++") {
                    return storm::solver::EquationSolverType::Gmmxx;
                } else if (equationSolverName == "native") {
                    return storm::solver::EquationSolverType::Native;
                } else if (equationSolverName == "eigen") {
                    return storm::solver::EquationSolverType::Eigen;
                } else if (equationSolverName == "elimination") {
                    return storm::solver::EquationSolverType::Elimination;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown underlying equation solver '" << equationSolverName << "'.");
            }
            
            bool TopologicalEquationSolverSettings::check() const {
                if (this->isUnderlyingEquationSolverTypeSet() && getUnderlyingEquationSolverType() == storm::solver::EquationSolverType::Topological) {
                    STORM_LOG_WARN("Underlying solver type of the topological solver can not be the topological solver.");
                    return false;
                }
                return true;
            }

        } // namespace modules
    } // namespace settings
} // namespace storm


