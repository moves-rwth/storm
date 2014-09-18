#include "src/settings/modules/GeneralSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            GeneralSettings::GeneralSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager) {
                settingsManager.addOption(storm::settings::OptionBuilder("general", "help", "h", "Shows all available options, arguments and descriptions.").build());
                settingsManager.addOption(storm::settings::OptionBuilder("general", "verbose", "v", "Be verbose.").build());
                settingsManager.addOption(storm::settings::OptionBuilder("general", "exportdot", "", "If specified, the loaded model will be written to the specified file in the dot format.")
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("dotFileName", "The file to export the model to.").build()).build());
                settingsManager.addOption(storm::settings::OptionBuilder("general", "config", "c", "If specified, this file will be read and parsed for additional configuration settings.")
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("configFileName", "The path and name of the file from which to read.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                settingsManager.addOption(storm::settings::OptionBuilder("general", "explicit", "e", "Explicit parsing from transition- and labeling files.")
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("transitionFileName", "The path and name of the file from which to read the transitions.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build())
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("labelingFileName", "The path and name of the file from which to read the labeling.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                settingsManager.addOption(storm::settings::OptionBuilder("general", "symbolic", "s", "Parse the given symbolic model file.")
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("symbolicFileName", "The path and name of the file from which to read the symbolic model.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                settingsManager.addOption(storm::settings::OptionBuilder("general", "prctl", "", "Performs model checking for the PRCTL formulas given in the file.")
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("prctlFileName", "The file from which to read the PRCTL formulas.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                settingsManager.addOption(storm::settings::OptionBuilder("general", "csl", "", "Performs model checking for the CSL formulas given in the file.")
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("cslFileName", "The file from which to read the CSL formulas.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                settingsManager.addOption(storm::settings::OptionBuilder("general", "ltl", "", "Performs model checking for the LTL formulas given in the file.")
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("ltlFileName", "The file from which to read the LTL formulas.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                settingsManager.addOption(storm::settings::OptionBuilder("general", "counterExample", "", "Generates a counterexample for the given PRCTL formulas if not satisfied by the model")
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("outputPath", "The path to the directory to write the generated counterexample files to.").build()).build());
                settingsManager.addOption(storm::settings::OptionBuilder("general", "transitionRewards", "", "If specified, the transition rewards are read from this file and added to the explicit model. Note that this requires an explicit model.")
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("transitionRewardsFileName", "The file from which to read the transition rewards.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                settingsManager.addOption(storm::settings::OptionBuilder("general", "stateRewards", "", "If specified, the state rewards are read from this file and added to the explicit model. Note that this requires an explicit model.")
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("stateRewardsFileName", "The file from which to read the state rewards.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                settingsManager.addOption(storm::settings::OptionBuilder("general", "fixDeadlocks", "", "If the model contains deadlock states, setting this option will insert self-loops for these states.").build());
                settingsManager.addOption(storm::settings::OptionBuilder("general", "timeout", "t", "If specified, computation will abort after the given number of seconds.")
                                          .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("seconds", "The number of seconds after which to timeout.").setDefaultValueUnsignedInteger(0).build()).build());
                
                std::vector<std::string> linearEquationSolver;
                linearEquationSolver.push_back("gmm++");
                linearEquationSolver.push_back("native");
                settingsManager.addOption(storm::settings::OptionBuilder("general", "linsolver", "", "Sets which solver is preferred for solving systems of linear equations.")
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the solver to prefer. Available are: gmm++ and native.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(linearEquationSolver)).setDefaultValueString("gmm++").build()).build());
                
                std::vector<std::string> nondeterministicLinearEquationSolver;
                nondeterministicLinearEquationSolver.push_back("gmm++");
                nondeterministicLinearEquationSolver.push_back("native");
                settingsManager.addOption(storm::settings::OptionBuilder("general", "ndsolver", "", "Sets which solver is preferred for solving systems of linear equations arising from nondeterministic systems.")
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the solver to prefer. Available are: gmm++ and native.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(nondeterministicLinearEquationSolver)).setDefaultValueString("native").build()).build());
                
                std::vector<std::string> lpSolvers;
                lpSolvers.push_back("gurobi");
                lpSolvers.push_back("glpk");
                settingsManager.addOption(storm::settings::OptionBuilder("general", "lpsolver", "", "Sets which LP solver is preferred.")
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("LP solver name", "The name of an available LP solver. Valid values are gurobi and glpk.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(lpSolvers)).setDefaultValueString("glpk").build()).build());
                
                settingsManager.addOption(storm::settings::OptionBuilder("general", "constants", "const", "Specifies the constant replacements to use in symbolic models.")
                                          .addArgument(storm::settings::ArgumentBuilder::createStringArgument("constantString", "A comma separated list of constants and their value, e.g. a=1,b=2,c=3").setDefaultValueString("").build()).build());
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm