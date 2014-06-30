#include "src/utility/StormOptions.h"

bool storm::utility::StormOptions::optionsRegistered = storm::settings::Settings::registerNewModule([] (storm::settings::Settings* settings) -> bool {

	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "help", "h", "Shows all available options, arguments and descriptions.").build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "verbose", "v", "Be verbose.").build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "debug", "", "Be very verbose (intended for debugging).").build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "trace", "", "Be extremly verbose (intended for debugging, heavy performance impacts).").build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "logfile", "l", "If specified, the log output will also be written to this file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("logFileName", "The path and name of the file to write to.").build()).build());
    
    settings->addOption(storm::settings::OptionBuilder("StoRM Main", "exportdot", "", "If specified, the loaded model will be written to the specified file in the dot format.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("dotFileName", "The file to export the model to.").build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "configfile", "c", "If specified, this file will be read and parsed for additional configuration settings.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("configFileName", "The path and name of the file from which to read.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "explicit", "", "Explicit parsing from transition- and labeling files.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("transitionFileName", "The path and name of the file from which to read the transitions.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).addArgument(storm::settings::ArgumentBuilder::createStringArgument("labelingFileName", "The path and name of the file from which to read the labeling.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "symbolic", "", "Parse the given symbolic model file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("symbolicFileName", "The path and name of the file from which to read the symbolic model.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "prctl", "", "Performs model checking for the PRCTL formulas given in the file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("prctlFileName", "The file from which to read the PRCTL formulas.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "csl", "", "Performs model checking for the CSL formulas given in the file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("cslFileName", "The file from which to read the CSL formulas.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "ltl", "", "Performs model checking for the LTL formulas given in the file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("ltlFileName", "The file from which to read the LTL formulas.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "counterExample", "", "Generates a counterexample for the given PRCTL formulas if not satisfied by the model").addArgument(storm::settings::ArgumentBuilder::createStringArgument("outputPath", "The path to the directory to write the generated counterexample files to.").build()).build());

	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "transitionRewards", "", "If specified, the transition rewards are read from this file and added to the explicit model. Note that this requires an explicit model.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("transitionRewardsFileName", "The file from which to read the transition rewards.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "stateRewards", "", "If specified, the state rewards are read from this file and added to the explicit model. Note that this requires an explicit model.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("stateRewardsFileName", "The file from which to read the state rewards.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "fixDeadlocks", "", "If the model contains deadlock states, setting this option will insert self-loops for these states.").build());
    
    settings->addOption(storm::settings::OptionBuilder("StoRM Main", "timeout", "t", "If specified, computation will abort after the given number of seconds.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("seconds", "The number of seconds after which to timeout.").setDefaultValueUnsignedInteger(0).build()).build());
    
	std::vector<std::string> linearEquationSolver;
	linearEquationSolver.push_back("gmm++");
	linearEquationSolver.push_back("native");
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "linsolver", "", "Sets which solver is preferred for solving systems of linear equations.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the solver to prefer. Available are: gmm++ and native.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(linearEquationSolver)).setDefaultValueString("gmm++").build()).build());

    std::vector<std::string> nondeterministicLinearEquationSolver;
	nondeterministicLinearEquationSolver.push_back("gmm++");
	nondeterministicLinearEquationSolver.push_back("native");
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "ndsolver", "", "Sets which solver is preferred for solving systems of linear equations arising from nondeterministic systems.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the solver to prefer. Available are: gmm++ and native.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(nondeterministicLinearEquationSolver)).setDefaultValueString("native").build()).build());

    std::vector<std::string> lpSolvers;
	lpSolvers.push_back("gurobi");
	lpSolvers.push_back("glpk");
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "lpsolver", "", "Sets which LP solver is preferred.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("LP solver name", "The name of an available LP solver. Valid values are gurobi and glpk.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(lpSolvers)).setDefaultValueString("glpk").build()).build());
    
	return true;
});
