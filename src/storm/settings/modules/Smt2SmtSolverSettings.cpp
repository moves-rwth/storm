#include "storm/settings/modules/Smt2SmtSolverSettings.h"

#include "storm/settings/SettingsManager.h"

#include "storm/settings/Argument.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"

namespace storm {
namespace settings {
namespace modules {

const std::string Smt2SmtSolverSettings::moduleName = "smt2";
const std::string Smt2SmtSolverSettings::solverCommandOption = "solvercommand";
const std::string Smt2SmtSolverSettings::exportScriptOption = "exportscript";

Smt2SmtSolverSettings::Smt2SmtSolverSettings() : ModuleSettings(moduleName) {
    this->addOption(storm::settings::OptionBuilder(moduleName, solverCommandOption, true,
                                                   "If set, this command is used to call the solver and to let the solver know that it should read SMT-LIBv2 "
                                                   "commands from standard input. If not set, only a SMT-LIB script file might be exported.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("command", "path to the solver + command line arguments.")
                                         .setDefaultValueString("")
                                         .build())
                        .build());

    this->addOption(storm::settings::OptionBuilder(moduleName, exportScriptOption, true, "If set, the SMT-LIBv2 script will be exportet to this file.")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument(
                                         "path", "path and filename to the location where the script file should be exported to.")
                                         .setDefaultValueString("")
                                         .build())
                        .build());
}

bool Smt2SmtSolverSettings::isSolverCommandSet() const {
    return this->getOption(solverCommandOption).getHasOptionBeenSet();
}

std::string Smt2SmtSolverSettings::getSolverCommand() const {
    return this->getOption(solverCommandOption).getArgumentByName("command").getValueAsString();
}

bool Smt2SmtSolverSettings::isExportSmtLibScriptSet() const {
    return this->getOption(exportScriptOption).getHasOptionBeenSet();
}

std::string Smt2SmtSolverSettings::getExportSmtLibScriptPath() const {
    return this->getOption(exportScriptOption).getArgumentByName("path").getValueAsString();
}

bool Smt2SmtSolverSettings::check() const {
    if (isSolverCommandSet() || isExportSmtLibScriptSet()) {
        // TODO check if paths are valid
    }
    return true;
}

}  // namespace modules
}  // namespace settings
}  // namespace storm
