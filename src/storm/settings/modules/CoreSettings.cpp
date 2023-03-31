#include "storm/settings/modules/CoreSettings.h"

#include "storm/settings/Argument.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/SettingsManager.h"
#include "storm/solver/SolverSelectionOptions.h"

#include "storm/adapters/IntelTbbAdapter.h"
#include "storm/storage/dd/DdType.h"

#include "storm/exceptions/IllegalArgumentValueException.h"
#include "storm/exceptions/InvalidOptionException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace settings {
namespace modules {

const std::string CoreSettings::moduleName = "core";
const std::string CoreSettings::eqSolverOptionName = "eqsolver";
const std::string CoreSettings::lpSolverOptionName = "lpsolver";
const std::string CoreSettings::smtSolverOptionName = "smtsolver";
const std::string CoreSettings::statisticsOptionName = "statistics";
const std::string CoreSettings::statisticsOptionShortName = "stats";
const std::string CoreSettings::engineOptionName = "engine";
const std::string CoreSettings::engineOptionShortName = "e";
const std::string CoreSettings::ddLibraryOptionName = "ddlib";
const std::string CoreSettings::cudaOptionName = "cuda";
const std::string CoreSettings::intelTbbOptionName = "enable-tbb";
const std::string CoreSettings::intelTbbOptionShortName = "tbb";

CoreSettings::CoreSettings() : ModuleSettings(moduleName), engine(storm::utility::Engine::Sparse) {
    std::vector<std::string> engines;
    for (auto e : storm::utility::getEngines()) {
        engines.push_back(storm::utility::toString(e));
    }
    engines.push_back("portfolio");  // for backwards compatibility

    this->addOption(storm::settings::OptionBuilder(moduleName, engineOptionName, false, "Sets which engine is used for model building and model checking.")
                        .setShortName(engineOptionShortName)
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the engine to use.")
                                         .addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(engines))
                                         .setDefaultValueString("sparse")
                                         .build())
                        .build());

    std::vector<std::string> linearEquationSolver = {"gmm++", "native", "eigen", "elimination", "topological", "acyclic"};
    this->addOption(
        storm::settings::OptionBuilder(moduleName, eqSolverOptionName, false, "Sets which solver is preferred for solving systems of linear equations.")
            .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the solver to prefer.")
                             .addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(linearEquationSolver))
                             .setDefaultValueString("topological")
                             .build())
            .build());

    std::vector<std::string> ddLibraries = {"cudd", "sylvan"};
    this->addOption(storm::settings::OptionBuilder(moduleName, ddLibraryOptionName, false, "Sets which library is preferred for decision-diagram operations.")
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the library to prefer.")
                                         .addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(ddLibraries))
                                         .setDefaultValueString("sylvan")
                                         .build())
                        .build());

    std::vector<std::string> lpSolvers = {"gurobi", "glpk", "z3", "soplex"};
    this->addOption(storm::settings::OptionBuilder(moduleName, lpSolverOptionName, false, "Sets which LP solver is preferred.")
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of an LP solver.")
                                         .addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(lpSolvers))
                                         .setDefaultValueString("glpk")
                                         .build())
                        .build());

    std::vector<std::string> smtSolvers = {"z3", "mathsat"};
    this->addOption(storm::settings::OptionBuilder(moduleName, smtSolverOptionName, false, "Sets which SMT solver is preferred.")
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of an SMT solver.")
                                         .addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(smtSolvers))
                                         .setDefaultValueString("z3")
                                         .build())
                        .build());
    this->addOption(storm::settings::OptionBuilder(moduleName, statisticsOptionName, false, "Sets whether to display statistics if available.")
                        .setShortName(statisticsOptionShortName)
                        .build());

    this->addOption(storm::settings::OptionBuilder(moduleName, cudaOptionName, false, "Sets whether to use CUDA.").setIsAdvanced().build());
    this->addOption(
        storm::settings::OptionBuilder(moduleName, intelTbbOptionName, false, "Sets whether to use Intel TBB (if Storm was built with support for TBB).")
            .setShortName(intelTbbOptionShortName)
            .build());
}

storm::solver::EquationSolverType CoreSettings::getEquationSolver() const {
    std::string equationSolverName = this->getOption(eqSolverOptionName).getArgumentByName("name").getValueAsString();
    if (equationSolverName == "gmm++") {
        return storm::solver::EquationSolverType::Gmmxx;
    } else if (equationSolverName == "native") {
        return storm::solver::EquationSolverType::Native;
    } else if (equationSolverName == "eigen") {
        return storm::solver::EquationSolverType::Eigen;
    } else if (equationSolverName == "elimination") {
        return storm::solver::EquationSolverType::Elimination;
    } else if (equationSolverName == "topological") {
        return storm::solver::EquationSolverType::Topological;
    } else if (equationSolverName == "acyclic") {
        return storm::solver::EquationSolverType::Acyclic;
    }
    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown equation solver '" << equationSolverName << "'.");
}

bool CoreSettings::isEquationSolverSet() const {
    return this->getOption(eqSolverOptionName).getHasOptionBeenSet();
}

bool CoreSettings::isEquationSolverSetFromDefaultValue() const {
    return !this->getOption(eqSolverOptionName).getHasOptionBeenSet() || this->getOption(eqSolverOptionName).getArgumentByName("name").wasSetFromDefaultValue();
}

storm::solver::LpSolverType CoreSettings::getLpSolver() const {
    std::string lpSolverName = this->getOption(lpSolverOptionName).getArgumentByName("name").getValueAsString();
    if (lpSolverName == "gurobi") {
        return storm::solver::LpSolverType::Gurobi;
    } else if (lpSolverName == "glpk") {
        return storm::solver::LpSolverType::Glpk;
    } else if (lpSolverName == "z3") {
        return storm::solver::LpSolverType::Z3;
    } else if (lpSolverName == "soplex") {
        return storm::solver::LpSolverType::Soplex;
    }
    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown LP solver '" << lpSolverName << "'.");
}

bool CoreSettings::isLpSolverSetFromDefaultValue() const {
    return !this->getOption(lpSolverOptionName).getHasOptionBeenSet() || this->getOption(lpSolverOptionName).getArgumentByName("name").wasSetFromDefaultValue();
}

storm::solver::SmtSolverType CoreSettings::getSmtSolver() const {
    std::string smtSolverName = this->getOption(smtSolverOptionName).getArgumentByName("name").getValueAsString();
    if (smtSolverName == "z3") {
        return storm::solver::SmtSolverType::Z3;
    } else if (smtSolverName == "mathsat") {
        return storm::solver::SmtSolverType::Mathsat;
    }
    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown SMT solver '" << smtSolverName << "'.");
}

storm::dd::DdType CoreSettings::getDdLibraryType() const {
    std::string ddLibraryAsString = this->getOption(ddLibraryOptionName).getArgumentByName("name").getValueAsString();
    if (ddLibraryAsString == "sylvan") {
        return storm::dd::DdType::Sylvan;
    } else {
        return storm::dd::DdType::CUDD;
    }
}

bool CoreSettings::isDdLibraryTypeSetFromDefaultValue() const {
    return !this->getOption(ddLibraryOptionName).getArgumentByName("name").getHasBeenSet() ||
           this->getOption(ddLibraryOptionName).getArgumentByName("name").wasSetFromDefaultValue();
}

bool CoreSettings::isShowStatisticsSet() const {
    return this->getOption(statisticsOptionName).getHasOptionBeenSet();
}

bool CoreSettings::isUseIntelTbbSet() const {
    return this->getOption(intelTbbOptionName).getHasOptionBeenSet();
}

bool CoreSettings::isUseCudaSet() const {
    return this->getOption(cudaOptionName).getHasOptionBeenSet();
}

storm::utility::Engine CoreSettings::getEngine() const {
    return engine;
}

void CoreSettings::setEngine(storm::utility::Engine const& newEngine) {
    this->engine = newEngine;
}

void CoreSettings::finalize() {
    // Finalize engine.
    std::string engineStr = this->getOption(engineOptionName).getArgumentByName("name").getValueAsString();
    engine = storm::utility::engineFromString(engineStr);
    STORM_LOG_THROW(engine != storm::utility::Engine::Unknown, storm::exceptions::IllegalArgumentValueException, "Unknown engine '" << engineStr << "'.");
}

bool CoreSettings::check() const {
#ifdef STORM_HAVE_INTELTBB
    return true;
#else
    STORM_LOG_WARN_COND(!isUseIntelTbbSet(), "Enabling TBB is not supported in this version of Storm as it was not built with support for it.");
    return true;
#endif
}

}  // namespace modules
}  // namespace settings
}  // namespace storm
