#include "storm/settings/modules/GurobiSettings.h"

#include "storm/settings/Argument.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/solver/SolverSelectionOptions.h"
namespace storm {
namespace settings {
namespace modules {

const std::string GurobiSettings::moduleName = "gurobi";
const std::string GurobiSettings::integerToleranceOption = "inttol";
const std::string GurobiSettings::threadsOption = "threads";
const std::string GurobiSettings::outputOption = "output";
const std::string GurobiSettings::mipFocusOption = "mipfocus";
const std::string GurobiSettings::concurrentMipThreadsOption = "concurrentmip";

GurobiSettings::GurobiSettings() : ModuleSettings(moduleName) {
    this->addOption(
        storm::settings::OptionBuilder(moduleName, threadsOption, true, "The number of threads that may be used by Gurobi.")
            .setIsAdvanced()
            .addArgument(
                storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of threads.").setDefaultValueUnsignedInteger(1).build())
            .build());

    this->addOption(storm::settings::OptionBuilder(moduleName, outputOption, true, "If set, the Gurobi output will be printed to the command line.")
                        .setIsAdvanced()
                        .build());

    this->addOption(storm::settings::OptionBuilder(moduleName, integerToleranceOption, true, "Sets Gurobi's precision for integer variables.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.")
                                         .setDefaultValueDouble(1e-06)
                                         .addValidatorDouble(ArgumentValidatorFactory::createDoubleRangeValidatorExcluding(0.0, 1.0))
                                         .build())
                        .build());

    this->addOption(storm::settings::OptionBuilder(moduleName, mipFocusOption, true, "The high level solution strategy used to solve MILPs.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("value", "The number of the strategy.")
                                         .setDefaultValueUnsignedInteger(0)
                                         .addValidatorUnsignedInteger(ArgumentValidatorFactory::createUnsignedRangeValidatorIncluding(0, 3))
                                         .build())
                        .build());

    this->addOption(storm::settings::OptionBuilder(moduleName, concurrentMipThreadsOption, true,
                                                   "The number of MIP solvers Gurobi spawns in parallel. Shall not be larger then the number of threads.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("value", "The number of parallel solvers.")
                                         .setDefaultValueUnsignedInteger(1)
                                         .addValidatorUnsignedInteger(ArgumentValidatorFactory::createUnsignedGreaterEqualValidator(1))
                                         .build())
                        .build());
}

bool GurobiSettings::isIntegerToleranceSet() const {
    return this->getOption(integerToleranceOption).getHasOptionBeenSet();
}

double GurobiSettings::getIntegerTolerance() const {
    return this->getOption(integerToleranceOption).getArgumentByName("value").getValueAsDouble();
}

bool GurobiSettings::isNumberOfThreadsSet() const {
    return this->getOption(threadsOption).getHasOptionBeenSet();
}

uint_fast64_t GurobiSettings::getNumberOfThreads() const {
    return this->getOption(threadsOption).getArgumentByName("count").getValueAsUnsignedInteger();
}

bool GurobiSettings::isOutputSet() const {
    return this->getOption(outputOption).getHasOptionBeenSet();
}

uint_fast64_t GurobiSettings::getMIPFocus() const {
    return this->getOption(mipFocusOption).getArgumentByName("value").getValueAsUnsignedInteger();
}

uint_fast64_t GurobiSettings::getNumberOfConcurrentMipThreads() const {
    return this->getOption(concurrentMipThreadsOption).getArgumentByName("value").getValueAsUnsignedInteger();
}

bool GurobiSettings::check() const {
    if (isOutputSet() || isIntegerToleranceSet() || isNumberOfThreadsSet()) {
        STORM_LOG_WARN_COND(storm::settings::getModule<storm::settings::modules::CoreSettings>().getLpSolver() == storm::solver::LpSolverType::Gurobi,
                            "Gurobi is not selected as the preferred LP solver, so setting options for Gurobi might have no effect.");
    }
    STORM_LOG_WARN_COND(getNumberOfConcurrentMipThreads() <= getNumberOfThreads(),
                        "Requested more concurrent MILP solvers then the available threads for Gurobi.");
    return true;
}

}  // namespace modules
}  // namespace settings
}  // namespace storm
